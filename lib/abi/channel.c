#include <fudge.h>
#include "call.h"
#include "file.h"
#include "channel.h"

#define CHANNEL_CALLBACKS               256

static struct channel_callback
{

    void (*callback)(unsigned int source, void *data, unsigned int size);
    unsigned int target;

} callbacks[CHANNEL_CALLBACKS];

static unsigned int poll;

static unsigned int send(unsigned int target, unsigned int event, unsigned int count, void *data)
{

    struct message_header header;
    unsigned int offset = 0;
    char *buffer = data;

    if (!target)
    {

        if (callbacks[event].target)
            target = callbacks[event].target;

    }

    if (!target)
        return 0;

    while (count > MESSAGE_SIZE)
    {

        message_initheader(&header, event, MESSAGE_SIZE);

        while (!call_place(target, &header, buffer + offset));

        count -= MESSAGE_SIZE;
        offset += MESSAGE_SIZE;

    }

    message_initheader(&header, event, count);

    while (!call_place(target, &header, buffer + offset));

    return count + offset;

}

static unsigned int read(unsigned int source, void *buffer, unsigned int count)
{

    struct message message;

    while (channel_pick(&message))
    {

        if (message.header.source == source && message.header.event == EVENT_CLOSE)
            return 0;
        else if (message.header.source == source && message.header.event == EVENT_DATA)
            return buffer_write(buffer, count, message.data.buffer, message_datasize(&message.header), 0);
        else
            channel_dispatch(&message);

    }

    return 0;

}

static unsigned int redirect(unsigned int target, unsigned int event, unsigned int mode, unsigned int id)
{

    struct event_redirect redirect;

    redirect.event = event;
    redirect.mode = mode;
    redirect.id = id;

    return send(target, EVENT_REDIRECT, sizeof (struct event_redirect), &redirect);

}

void channel_dispatch(struct message *message)
{

    if (message->header.event < CHANNEL_CALLBACKS)
    {

        if (callbacks[message->header.event].callback)
            callbacks[message->header.event].callback(message->header.source, message->data.buffer, message_datasize(&message->header));

    }

}

unsigned int channel_send(unsigned int event)
{

    return send(0, event, 0, 0);

}

unsigned int channel_sendto(unsigned int target, unsigned int event)
{

    return send(target, event, 0, 0);

}

unsigned int channel_sendbuffer(unsigned int event, unsigned int count, void *data)
{

    return send(0, event, count, data);

}

unsigned int channel_sendbufferto(unsigned int target, unsigned int event, unsigned int count, void *data)
{

    return send(target, event, count, data);

}

unsigned int channel_sendstring(unsigned int event, char *string)
{

    return send(0, event, cstring_length(string), string);

}

unsigned int channel_sendstringto(unsigned int target, unsigned int event, char *string)
{

    return send(target, event, cstring_length(string), string);

}

unsigned int channel_sendstringz(unsigned int event, char *string)
{

    return send(0, event, cstring_lengthz(string), string);

}

unsigned int channel_sendstringzto(unsigned int target, unsigned int event, char *string)
{

    return send(target, event, cstring_lengthz(string), string);

}

unsigned int channel_sendvalue(unsigned int event, int value, unsigned int base, unsigned int padding)
{

    char num[CSTRING_NUMSIZE];

    return send(0, event, cstring_wvalue(num, CSTRING_NUMSIZE, value, base, padding), num);

}

unsigned int channel_sendvalueto(unsigned int target, unsigned int event, int value, unsigned int base, unsigned int padding)
{

    char num[CSTRING_NUMSIZE];

    return send(target, event, cstring_wvalue(num, CSTRING_NUMSIZE, value, base, padding), num);

}

unsigned int channel_sendmessage(struct message *message)
{

    return send(0, message->header.event, message_datasize(&message->header), message->data.buffer);

}

unsigned int channel_sendmessageto(unsigned int target, struct message *message)
{

    return send(target, message->header.event, message_datasize(&message->header), message->data.buffer);

}

unsigned int channel_redirectsame(unsigned int target, unsigned int event)
{

    return redirect(target, event, EVENT_REDIRECT_TARGET, callbacks[event].target);

}

unsigned int channel_redirecttarget(unsigned int target, unsigned int event, unsigned int id)
{

    return redirect(target, event, EVENT_REDIRECT_TARGET, id);

}

unsigned int channel_redirectback(unsigned int target, unsigned int event)
{

    return redirect(target, event, EVENT_REDIRECT_SOURCE, 0);

}

unsigned int channel_pick(struct message *message)
{

    while (poll)
    {

        if (call_pick(&message->header, &message->data))
            return message->header.event;

    }

    return 0;

}

unsigned int channel_process(void)
{

    struct message message;

    if (channel_pick(&message))
    {

        channel_dispatch(&message);

        return message.header.event;

    }

    return 0;

}

unsigned int channel_pollfrom(unsigned int source, struct message *message)
{

    while (channel_pick(message))
    {

        if (message->header.source == source)
            return message->header.event;

        channel_dispatch(message);

    }

    return 0;

}

unsigned int channel_pollevent(unsigned int event, struct message *message)
{

    while (channel_pick(message))
    {

        if (message->header.event == event)
            return message->header.event;

        channel_dispatch(message);

    }

    return 0;

}

unsigned int channel_polleventfrom(unsigned int source, unsigned int event, struct message *message)
{

    while (channel_pick(message))
    {

        if (message->header.source == source && message->header.event == event)
            return message->header.event;

        channel_dispatch(message);

    }

    return 0;

}

unsigned int channel_pollsystem(struct message *message)
{

    return channel_pollfrom(0, message);

}

unsigned int channel_polleventsystem(unsigned int event, struct message *message)
{

    return channel_polleventfrom(0, event, message);

}

unsigned int channel_read(void *buffer, unsigned int count)
{

    return read(0, buffer, count);

}

unsigned int channel_readfrom(unsigned int source, void *buffer, unsigned int count)
{

    return read(source, buffer, count);

}

unsigned int channel_wait(unsigned int source, unsigned int event)
{

    struct message message;

    while (channel_pick(&message))
    {

        if (message.header.source == source && message.header.event == event)
            return message.header.event;

        channel_dispatch(&message);

    }

    return 0;

}

void channel_bind(unsigned int event, void (*callback)(unsigned int source, void *mdata, unsigned int msize))
{

    callbacks[event].callback = callback;

}

void channel_route(unsigned int event, unsigned int mode, unsigned int id, unsigned int source)
{

    switch (mode)
    {

    case EVENT_REDIRECT_TARGET:
        callbacks[event].target = id;

        break;

    case EVENT_REDIRECT_SOURCE:
        callbacks[event].target = source;

        break;

    default:
        callbacks[event].target = 0;

        break;

    }

}

void channel_open(void)
{

    poll = 1;

}

void channel_close(void)
{

    channel_send(EVENT_CLOSE);

    poll = 0;

}

