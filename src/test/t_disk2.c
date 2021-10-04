#include <fudge.h>
#include <abi.h>

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    struct message message;

    p9p_mktversion(&message, 1200, "9P2000");
    file_notify(FILE_G0, EVENT_P9P, message_datasize(&message.header), message.data.buffer);
    channel_pollevent(EVENT_P9P, &message);

    /* check that version is ok */

    p9p_mktwalk(&message, 22445566, 0, "build/data/help.txt");
    file_notify(FILE_G0, EVENT_P9P, message_datasize(&message.header), message.data.buffer);
    channel_pollevent(EVENT_P9P, &message);
    
    /* check that walk is ok */

    p9p_mktread(&message, 22445566, 0, 0, 512);
    file_notify(FILE_G0, EVENT_P9P, message_datasize(&message.header), message.data.buffer);
    channel_pollevent(EVENT_P9P, &message);

    /* check that read is ok */

    channel_sendbuffer(EVENT_DATA, message_datasize(&message.header) - sizeof (struct event_p9p) - sizeof (struct p9p_rread), message.data.buffer + sizeof (struct event_p9p) + sizeof (struct p9p_rread));
    channel_close();

}

void init(void)
{

    if (!file_walk2(FILE_G0, "system:service/fd0"))
        return;

    channel_bind(EVENT_MAIN, onmain);

}

