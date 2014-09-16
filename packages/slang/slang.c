#include <fudge.h>

static unsigned int walk_path(unsigned int index, unsigned int indexw, unsigned int count, char *buffer)
{

    if (memory_match(buffer, "/", 1))
        return call_walk(index, CALL_DR, count - 1, buffer + 1);

    return call_walk(index, indexw, count, buffer);

}

struct stringtable
{

    unsigned int head;
    unsigned int size;
    char *buffer;

};

static void stringtable_init(struct stringtable *table, unsigned int size, char *buffer)
{

    memory_clear(table, sizeof (struct stringtable));

    table->size = size;
    table->buffer = buffer;

}

static unsigned int stringtable_push(struct stringtable *table, char c)
{

    table->buffer[table->head] = c;

    if (table->head < table->size)
        table->head++;

    return table->head;

}

enum token_type
{

    END,
    IDENT,
    IN,
    OUT,
    PIPE

};

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

static void tokenlist_init(struct tokenlist *list, unsigned int size, struct token *table)
{

    memory_clear(list, sizeof (struct tokenlist));

    list->size = size;
    list->table = table;

}

static void tokenlist_add(struct tokenlist *list, unsigned int type, char *str)
{

    list->table[list->head].type = type;
    list->table[list->head].str = str;

    if (list->head < list->size)
        list->head++;

}

static void tokenlist_copy(struct tokenlist *list, struct token *token)
{

    list->table[list->head].type = token->type;
    list->table[list->head].str = token->str;

    if (list->head < list->size)
        list->head++;

}

static struct token *tokenlist_pop(struct tokenlist *list)
{

    if (!list->head)
        return 0;

    list->head--;

    return &list->table[list->head];

}

static unsigned int precedence(struct token *token)
{

    switch (token->type)
    {

    case END:
        return 0;

    case PIPE:
        return 1;

    case IN:
    case OUT:
        return 2;

    }

    return 0;

}

static void tokenize(struct tokenlist *infix, struct stringtable *table, unsigned int count, char *buffer)
{

    unsigned int i;
    unsigned int ident;
    unsigned int identcount = 0;

    for (i = 0; i < count; i++)
    {

        char c = buffer[i];

        ident = 0;

        switch (c)
        {

        case ' ':
        case '\t':
            break;

        case '<':
            tokenlist_add(infix, IN, 0);

            break;

        case '>':
            tokenlist_add(infix, OUT, 0);

            break;

        case '|':
            tokenlist_add(infix, PIPE, 0);

            break;

        case ';':
        case '\n':
            tokenlist_add(infix, END, 0);

            break;

        default:
            ident = 1;

            if (!identcount)
                tokenlist_add(infix, IDENT, table->buffer + table->head);

            break;

        }

        if (ident)
        {

            stringtable_push(table, c);

            identcount++;

        }

        else
        {

            if (identcount)
            {

                stringtable_push(table, '\0');

                identcount = 0;

            }

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

        if (token->type == IDENT)
        {

            tokenlist_copy(postfix, token);

            continue;

        }

        if (token->type == END || token->type == PIPE)
        {

            while ((t = tokenlist_pop(stack)))
                tokenlist_copy(postfix, t);

            tokenlist_copy(postfix, token);

            continue;

        }

        while ((t = tokenlist_pop(stack)))
        {

            if (precedence(token) > precedence(t))
            {

                tokenlist_copy(stack, t);

                break;

            }

            tokenlist_copy(postfix, t);

        }

        tokenlist_copy(stack, token);

    }

}

static void parse(struct tokenlist *postfix, struct tokenlist *stack)
{

    unsigned int i;

    call_walk(CALL_I1, CALL_I0, 0, 0);
    call_walk(CALL_O1, CALL_O0, 0, 0);

    for (i = 0; i < postfix->head; i++)
    {

        struct token *token = &postfix->table[i];
        struct token *t;

        switch (token->type)
        {

        case IDENT:
            tokenlist_copy(stack, token);

            break;

        case IN:
            t = tokenlist_pop(stack);

            if (!t)
                return;

            if (!walk_path(CALL_I1, CALL_DW, ascii_length(t->str), t->str))
                return;

            break;

        case OUT:
            t = tokenlist_pop(stack);

            if (!t)
                return;

            if (!walk_path(CALL_O1, CALL_DW, ascii_length(t->str), t->str))
                return;

            break;

        case PIPE:
            t = tokenlist_pop(stack);

            if (!t)
                return;

            if (!walk_path(CALL_DP, CALL_L0, ascii_length(t->str), t->str))
                return;

            if (!walk_path(CALL_L1, CALL_DW, 13, "/system/pipe/"))
                return;

            call_open(CALL_L1);

            if (!walk_path(CALL_O1, CALL_L1, 1, "0"))
                return;

            if (!walk_path(CALL_I2, CALL_L1, 1, "1"))
                return;

            call_walk(CALL_O2, CALL_O0, 0, 0);
            call_spawn(CALL_DP);
            call_close(CALL_L1);

            break;

        case END:
            t = tokenlist_pop(stack);

            if (!t)
                return;

            if (!walk_path(CALL_DP, CALL_L0, ascii_length(t->str), t->str))
                return;

            call_spawn(CALL_DP);
            call_walk(CALL_I1, CALL_I0, 0, 0);
            call_walk(CALL_O1, CALL_O0, 0, 0);

            break;

        }

    }

}

void main()
{

    char buffer[FUDGE_BSIZE];
    unsigned int count, roff;
    char strtbl[32768];
    struct stringtable table;
    struct token infixtbl[1024];
    struct token postfixtbl[1024];
    struct token stacktbl[8];
    struct tokenlist infix;
    struct tokenlist postfix;
    struct tokenlist stack;

    stringtable_init(&table, 32768, strtbl);
    tokenlist_init(&infix, 1024, infixtbl);
    tokenlist_init(&postfix, 1024, postfixtbl);
    tokenlist_init(&stack, 8, stacktbl);

    if (!call_walk(CALL_L0, CALL_DR, 4, "bin/"))
        return;

    call_open(CALL_I0);

    for (roff = 0; (count = call_read(CALL_I0, roff, FUDGE_BSIZE, buffer)); roff += count)
        tokenize(&infix, &table, count, buffer);

    call_close(CALL_I0);

    if (stack.head)
        return;

    translate(&postfix, &infix, &stack);

    if (stack.head)
        return;

    parse(&postfix, &stack);

}

