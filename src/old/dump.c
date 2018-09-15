#include <abi.h>
#include <fudge.h>
#include <event/base.h>

static unsigned int quit;

static void reply(struct event_header *header, unsigned int count, char *buffer)
{

    unsigned int i;

    for (i = 0; i < count; i++)
    {

        unsigned char num[FUDGE_NSIZE];

        event_senddata(FILE_L0, header->destination, header->source, ascii_wzerovalue(num, FUDGE_NSIZE, buffer[i], 16, 2, 0), num);
        event_senddata(FILE_L0, header->destination, header->source, 2, "  ");

    }

}

static void ondata(struct event_header *header, void *data)
{

    reply(header, header->length - sizeof (struct event_header), data);

}

static void onfile(struct event_header *header, void *data)
{

    struct event_file *file = data;
    unsigned char buffer[FUDGE_BSIZE];
    unsigned int count;

    file_open(file->num);

    while ((count = file_read(file->num, buffer, FUDGE_BSIZE)))
        reply(header, count, data);

    file_close(file->num);

}

static void onkill(struct event_header *header, void *data)
{

    quit = 1;

}

void main(void)
{

    if (!file_walk(FILE_L0, "/system/event"))
        return;

    file_open(FILE_L0);

    while (!quit)
    {

        struct event event;

        event_read(FILE_L0, &event);

        switch (event.header.type)
        {

        case EVENT_EXIT:
        case EVENT_KILL:
            onkill(&event.header, event.data);

            break;

        case EVENT_DATA:
            ondata(&event.header, event.data);

            break;

        case EVENT_FILE:
            onfile(&event.header, event.data);

            break;

        }

    }

    file_close(FILE_L0);

}
