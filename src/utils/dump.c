#include <fudge.h>
#include <abi.h>

static void dump(struct event_header *iheader, struct event_header *oheader, unsigned int count, void *buffer)
{

    char *data = buffer;
    unsigned int i;

    for (i = 0; i < count; i++)
    {

        unsigned char num[FUDGE_NSIZE];

        event_replydata(oheader, iheader);
        event_appenddata(oheader, ascii_wzerovalue(num, FUDGE_NSIZE, data[i], 16, 2, 0), num);
        event_appenddata(oheader, 2, "  ");
        event_send(oheader);

    }

}

static unsigned int ondata(struct event_header *iheader, struct event_header *oheader)
{

    struct event_data *data = event_getdata(iheader);

    dump(iheader, oheader, data->count, data + 1);

    return 0;

}

static unsigned int onfile(struct event_header *iheader, struct event_header *oheader)
{

    struct event_file *file = event_getdata(iheader);
    char buffer[FUDGE_BSIZE];
    unsigned int count;

    if (!file->descriptor)
        return 0;

    file_open(file->descriptor);

    while ((count = file_read(file->descriptor, buffer, FUDGE_BSIZE)))
        dump(iheader, oheader, count, buffer);

    file_close(file->descriptor);

    return 0;

}

static unsigned int onstop(struct event_header *iheader, struct event_header *oheader)
{

    return 1;

}

static unsigned int onkill(struct event_header *iheader, struct event_header *oheader)
{

    return 1;

}

void main(void)
{

    unsigned int status = 0;
    union event_message imessage;
    union event_message omessage;

    event_open();

    while (!status)
    {

        event_read(&imessage);

        switch (imessage.header.type)
        {

        case EVENT_DATA:
            status = ondata(&imessage.header, &omessage.header);

            break;

        case EVENT_FILE:
            status = onfile(&imessage.header, &omessage.header);

            break;

        case EVENT_STOP:
            status = onstop(&imessage.header, &omessage.header);

            break;

        case EVENT_KILL:
            status = onkill(&imessage.header, &omessage.header);

            break;

        }

    }

    event_close();

}

