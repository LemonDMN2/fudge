void render_scroll(unsigned int x, unsigned int y, int z);
void render_flush(unsigned char *canvasdata, unsigned int size, void (*draw)(void *data, unsigned int count, unsigned int offset));
void render_write(unsigned int source, void *buffer, unsigned int count);
void render_resize(unsigned int source, int x, int y, int w, int h, unsigned int padding, unsigned int lineheight, unsigned int steplength);
void render_clean(unsigned int source);
void render_complete(void);
void render_setdraw(unsigned int w, unsigned int h, unsigned int bpp);
void render_setfont(unsigned int index, void *data, unsigned int lineheight, unsigned int padding);
void render_setlayer(unsigned int index, unsigned char *data, unsigned int total);
void render_init();
