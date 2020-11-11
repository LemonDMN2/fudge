#include <fudge.h>
#include <kernel.h>
#include "system.h"

static struct system_node root;
static struct service_protocol protocol;

static struct system_node *getnode(unsigned int id)
{

    return (struct system_node *)id;

}

static unsigned int protocol_root(void)
{

    return (unsigned int)&root;

}

static unsigned int protocol_parent(unsigned int id)
{

    struct system_node *node = getnode(id);

    return (unsigned int)node->parent;

}

static unsigned int protocol_child(unsigned int id, char *path, unsigned int length)
{

    struct system_node *node = getnode(id);
    struct list_item *current;
    struct system_node *n = node;

    spinlock_acquire(&node->children.spinlock);

    for (current = node->children.head; current; current = current->next)
    {

        struct system_node *n2 = current->data;
        unsigned int length0 = ascii_length(n2->name);

        if (n2->type == SYSTEM_NODETYPE_MULTIGROUP)
        {

            unsigned int colon = memory_findbyte(path, length, ':');
            unsigned int val;

            if (length0 != colon)
                continue;

            if (!memory_match(n2->name, path, colon))
                continue;

            val = ascii_rvalue(path + colon + 1, length - colon - 1, 10);

            if (val != n2->index)
                continue;

        }

        else
        {

            if (length0 != length)
                continue;

            if (!memory_match(n2->name, path, length))
                continue;

        }

        n = n2;

        break;

    }

    spinlock_release(&node->children.spinlock);

    return (unsigned int)n;

}

static unsigned int protocol_create(unsigned int id, char *name, unsigned int length)
{

    return 0;

}

static unsigned int protocol_destroy(unsigned int id, char *name, unsigned int length)
{

    return 0;

}

static unsigned int protocol_step(unsigned int id, unsigned int current)
{

    struct system_node *node = getnode(current);

    if (id == current)
    {

        spinlock_acquire(&node->children.spinlock);

        id = (node->children.head) ? (unsigned int)node->children.head->data : 0;

        spinlock_release(&node->children.spinlock);

        return id;

    }

    else
    {

        return (node->item.next) ? (unsigned int)node->item.next->data : 0;

    }

}

static unsigned int protocol_open(struct service_state *state, unsigned int id)
{

    struct system_node *node = getnode(id);

    list_add(&node->states, &state->item);

    return id;

}

static unsigned int protocol_close(struct service_state *state, unsigned int id)
{

    struct system_node *node = getnode(id);

    list_remove(&node->states, &state->item);

    return id;

}

static unsigned int protocol_read(struct service_state *state, unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset)
{

    struct system_node *node = getnode(id);
    struct system_node *currentnode = getnode(current);

    return node->operations.read(node, currentnode, state, buffer, count, offset);

}

static unsigned int protocol_write(struct service_state *state, unsigned int id, unsigned int current, void *buffer, unsigned int count, unsigned int offset)
{

    struct system_node *node = getnode(id);
    struct system_node *currentnode = getnode(current);

    return node->operations.write(node, currentnode, state, buffer, count, offset);

}

static unsigned int protocol_seek(unsigned int id, unsigned int offset)
{

    struct system_node *node = getnode(id);

    return node->operations.seek(node, offset);

}

static unsigned int protocol_map(unsigned int id)
{

    return id;

}

void system_registernode(struct system_node *node)
{

    system_addchild(&root, node);

}

void system_unregisternode(struct system_node *node)
{

    system_removechild(&root, node);

}

void system_initprotocol(void)
{

    system_initnode(&root, SYSTEM_NODETYPE_GROUP, "FUDGE_ROOT");

    root.parent = &root;

    service_initprotocol(&protocol, 2000, protocol_root, protocol_parent, protocol_child, protocol_create, protocol_destroy, protocol_step, protocol_open, protocol_close, protocol_read, protocol_write, protocol_seek, protocol_map);
    resource_register(&protocol.resource);

}

