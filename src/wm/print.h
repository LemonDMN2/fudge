void print_insertfill(struct buffer *buffer, unsigned int source, struct element_fill *fill, unsigned int z);
void print_insertmouse(struct buffer *buffer, unsigned int source, struct element_mouse *mouse, unsigned int z);
void print_insertpanel(struct buffer *buffer, unsigned int source, struct element_panel *panel, unsigned int z);
void print_inserttext(struct buffer *buffer, unsigned int source, struct element_text *text, unsigned int z, void *textbuffer, unsigned int count);
void print_insertwindow(struct buffer *buffer, unsigned int source, struct element_window *window, unsigned int z);
void print_removefill(struct buffer *buffer, unsigned int source, struct element_fill *fill);
void print_removemouse(struct buffer *buffer, unsigned int source, struct element_mouse *mouse);
void print_removepanel(struct buffer *buffer, unsigned int source, struct element_panel *panel);
void print_removetext(struct buffer *buffer, unsigned int source, struct element_text *text);
void print_removewindow(struct buffer *buffer, unsigned int source, struct element_window *window);
