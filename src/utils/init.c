#include <fudge.h>
#include <abi.h>

static void ondata(unsigned int source, void *mdata, unsigned int msize)
{

    struct job jobs[32];
    unsigned int n = job_parse(jobs, 32, mdata, msize);
    unsigned int njobs = job_spawn(jobs, n);

    job_pipe(jobs, njobs);
    job_run(jobs, njobs);

}

void init(void)
{

    unsigned int id = file_spawn("/bin/slang");

    if (id)
    {

        channel_bind(EVENT_DATA, ondata);
        channel_redirectback(id, EVENT_DATA);
        channel_sendstringzto(id, EVENT_PATH, "/config/base.slang");
        channel_sendstringzto(id, EVENT_PATH, "/config/arch.slang");
        channel_sendstringzto(id, EVENT_PATH, "/config/init.slang");
        channel_sendto(id, EVENT_MAIN);

    }

}

