#include <call.h>
#include <vfs.h>
#include <write.h>

void main(int argc, char *argv[])
{

    struct vfs_node *node = call_open("dev/rtc");

    unsigned char buffer;

    write_string("20");
    vfs_read(node, 0, 1, &buffer);
    write_string_bcd(buffer);

    write_string("-");

    vfs_read(node, 1, 1, &buffer);
    write_string_bcd(buffer);

    write_string("-");

    vfs_read(node, 2, 1, &buffer);
    write_string_bcd(buffer);

    write_string(" ");

    vfs_read(node, 3, 1, &buffer);
    write_string_bcd(buffer);

    write_string(":");

    vfs_read(node, 4, 1, &buffer);
    write_string_bcd(buffer);

    write_string(":");

    vfs_read(node, 5, 1, &buffer);
    write_string_bcd(buffer);

    vfs_read(node, 3, 1, &buffer);

    if (buffer >> 4)
        write_string("PM\n");
    else
        write_string("AM\n");

}

