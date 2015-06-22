#include <fudge.h>
#include <kernel.h>
#include <modules/system/system.h>
#include "video.h"

static unsigned int interfacectrl_read(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;
    struct ctrl_videosettings *settings = buffer;

    if (size == sizeof (struct ctrl_header) || size == sizeof (struct ctrl_videosettings))
        return memory_read(settings, 1, &interface->settings, 1, size, offset);

    return 0;

}

static unsigned int interfacectrl_write(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;
    struct ctrl_videosettings *settings = buffer;

    if (size == sizeof (struct ctrl_videosettings))
    {

        interface->setmode(settings);

        return 1;

    }

    return 0;

}

static unsigned int interfacedata_read(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;

    return interface->rdata(offset, size, count, buffer);

}

static unsigned int interfacedata_write(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;

    return interface->wdata(offset, size, count, buffer);

}

static unsigned int interfacecolormap_read(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;

    return interface->rcolormap(offset, size, count, buffer);

}

static unsigned int interfacecolormap_write(struct system_node *self, unsigned int offset, unsigned int size, unsigned int count, void *buffer)
{

    struct video_interface *interface = self->resource->data;

    return interface->wcolormap(offset, size, count, buffer);

}

void video_registerinterface(struct video_interface *interface, unsigned int id)
{

    resource_register(&interface->resource);
    system_addchild(&interface->root, &interface->ctrl);
    system_addchild(&interface->root, &interface->data);
    system_addchild(&interface->root, &interface->colormap);
    system_registernode(&interface->root);

    interface->id = id;

}

void video_unregisterinterface(struct video_interface *interface)
{

    resource_unregister(&interface->resource);
    system_removechild(&interface->root, &interface->ctrl);
    system_removechild(&interface->root, &interface->data);
    system_removechild(&interface->root, &interface->colormap);
    system_unregisternode(&interface->root);

}

void video_initinterface(struct video_interface *interface, void (*setmode)(struct ctrl_videosettings *settings), unsigned int (*rdata)(unsigned int offset, unsigned int size, unsigned int count, void *buffer), unsigned int (*wdata)(unsigned int offset, unsigned int size, unsigned int count, void *buffer), unsigned int (*rcolormap)(unsigned int offset, unsigned int size, unsigned int count, void *buffer), unsigned int (*wcolormap)(unsigned int offset, unsigned int size, unsigned int count, void *buffer))
{

    resource_init(&interface->resource, RESOURCE_VIDEOINTERFACE, interface);
    system_initnode(&interface->root, SYSTEM_NODETYPE_GROUP | SYSTEM_NODETYPE_MULTI, "video");
    system_initnode(&interface->ctrl, SYSTEM_NODETYPE_NORMAL, "ctrl");
    system_initnode(&interface->data, SYSTEM_NODETYPE_NORMAL, "data");
    system_initnode(&interface->colormap, SYSTEM_NODETYPE_NORMAL, "colormap");

    interface->setmode = setmode;
    interface->rdata = rdata;
    interface->wdata = wdata;
    interface->rcolormap = rcolormap;
    interface->wcolormap = wcolormap;
    interface->ctrl.resource = &interface->resource;
    interface->ctrl.read = interfacectrl_read;
    interface->ctrl.write = interfacectrl_write;
    interface->data.resource = &interface->resource;
    interface->data.read = interfacedata_read;
    interface->data.write = interfacedata_write;
    interface->colormap.resource = &interface->resource;
    interface->colormap.read = interfacecolormap_read;
    interface->colormap.write = interfacecolormap_write;

}
