#include <fudge.h>
#include <abi.h>

#define TOKEN_SKIP                      1
#define TOKEN_END                       2
#define TOKEN_IDENT                     3
#define TOKEN_OPTION                    4
#define TOKEN_PIPE                      5

struct token
{

    unsigned int type;
    char *str;

};

struct tokenlist
{

    unsigned int head;
    unsigned int size;
    struct token *table;

};

static char stringdata[BUFFER_SIZE];
static struct ring stringtable;
static struct token infixdata[1024];
static struct token postfixdata[1024];
static struct token stackdata[8];
static struct tokenlist infix;
static struct tokenlist postfix;
static struct tokenlist stack;

static void tokenlist_init(struct tokenlist *list, unsigned int size, struct token *table)
{

    list->head = 0;
    list->size = size;
    list->table = table;

}

static void tokenlist_reset(struct tokenlist *list)
{

    list->head = 0;

}

static void tokenlist_push(struct tokenlist *list, struct token *token)
{

    list->table[list->head].type = token->type;
    list->table[list->head].str = token->str;

    if (list->head < list->size)
        list->head++;

}

static struct token *tokenlist_pop(struct tokenlist *list)
{

    return (list->head) ? &list->table[--list->head] : 0;

}

static unsigned int tokenlist_check(struct tokenlist *list)
{

    return (list->head) ? list->table[list->head - 1].type : 0;

}

static void tokenlist_add(struct tokenlist *list, unsigned int type, char *str)
{

    struct token token;

    token.type = type;
    token.str = str;

    tokenlist_push(list, &token);

}

static unsigned int precedence(struct token *token)
{

    switch (token->type)
    {

    case TOKEN_END:
        return 1;

    case TOKEN_PIPE:
        return 2;

    case TOKEN_OPTION:
        return 3;

    }

    return 0;

}

static unsigned int tokenize(char c)
{

    switch (c)
    {

    case ' ':
    case '\t':
        return TOKEN_SKIP;

    case '?':
        return TOKEN_OPTION;

    case '|':
        return TOKEN_PIPE;

    case ';':
    case '\n':
        return TOKEN_END;

    }

    return TOKEN_IDENT;

}

static unsigned int getidentlength(unsigned int count, char *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        unsigned int token = tokenize(buffer[i]);

        if (token != TOKEN_IDENT)
            break;

    }

    return i;

}

static void tokenizebuffer(struct tokenlist *infix, struct ring *stringtable, unsigned int count, void *buffer)
{

    char *data = buffer;
    unsigned int i;
    unsigned int c;

    for (i = 0; i < count; i++)
    {

        unsigned int token = tokenize(data[i]);

        switch (token)
        {

        case TOKEN_SKIP:
            continue;

        case TOKEN_IDENT:
            tokenlist_add(infix, token, stringtable->buffer + ring_count(stringtable));

            c = getidentlength(count - i, data + i);

            ring_write(stringtable, &data[i], c);
            ring_write(stringtable, "\0", 1);

            i += c - 1;

            break;

        default:
            tokenlist_add(infix, token, 0);

            break;

        }

    }

}

static void translate(struct tokenlist *postfix, struct tokenlist *infix, struct tokenlist *stack)
{

    unsigned int i;

    for (i = 0; i < infix->head; i++)
    {

        struct token *token = &infix->table[i];
        struct token *t;

        if (token->type == TOKEN_IDENT)
        {

            tokenlist_push(postfix, token);

            continue;

        }

        if (token->type == TOKEN_END || token->type == TOKEN_PIPE)
        {

            while ((t = tokenlist_pop(stack)))
                tokenlist_push(postfix, t);

            tokenlist_push(postfix, token);

            continue;

        }

        while ((t = tokenlist_pop(stack)))
        {

            if (precedence(token) > precedence(t))
            {

                tokenlist_push(stack, t);

                break;

            }

            tokenlist_push(postfix, t);

        }

        tokenlist_push(stack, token);

    }

}

static void parse(unsigned int source, struct tokenlist *postfix, struct tokenlist *stack)
{

    struct message message;
    unsigned int i;

    message_init(&message, EVENT_DATA);

    for (i = 0; i < postfix->head; i++)
    {

        struct token *token = &postfix->table[i];
        struct token *t;
        struct token *u;

        switch (token->type)
        {

        case TOKEN_IDENT:
            tokenlist_push(stack, token);

            break;

        case TOKEN_OPTION:
            t = tokenlist_pop(stack);

            if (!t)
                return;

            u = tokenlist_pop(stack);

            if (!u)
                return;

            message_putstringz(&message, "O");
            message_putstringz(&message, t->str);
            message_putstringz(&message, u->str);

            break;

        case TOKEN_PIPE:
            while (tokenlist_check(stack) == TOKEN_IDENT)
            {

                t = tokenlist_pop(stack);

                if (tokenlist_check(stack) == TOKEN_IDENT)
                {

                    message_putstringz(&message, "D");
                    message_putstringz(&message, t->str);

                }

                else
                {

                    message_putstringz(&message, "P");
                    message_putstringz(&message, t->str);

                }

            }

            break;

        case TOKEN_END:
            while (tokenlist_check(stack) == TOKEN_IDENT)
            {

                t = tokenlist_pop(stack);

                if (tokenlist_check(stack) == TOKEN_IDENT)
                {

                    message_putstringz(&message, "D");
                    message_putstringz(&message, t->str);

                }

                else
                {

                    message_putstringz(&message, "P");
                    message_putstringz(&message, t->str);

                }

            }

            message_putstringz(&message, "E");
            channel_sendmessage(&message);
            message_init(&message, EVENT_DATA);

            break;

        }

    }

}

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    if (msize)
    {

        ring_reset(&stringtable);
        tokenlist_reset(&infix);
        tokenlist_reset(&postfix);
        tokenlist_reset(&stack);
        tokenizebuffer(&infix, &stringtable, msize, mdata);
        translate(&postfix, &infix, &stack);
        parse(source, &postfix, &stack);

    }

}

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk2(FILE_L0, mdata))
    {

        char buffer[BUFFER_SIZE];
        unsigned int count;

        ring_reset(&stringtable);
        tokenlist_reset(&infix);
        tokenlist_reset(&postfix);
        tokenlist_reset(&stack);
        file_reset(FILE_L0);

        while ((count = file_read(FILE_L0, buffer, BUFFER_SIZE)))
            tokenizebuffer(&infix, &stringtable, count, buffer);

        translate(&postfix, &infix, &stack);
        parse(source, &postfix, &stack);

    }

}

void init(void)
{

    ring_init(&stringtable, BUFFER_SIZE, stringdata);
    tokenlist_init(&infix, 1024, infixdata);
    tokenlist_init(&postfix, 1024, postfixdata);
    tokenlist_init(&stack, 8, stackdata);
    channel_bind(EVENT_DATA, ondata);
    channel_bind(EVENT_PATH, onpath);

}

