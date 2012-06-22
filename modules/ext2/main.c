#include <kernel/modules.h>
#include <modules/ext2/ext2.h>

static struct ext2_driver driver;

void init()
{

    ext2_driver_init(&driver);
    modules_register_driver(&driver.base);

}

void destroy()
{

    modules_unregister_driver(&driver.base);

}

