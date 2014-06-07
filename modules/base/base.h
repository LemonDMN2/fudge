#define BASE_RESOURCE_BUS               8021
#define BASE_RESOURCE_DEVICE            8022
#define BASE_RESOURCE_DRIVER            8023

struct base_bus
{

    struct resource_item resource;
    const char *name;
    unsigned int type;
    void (*scan)(struct base_bus *self);

};

struct base_device
{

    struct resource_item resource;
    const char *name;
    unsigned int type;
    unsigned int irq;
    struct base_bus *bus;

};

struct base_driver
{

    struct resource_item resource;
    const char *name;
    unsigned int (*check)(struct base_device *device);
    void (*attach)(struct base_device *device);
    void (*detach)();

};

enum base_interface_type
{

    BASE_INTERFACE_TYPE_OTHER           = 0xFF

};

struct base_interface
{

    unsigned int type;

};

void base_register_bus(struct base_bus *bus);
void base_register_device(struct base_device *device);
void base_register_driver(struct base_driver *driver);
void base_unregister_bus(struct base_bus *bus);
void base_unregister_device(struct base_device *device);
void base_unregister_driver(struct base_driver *driver);
void base_init_bus(struct base_bus *bus, unsigned int type, const char *name, void (*scan)(struct base_bus *self));
void base_init_device(struct base_device *device, unsigned int type, unsigned int irq, const char *name, struct base_bus *bus);
void base_init_driver(struct base_driver *driver, const char *name, unsigned int (*check)(struct base_device *device), void (*attach)(struct base_device *device), void (*detach)(struct base_device *device));
void base_init_interface(struct base_interface *interface, unsigned int type);
