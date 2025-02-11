struct service
{

    char *name;
    struct resource resource;
    unsigned int (*root)(void);
    unsigned int (*parent)(unsigned int id);
    unsigned int (*child)(unsigned int id, char *path, unsigned int length);
    unsigned int (*create)(unsigned int id, char *name, unsigned int length);
    unsigned int (*destroy)(unsigned int id);
    unsigned int (*step)(unsigned int id, unsigned int current);
    unsigned int (*read)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset);
    unsigned int (*write)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset);
    unsigned int (*seek)(unsigned int id, unsigned int offset);
    unsigned int (*map)(unsigned int id);
    unsigned int (*link)(unsigned int id, unsigned int source);
    unsigned int (*unlink)(unsigned int id, unsigned int source);
    unsigned int (*notify)(unsigned int id, unsigned int source, unsigned int event, unsigned int count, void *data);

};

struct service *service_find(unsigned int count, char *name);
void service_init(struct service *service, char *name, unsigned int (*root)(void), unsigned int (*parent)(unsigned int id), unsigned int (*child)(unsigned int id, char *path, unsigned int length), unsigned int (*create)(unsigned int id, char *name, unsigned int length), unsigned int (*destroy)(unsigned int id), unsigned int (*step)(unsigned int id, unsigned int current), unsigned int (*read)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset), unsigned int (*write)(unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset), unsigned int (*seek)(unsigned int id, unsigned int offset), unsigned int (*map)(unsigned int id), unsigned int (*link)(unsigned int id, unsigned int source), unsigned int (*unlink)(unsigned int id, unsigned int source), unsigned int (*notify)(unsigned int id, unsigned int source, unsigned int event, unsigned int count, void *data));
