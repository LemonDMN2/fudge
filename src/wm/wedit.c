#include <fudge.h>
#include <abi.h>
#include <widget.h>

static struct widget_textbox content;
static char outputdata[FUDGE_BSIZE];
static struct ring output;
static char inputdata1[FUDGE_BSIZE];
static struct ring input1;
static char inputdata2[FUDGE_BSIZE];
static struct ring input2;

static void updatecontent(void)
{

    struct message_header header;
    struct message_data data;
    unsigned int count;

    content.length = ring_count(&input1) + ring_count(&input2) + 1;
    content.cursor = ring_count(&input1);

    widget_update(&output, &content, WIDGET_Z_BOTTOM, WIDGET_TYPE_TEXTBOX, sizeof (struct widget_textbox) + content.length, &content.size);
    ring_write(&output, &content, sizeof (struct widget_textbox));
    ring_copy(&output, &input1);
    ring_copy(&output, &input2);
    ring_write(&output, "\n", 1);

    while ((count = ring_read(&output, data.buffer, FUDGE_MSIZE)))
    {

        message_initheader(&header, EVENT_DATA, count);
        file_notify(FILE_G0, &header, &data);

    }

    ring_reset(&output);

}

static void moveleft(unsigned int steps)
{

    char buffer[FUDGE_BSIZE];

    ring_writereverse(&input2, buffer, ring_readreverse(&input1, buffer, steps));

}

static void moveright(unsigned int steps)
{

    char buffer[FUDGE_BSIZE];

    ring_write(&input1, buffer, ring_read(&input2, buffer, steps));

}

static void moveup(void)
{

    unsigned int offset1;
    unsigned int offset2;

    offset1 = ring_findreverse(&input1, '\n');

    moveleft(ring_findreverse(&input1, '\n') + 1);

    offset2 = ring_findreverse(&input1, '\n');

    moveleft(offset2 - (offset1 < offset2 ? offset1 : offset2));

}

static void movedown(void)
{

    unsigned int offset1;
    unsigned int offset2;

    offset1 = ring_findreverse(&input1, '\n');

    moveright(ring_find(&input2, '\n') + 1);

    offset2 = ring_find(&input2, '\n');

    moveright(offset1 < offset2 ? offset1 : offset2);

}

static void onfile(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    char buffer[FUDGE_BSIZE];
    unsigned int count;

    if (!file_walk2(FILE_L0, mdata))
        return;

    file_open(FILE_L0);

    while ((count = file_read(FILE_L0, buffer, FUDGE_BSIZE)))
        ring_write(&input2, buffer, count);

    file_close(FILE_L0);

}

static void onwmkeypress(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    struct event_wmkeypress *wmkeypress = mdata;

    switch (wmkeypress->scancode)
    {

    case 0x0E:
        ring_skipreverse(&input1, 1);

        break;

    case 0x47:
        moveleft(ring_findreverse(&input1, '\n'));

        break;

    case 0x48:
        if (wmkeypress->keymod & KEYMOD_SHIFT)
            content.offset--;
        else
            moveup();

        break;

    case 0x4B:
        moveleft(1);

        break;

    case 0x4D:
        moveright(1);

        break;

    case 0x4F:
        moveright(ring_find(&input2, '\n'));

        break;

    case 0x50:
        if (wmkeypress->keymod & KEYMOD_SHIFT)
            content.offset++;
        else
            movedown();

        break;

    default:
        ring_write(&input1, &wmkeypress->unicode, wmkeypress->length);

        break;

    }

    updatecontent();

}

static void onwmshow(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    updatecontent();

}

static void onterm(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    struct message_header header;

    message_initheader(&header, EVENT_WMUNMAP, 0);
    file_notify(FILE_G0, &header, 0);
    channel_close(channel);

}

static void onmain(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    struct message_header header;
    struct message_data data;

    message_initheader(&header, EVENT_WMMAP, 0);
    file_notify(FILE_G0, &header, &data);

    while (channel_poll(channel, &header, &data))
        channel_dispatch(channel, &header, &data);

}

void init(struct channel *channel)
{

    ring_init(&output, FUDGE_BSIZE, outputdata);
    ring_init(&input1, FUDGE_BSIZE, inputdata1);
    ring_init(&input2, FUDGE_BSIZE, inputdata2);
    widget_inittextbox(&content);

    if (!file_walk2(FILE_G0, "/system/wserver"))
        return;

    channel_setcallback(channel, EVENT_MAIN, onmain);
    channel_setcallback(channel, EVENT_TERM, onterm);
    channel_setcallback(channel, EVENT_FILE, onfile);
    channel_setcallback(channel, EVENT_WMKEYPRESS, onwmkeypress);
    channel_setcallback(channel, EVENT_WMSHOW, onwmshow);

}

