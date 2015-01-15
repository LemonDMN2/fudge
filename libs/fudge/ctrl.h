enum
{

    CTRL_TYPE_NULL,
    CTRL_TYPE_CONSOLE,
    CTRL_TYPE_NETWORK,
    CTRL_TYPE_VIDEO

};

struct ctrl_header
{

    unsigned int type;

};

struct ctrl_consolesettings
{

    struct ctrl_header header;
    unsigned char scroll;

};

struct ctrl_networksettings
{

    struct ctrl_header header;
    unsigned char mac[6];

};

struct ctrl_videosettings
{

    struct ctrl_header header;
    unsigned int w;
    unsigned int h;
    unsigned int bpp;

};

void ctrl_init_header(struct ctrl_header *header, unsigned int type);
void ctrl_init_consolesettings(struct ctrl_consolesettings *settings);
void ctrl_init_networksettings(struct ctrl_networksettings *settings);
void ctrl_init_videosettings(struct ctrl_videosettings *settings);