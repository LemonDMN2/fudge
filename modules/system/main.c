#include <fudge/module.h>
#include <kernel/vfs.h>
#include "system.h"
#include "filesystem.h"

static struct system_backend backend;
static struct vfs_protocol protocol;

void system_group_add(struct system_group *group, struct system_node *node)
{

    struct system_node *current;

    node->parent = &group->node;

    if (!group->children)
    {

        group->children = node;

        return;

    }

    for (current = group->children; current; current = current->sibling)
    {

        if (current->sibling)
            continue;

        current->sibling = node;

        return;

    }

}

void system_group_remove(struct system_group *group, struct system_node *node)
{

    struct system_node *current;

    node->parent = 0;

    if (group->children == node)
    {

        group->children = group->children->sibling;

        return;

    }

    for (current = group->children; current; current = current->sibling)
    {

        if (current->sibling != node)
            continue;

        current->sibling = current->sibling->sibling;

        return;

    }

}

void system_register_node(struct system_node *node)
{

    system_group_add(&backend.root, node);

}

void system_unregister_node(struct system_node *node)
{

    system_group_remove(&backend.root, node);

}

static void system_init_node(struct system_node *node, unsigned int type, char *name)
{

    memory_clear(node, sizeof (struct system_node));

    node->type = type;
    node->name = name;

}

void system_init_group(struct system_group *group, char *name)
{

    memory_clear(group, sizeof (struct system_group));
    system_init_node(&group->node, SYSTEM_NODETYPE_GROUP, name);

}

void system_init_stream(struct system_stream *stream, char *name, unsigned int (*read)(struct system_stream *self, unsigned int offset, unsigned int count, void *buffer), unsigned int (*write)(struct system_stream *self, unsigned int offset, unsigned int count, void *buffer))
{

    memory_clear(stream, sizeof (struct system_stream));
    system_init_node(&stream->node, SYSTEM_NODETYPE_STREAM, name);

    stream->read = read;
    stream->write = write;

}

struct vfs_backend *get_backend()
{

    return &backend.base;

}

void init()
{

    system_init_backend(&backend);
    system_init_protocol(&protocol, &backend);
    vfs_register_protocol(&protocol);

}

void destroy()
{

}

