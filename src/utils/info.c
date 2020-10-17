#include <fudge.h>
#include <abi.h>

static unsigned int idecho;

static void onmain(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    channel_sendfile(channel, idecho, "/system/info/cores");
    channel_sendfile(channel, idecho, "/system/info/tasks");
    channel_sendfile(channel, idecho, "/system/info/mailboxes");
    channel_sendmain(channel, idecho);
    channel_close(channel);

}

static void onredirect(struct channel *channel, unsigned int source, void *mdata, unsigned int msize)
{

    struct event_redirect *redirect = mdata;

    if (redirect->mode == 2)
        channel_sendredirect(channel, idecho, redirect->type, 1, source);
    else
        channel_sendredirect(channel, idecho, redirect->type, redirect->mode, redirect->id);

}

static void oninit(struct channel *channel)
{

    if (!file_walk2(FILE_CP, "/bin/echo"))
        return;

    idecho = call_spawn(FILE_CP);

}

void main(void)
{

    struct channel channel;

    channel_init(&channel);
    channel_setsignal(&channel, EVENT_MAIN, onmain);
    channel_setsignal(&channel, EVENT_REDIRECT, onredirect);
    channel_listen2(&channel, oninit, 0);

}

