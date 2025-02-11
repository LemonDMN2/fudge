struct list_item *pool_next(struct list_item *current);
struct list_item *pool_nextin(struct list_item *current, char *in);
struct widget *pool_getwidgetbyid(char *id);
void pool_bump(struct widget *widget);
struct widget *pool_create(unsigned int type, char *id, char *in);
void pool_setup(void);
