void channel_dispatch(struct message *message);
unsigned int channel_send(unsigned int target, unsigned int event);
unsigned int channel_sendbuffer(unsigned int target, unsigned int event, unsigned int count, void *data);
unsigned int channel_sendstring(unsigned int target, unsigned int event, char *string);
unsigned int channel_sendstringz(unsigned int target, unsigned int event, char *string);
unsigned int channel_sendredirectsame(unsigned int target, unsigned int event);
unsigned int channel_sendredirectto(unsigned int target, unsigned int event, unsigned int id);
unsigned int channel_sendredirectback(unsigned int target, unsigned int event);
unsigned int channel_reply(unsigned int event, unsigned int count, void *data);
unsigned int channel_replymsg(struct message *message);
unsigned int channel_replystring(unsigned int event, char *string);
unsigned int channel_replystringz(unsigned int event, char *string);
unsigned int channel_poll(struct message *message);
unsigned int channel_pollsource(unsigned int source, struct message *message);
unsigned int channel_pollevent(unsigned int event, struct message *message);
unsigned int channel_pollsourceevent(unsigned int source, unsigned int event, struct message *message);
unsigned int channel_polldescriptor(unsigned int descriptor, struct message *message);
unsigned int channel_polldescriptorevent(unsigned int descriptor, unsigned int event, struct message *message);
unsigned int channel_process(void);
unsigned int channel_readsource(unsigned int source, void *buffer, unsigned int count);
unsigned int channel_readdescriptor(unsigned int descriptor, void *buffer, unsigned int count);
unsigned int channel_wait(unsigned int source, unsigned int event);
void channel_close(void);
void channel_setcallback(unsigned int event, void (*callback)(unsigned int source, void *mdata, unsigned int msize));
void channel_redirect(unsigned int event, unsigned int mode, unsigned int id, unsigned int source);
void channel_init(void);
