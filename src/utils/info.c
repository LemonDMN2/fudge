#include <fudge.h>
#include <abi.h>

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    unsigned int id = file_spawn("/bin/echo");

    if (id)
    {

        channel_redirectsame(id, EVENT_DATA);
        channel_redirectback(id, EVENT_CLOSE);
        channel_sendstringzto(id, EVENT_PATH, "system:info/cores");
        channel_sendstringzto(id, EVENT_PATH, "system:info/tasks");
        channel_sendstringzto(id, EVENT_PATH, "system:info/mailboxes");
        channel_sendto(id, EVENT_MAIN);
        channel_wait(id, EVENT_CLOSE);

    }

    channel_close();

}

void init(void)
{

    channel_bind(EVENT_MAIN, onmain);

}

