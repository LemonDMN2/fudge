struct kernel_module
{

    struct vfs_backend base;
    void *address;
    unsigned int limit;

};

struct kernel_rendezvous
{

    unsigned int id;
    struct task *task;

};

void kernel_rendezvous_sleep(struct kernel_rendezvous *rendezvous, unsigned int condition);
void kernel_rendezvous_unsleep(struct kernel_rendezvous *rendezvous, unsigned int condition);
void kernel_setup_modules(unsigned int count, struct kernel_module *modules);
struct container *kernel_setup();
