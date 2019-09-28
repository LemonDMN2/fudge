#include <fudge.h>
#include "call.h"
#include "file.h"
#include "channel.h"

static void ignore(struct channel *channel, void *mdata, unsigned int msize)
{

}

static void abort(struct channel *channel, void *mdata, unsigned int msize)
{

    channel_exit(channel);

}

unsigned int channel_pick(struct channel *channel, void *data)
{

    return call_pick(&channel->i, data);

}

unsigned int channel_place(struct channel *channel, unsigned int id)
{

    return call_place(id, &channel->o, channel->data);

}

unsigned int channel_write(struct channel *channel, unsigned int descriptor)
{

    return file_writeall(descriptor, &channel->o, channel->o.length);

}

void channel_listen(struct channel *channel)
{

    while (channel->state)
    {

        char data[FUDGE_BSIZE];

        if (channel_pick(channel, data))
        {

            channel->signals[channel->i.type](channel, data, channel->i.length - sizeof (struct ipc_header));
            channel->signals[EVENT_ANY](channel, data, channel->i.length - sizeof (struct ipc_header));

        }

    }

}

void channel_nosignal(struct channel *channel, unsigned int type)
{

    channel->signals[type] = ignore;

}

void channel_setsignal(struct channel *channel, unsigned int type, void (*callback)(struct channel *channel, void *mdata, unsigned int msize))
{

    channel->signals[type] = callback;

}

void channel_forward(struct channel *channel, unsigned int type)
{

    unsigned int i;

    ipc_create(&channel->o, type, 0);

    channel->o.session = channel->i.session;

    for (i = 0; i < channel->i.nroutes; i++)
        ipc_addroute(&channel->o, channel->i.routes[i].target, channel->i.routes[i].session);

}

void channel_request(struct channel *channel, unsigned int type)
{

    channel_forward(channel, type);
    ipc_addroute(&channel->o, channel->i.target, 0);

}

void channel_request2(struct channel *channel, unsigned int type, unsigned int session)
{

    channel_forward(channel, type);

    channel->o.session = session;

    ipc_addroute(&channel->o, channel->i.target, session);

}

unsigned int channel_reply(struct channel *channel, unsigned int type)
{

    channel_forward(channel, type);

    if (channel->o.nroutes)
    {

        channel->o.nroutes--;
        channel->o.session = channel->o.routes[channel->o.nroutes].session;

        return channel->o.routes[channel->o.nroutes].target;

    }

    return channel->i.source;

}

void channel_append(struct channel *channel, unsigned int count, void *buffer)
{

    channel->o.length += memory_write(&channel->data, FUDGE_BSIZE, buffer, count, channel->o.length - sizeof (struct ipc_header));

}

void channel_appendstring(struct channel *channel, char *string)
{

    channel->o.length += memory_write(&channel->data, FUDGE_BSIZE, string, ascii_length(string), channel->o.length - sizeof (struct ipc_header));

}

void channel_appendvalue(struct channel *channel, int value, unsigned int base)
{

    char num[FUDGE_NSIZE];

    channel->o.length += memory_write(&channel->data, FUDGE_BSIZE, num, ascii_wvalue(num, FUDGE_NSIZE, value, base, 0), channel->o.length - sizeof (struct ipc_header));

}

void channel_appendvaluepadded(struct channel *channel, int value, unsigned int base, unsigned int padding)
{

    char num[FUDGE_NSIZE];

    channel->o.length += memory_write(&channel->data, FUDGE_BSIZE, num, ascii_wvalue(num, FUDGE_NSIZE, value, base, padding), channel->o.length - sizeof (struct ipc_header));

}

void channel_init(struct channel *channel)
{

    unsigned int i;

    channel->state = 1;

    for (i = 0; i < EVENTS; i++)
        channel_nosignal(channel, i);

    channel_setsignal(channel, EVENT_KILL, abort);
    channel_setsignal(channel, EVENT_CLOSE, abort);
    ipc_create(&channel->i, 0, 0);
    ipc_create(&channel->o, 0, 0);

}

void channel_exit(struct channel *channel)
{

    channel->state = 0;

}

