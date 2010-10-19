#include <call.h>
#include <file.h>
#include <session.h>
#include <vfs.h>

void main(int argc, char *argv[])
{

    struct vfs_node *node;

    if (argc == 2)
        node = call_open(argv[1]);
    else
        node = call_open("/");

    if (!node)
    {

        file_write(session_get_out(), "Directory does not exist.\n");

        return;

    }

    if (!node->walk)
    {

        file_write(session_get_out(), "Not a directory.\n");

        return;

    }

    struct vfs_node *current;

    unsigned int i;

    for (i = 0; (current = vfs_walk(node, i)); i++)
    {

        if (current->walk)
        {

            file_write(session_get_out(), "D ");
            file_write(session_get_out(), current->name);

        }

        else
        {

            file_write(session_get_out(), "F ");
            file_write(session_get_out(), current->name);

        }

        file_write(session_get_out(), "\n");

    }

}

