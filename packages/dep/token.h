enum token_type
{

    TOKEN_TYPE_NULL                     = 0,
    TOKEN_TYPE_PATH                     = 1,
    TOKEN_TYPE_SPACE                    = 4,
    TOKEN_TYPE_COLON                    = 8,
    TOKEN_TYPE_NEWLINE                  = 16

};

struct token_state
{

    char *buffer;
    unsigned int count;
    unsigned int current;

};

unsigned int token_next(struct token_state *state);
unsigned int token_accept(struct token_state *state, enum token_type type);
void token_skip(struct token_state *state, enum token_type type);
void token_init_state(struct token_state *state, unsigned int count, char *buffer);
