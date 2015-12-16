void kernel_copydescriptors(struct container *container, struct task *source, struct task *target);
unsigned int kernel_setupbinary(struct container *container, struct task *task, unsigned int sp);
unsigned int kernel_setupramdisk(struct container *container, struct task *task, struct vfs_backend *backend);
void kernel_setup(void);
