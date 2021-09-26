#include <fudge.h>
#include <net.h>
#include <abi.h>
#include <socket.h>

static struct socket local;
static struct socket remote;
static struct socket router;
static char domain[512];

static unsigned int buildrequest(unsigned int count, void *buffer)
{

    struct dns_header header;
    struct dns_question question;
    unsigned int offset = 0;

    buffer_clear(&header, sizeof (struct dns_header));
    buffer_clear(&question, sizeof (struct dns_question));
    net_save16(header.id, 0x0001);
    net_save16(header.flags, 0x0100);
    net_save16(header.questions, 0x0001);
    net_save16(question.type, 0x0001);
    net_save16(question.class, 0x0001);

    offset += buffer_write(buffer, count, &header, sizeof (struct dns_header), offset);
    offset += dns_copyname((char *)buffer + offset, count - offset, domain);
    offset += buffer_write(buffer, count, &question, sizeof (struct dns_question), offset);

    return offset;

}

static void printmain(unsigned short type, char *name, void *rddata, void *buffer)
{

    unsigned char *addr = rddata;
    struct message message;
    char fullname[256];

    message_init(&message, EVENT_DATA);

    switch (type)
    {

    case 1:
        message_putbuffer2(&message, dns_writename(fullname, 256, name, buffer), fullname);
        message_putstring2(&message, " has address ");
        message_putvalue2(&message, addr[0], 10, 0);
        message_putstring2(&message, ".");
        message_putvalue2(&message, addr[1], 10, 0);
        message_putstring2(&message, ".");
        message_putvalue2(&message, addr[2], 10, 0);
        message_putstring2(&message, ".");
        message_putvalue2(&message, addr[3], 10, 0);
        message_putstring2(&message, "\n");

        break;

    case 5:
        message_putbuffer2(&message, dns_writename(fullname, 256, name, buffer), fullname);
        message_putstring2(&message, " is an alias for ");
        message_putbuffer2(&message, dns_writename(fullname, 256, rddata, buffer), fullname);
        message_putstring2(&message, "\n");

        break;

    }

    channel_replymsg(&message);

}

static void printquery(char *query, unsigned int qsize, unsigned short type, char *name, void *rddata, void *buffer)
{

    unsigned int qtype = ascii_match(query, "type");
    unsigned int qname = ascii_match(query, "name");
    unsigned int qdata = ascii_match(query, "data");
    unsigned char *addr = rddata;
    struct message message;
    char fullname[256];

    message_init(&message, EVENT_DATA);

    if (qtype)
    {

        message_putvalue2(&message, type, 10, 0);
        message_putstringz2(&message, "");

    }

    if (qname)
    {

        message_putbuffer2(&message, dns_writename(fullname, 256, name, buffer), fullname);
        message_putstringz2(&message, "");

    }

    if (qdata)
    {

        switch (type)
        {

        case 1:
            message_putvalue2(&message, addr[0], 10, 0);
            message_putstring2(&message, ".");
            message_putvalue2(&message, addr[1], 10, 0);
            message_putstring2(&message, ".");
            message_putvalue2(&message, addr[2], 10, 0);
            message_putstring2(&message, ".");
            message_putvalue2(&message, addr[3], 10, 0);
            message_putstringz2(&message, "");

            break;

        case 5:
            message_putbuffer2(&message, dns_writename(fullname, 256, rddata, buffer), fullname);
            message_putstringz2(&message, "");

            break;

        default:
            message_putstringz2(&message, "<null>");

            break;

        }

    }

    channel_replymsg(&message);

}

static void onmain(unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk(FILE_L0, FILE_G0, "addr"))
        socket_resolvelocal(FILE_L0, &local);

    if (file_walk(FILE_L0, FILE_G0, "data"))
    {

        unsigned char buffer[BUFFER_SIZE];
        unsigned int count = buildrequest(BUFFER_SIZE, buffer);

        file_link(FILE_L0);
        socket_resolveremote(FILE_L0, &local, &router);
        socket_send_udp(FILE_L0, &local, &remote, &router, count, buffer);

        count = socket_receive_udp(FILE_L0, &local, &remote, &router, buffer, BUFFER_SIZE);

        if (count)
        {

            struct dns_header *header = (struct dns_header *)buffer;
            unsigned int responselength = sizeof (struct dns_header);
            unsigned int i;

            for (i = 0; i < net_load16(header->questions); i++)
            {

                char *name;

                name = (char *)(buffer + responselength);
                responselength += dns_namesize(name);
                responselength += sizeof (struct dns_question);

            }

            for (i = 0; i < net_load16(header->answers); i++)
            {

                struct dns_answer *answer;
                char *name;
                void *rddata;

                name = (char *)(buffer + responselength);
                responselength += dns_namesize(name);
                answer = (struct dns_answer *)(buffer + responselength);
                responselength += sizeof (struct dns_answer);
                rddata = (buffer + responselength);
                responselength += net_load16(answer->rdlength);

                printmain(net_load16(answer->type), name, rddata, buffer);

            }

        }

        file_unlink(FILE_L0);

    }

    channel_close();

}

static void onoption(unsigned int source, void *mdata, unsigned int msize)
{

    char *key = mdata;
    char *value = key + ascii_lengthz(key);

    if (ascii_match(key, "ethernet"))
        file_walk2(FILE_G0, value);

    if (ascii_match(key, "domain"))
        ascii_copy(domain, value);

    if (ascii_match(key, "local-address"))
        socket_bind_ipv4s(&local, value);

    if (ascii_match(key, "local-port"))
        socket_bind_udps(&local, value);

    if (ascii_match(key, "remote-address") || ascii_match(key, "address"))
        socket_bind_ipv4s(&remote, value);

    if (ascii_match(key, "remote-port") || ascii_match(key, "port"))
        socket_bind_udps(&remote, value);

    if (ascii_match(key, "router-address"))
        socket_bind_ipv4s(&router, value);

}

static void onquery(unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk(FILE_L0, FILE_G0, "addr"))
        socket_resolvelocal(FILE_L0, &local);

    if (file_walk(FILE_L0, FILE_G0, "data"))
    {

        unsigned char buffer[BUFFER_SIZE];
        unsigned int count = buildrequest(BUFFER_SIZE, buffer);

        file_link(FILE_L0);
        socket_resolveremote(FILE_L0, &local, &router);
        socket_send_udp(FILE_L0, &local, &remote, &router, count, buffer);

        count = socket_receive_udp(FILE_L0, &local, &remote, &router, buffer, BUFFER_SIZE);

        if (count)
        {

            struct dns_header *header = (struct dns_header *)buffer;
            unsigned int responselength = sizeof (struct dns_header);
            unsigned int i;

            for (i = 0; i < net_load16(header->questions); i++)
            {

                char *name;

                name = (char *)(buffer + responselength);
                responselength += dns_namesize(name);
                responselength += sizeof (struct dns_question);

            }

            for (i = 0; i < net_load16(header->answers); i++)
            {

                struct dns_answer *answer;
                char *name;
                void *rddata;

                name = (char *)(buffer + responselength);
                responselength += dns_namesize(name);
                answer = (struct dns_answer *)(buffer + responselength);
                responselength += sizeof (struct dns_answer);
                rddata = (buffer + responselength);
                responselength += net_load16(answer->rdlength);

                /* just print one query for now and only if type is 1 */
                if (net_load16(answer->type) == 1)
                {

                    printquery(mdata, msize, net_load16(answer->type), name, rddata, buffer);

                    break;

                }

            }

        }

        file_unlink(FILE_L0);

    }

    channel_close();

}

void init(void)
{

    file_walk2(FILE_G0, "system:ethernet/if:0");
    socket_init(&local);
    socket_bind_ipv4s(&local, "10.0.5.1");
    socket_bind_udps(&local, "50000");
    socket_init(&remote);
    socket_bind_ipv4s(&remote, "8.8.8.8");
    socket_bind_udps(&remote, "53");
    socket_init(&router);
    socket_bind_ipv4s(&router, "10.0.5.80");
    channel_setcallback(EVENT_MAIN, onmain);
    channel_setcallback(EVENT_OPTION, onoption);
    channel_setcallback(EVENT_QUERY, onquery);

}

