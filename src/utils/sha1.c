#include <fudge.h>
#include <abi.h>

static struct sha1 sum;

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    sha1_read(&sum, mdata, msize);

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    unsigned char digest[20];
    struct message message;
    unsigned int i;

    sha1_write(&sum, digest);
    message_init(&message, EVENT_DATA);

    for (i = 0; i < 20; i++)
        message_putvalue2(&message, digest[i], 16, 2);

    message_putstring2(&message, "\n");
    channel_replymsg(&message);
    channel_close();

}

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk2(FILE_L0, mdata))
    {

        char buffer[BUFFER_SIZE];
        unsigned int count;

        file_seek(FILE_L0, 0);

        while ((count = file_read(FILE_L0, buffer, BUFFER_SIZE)))
            sha1_read(&sum, buffer, count);

    }

}

void init(void)
{

    sha1_init(&sum);
    channel_setcallback(EVENT_DATA, ondata);
    channel_setcallback(EVENT_MAIN, onmain);
    channel_setcallback(EVENT_PATH, onpath);

}

