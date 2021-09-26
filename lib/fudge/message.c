#include "buffer.h"
#include "ascii.h"
#include "message.h"

unsigned int message_headersize(struct message_header *header)
{

    return sizeof (struct message_header);

}

unsigned int message_datasize(struct message_header *header)
{

    return header->length - message_headersize(header);

}

unsigned int message_putbuffer(struct message *message, unsigned int count, void *buffer, unsigned int offset)
{

    return offset + buffer_write(message->data.buffer, MESSAGE_SIZE, buffer, count, offset);

}

unsigned int message_putstring(struct message *message, char *string, unsigned int offset)
{

    return message_putbuffer(message, ascii_length(string), string, offset);

}

unsigned int message_putstringz(struct message *message, char *string, unsigned int offset)
{

    return message_putbuffer(message, ascii_lengthz(string), string, offset);

}

unsigned int message_putvalue(struct message *message, int value, unsigned int base, unsigned int padding, unsigned int offset)
{

    char num[ASCII_NUMSIZE];

    return message_putbuffer(message, ascii_wvalue(num, ASCII_NUMSIZE, value, base, padding), num, offset);

}

unsigned int message_putbuffer2(struct message *message, unsigned int count, void *buffer)
{

    return message->header.length += buffer_write(message->data.buffer, MESSAGE_SIZE, buffer, count, message_datasize(&message->header));

}

unsigned int message_putstring2(struct message *message, char *string)
{

    return message_putbuffer2(message, ascii_length(string), string);

}

unsigned int message_putstringz2(struct message *message, char *string)
{

    return message_putbuffer2(message, ascii_lengthz(string), string);

}

unsigned int message_putvalue2(struct message *message, int value, unsigned int base, unsigned int padding)
{

    char num[ASCII_NUMSIZE];

    return message_putbuffer2(message, ascii_wvalue(num, ASCII_NUMSIZE, value, base, padding), num);

}

void message_initheader(struct message_header *header, unsigned int event, unsigned int length)
{

    header->event = event;
    header->source = 0;
    header->length = message_headersize(header) + length;

}

void message_init(struct message *message, unsigned int event)
{

    message_initheader(&message->header, event, 0);

}

