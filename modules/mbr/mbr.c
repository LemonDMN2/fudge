#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/log.h>
#include <kernel/modules.h>
#include <kernel/vfs.h>
#include <modules/ata/ata.h>
#include <modules/mbr/mbr.h>

static struct mbr_driver driver;
static struct mbr_partition partitions[MBR_PARTITION_SLOTS];

static void read()
{

    struct ata_bus *bus = (struct ata_bus *)modules_get_bus(ATA_BUS_TYPE, 0);

    if (!bus)
        return;

    struct ata_device *device = bus->find_device(bus, ATA_DEVICE_TYPE_ATA, 0);

    if (!device)
        return;

    char buffer[512];

    device->read_lba28(device, 0, 1, buffer);

    unsigned int i;

    for (i = 0; i < MBR_PARTITION_SLOTS; i++)
        memory_copy(&partitions[i], buffer + MBR_PARTITION_OFFSET + i * MBR_PARTITION_SIZE, sizeof (struct mbr_partition));

}

static struct mbr_partition *mbr_driver_get_partition(unsigned int index)
{

    read();

    struct mbr_partition *partition = &partitions[index];

    if (!partitions->systemId)
        return 0;

    return partition;

}

void mbr_driver_init(struct mbr_driver *driver)
{

    modules_driver_init(&driver->base, MBR_DRIVER_TYPE);
    driver->get_partition = mbr_driver_get_partition;

}

void init()
{

    mbr_driver_init(&driver);
    modules_register_driver(&driver.base);

}

void destroy()
{

    modules_unregister_driver(&driver.base);

}

