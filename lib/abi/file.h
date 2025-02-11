unsigned int file_walk(unsigned int descriptor, unsigned int pdescriptor, char *path);
unsigned int file_walk2(unsigned int descriptor, char *path);
unsigned int file_duplicate(unsigned int descriptor, unsigned int pdescriptor);
unsigned int file_create(unsigned int descriptor, unsigned int pdescriptor, char *name);
unsigned int file_reset(unsigned int descriptor);
unsigned int file_seek(unsigned int descriptor, unsigned int offset);
unsigned int file_step(unsigned int descriptor);
unsigned int file_read(unsigned int descriptor, void *buffer, unsigned int count);
unsigned int file_readall(unsigned int descriptor, void *buffer, unsigned int count);
unsigned int file_write(unsigned int descriptor, void *buffer, unsigned int count);
unsigned int file_writeall(unsigned int descriptor, void *buffer, unsigned int count);
unsigned int file_seekread(unsigned int descriptor, void *buffer, unsigned int count, unsigned int offset);
unsigned int file_seekreadall(unsigned int descriptor, void *buffer, unsigned int count, unsigned int offset);
unsigned int file_seekwrite(unsigned int descriptor, void *buffer, unsigned int count, unsigned int offset);
unsigned int file_seekwriteall(unsigned int descriptor, void *buffer, unsigned int count, unsigned int offset);
unsigned int file_link(unsigned int descriptor);
unsigned int file_unlink(unsigned int descriptor);
unsigned int file_notify(unsigned int descriptor, unsigned int event, unsigned int count, void *data);
unsigned int file_spawn(char *path);
