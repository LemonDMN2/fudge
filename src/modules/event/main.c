#include <fudge.h>
#include <kernel.h>
#include <modules/system/system.h>

static struct system_node wserver;
static struct system_node wclient;

static unsigned int multicast(struct service_link *link, struct list *targets, struct message_header *header, void *data)
{

    struct list_item *current;

    spinlock_acquire(&targets->spinlock);

    for (current = targets->head; current; current = current->next)
    {

        struct service_link *target = current->data;

        header->source = link->id;

        kernel_place(target->id, header, data);

    }

    spinlock_release(&targets->spinlock);

    return header->length;

}

static unsigned int wserver_seek(unsigned int offset)
{

    return 0;

}

static unsigned int wclient_write(struct service_link *link, void *buffer, unsigned int count, unsigned int offset)
{

    struct message_header *header = buffer;

    return multicast(link, &wserver.links, header, header + 1);

}

static unsigned int wclient_seek(unsigned int offset)
{

    return 0;

}

void module_init(void)
{

    system_initnode(&wserver, SYSTEM_NODETYPE_NORMAL, "wserver");

    wserver.operations.seek = wserver_seek;

    system_initnode(&wclient, SYSTEM_NODETYPE_NORMAL, "wclient");

    wclient.operations.write = wclient_write;
    wclient.operations.seek = wclient_seek;

}

void module_register(void)
{

    system_registernode(&wserver);
    system_registernode(&wclient);

}

void module_unregister(void)
{

    system_unregisternode(&wserver);
    system_unregisternode(&wclient);

}

