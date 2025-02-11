#include <fudge.h>
#include <abi.h>

static unsigned int readheader(unsigned int descriptor, struct elf_header *header)
{

    return file_seekreadall(descriptor, header, ELF_HEADER_SIZE, 0);

}

static unsigned int readsectionheader(unsigned int descriptor, struct elf_header *header, unsigned int index, struct elf_sectionheader *sectionheader)
{

    return file_seekreadall(descriptor, sectionheader, header->shsize, header->shoffset + index * header->shsize);

}

static unsigned int findvalue(unsigned int descriptor, struct elf_header *header, struct elf_sectionheader *symbolheader, char *strings, unsigned int count, char *symbolname)
{

    unsigned int i;

    for (i = 0; i < symbolheader->size / symbolheader->esize; i++)
    {

        struct elf_symbol symbol;

        if (!file_seekreadall(descriptor, &symbol, symbolheader->esize, symbolheader->offset + i * symbolheader->esize))
            return 0;

        if (strings[symbol.name + count] != '\0')
            continue;

        if (!buffer_match(symbolname, &strings[symbol.name], count))
            continue;

        if (header->type == ELF_TYPE_RELOCATABLE)
        {

            struct elf_sectionheader referenceheader;

            if (!readsectionheader(descriptor, header, symbol.shindex, &referenceheader))
                return 0;

            return referenceheader.address + referenceheader.offset + symbol.value;

        }

        return symbol.value;

    }

    return 0;

}

static unsigned int findsymbol(unsigned int descriptor, unsigned int count, char *symbolname)
{

    struct elf_header header;
    unsigned int i;

    file_reset(descriptor);

    if (!readheader(descriptor, &header))
        return 0;

    if (!elf_validate(&header))
        return 0;

    for (i = 0; i < header.shcount; i++)
    {

        struct elf_sectionheader symbolheader;
        struct elf_sectionheader stringheader;
        char strings[BUFFER_SIZE * 4];
        unsigned int value;

        if (!readsectionheader(descriptor, &header, i, &symbolheader))
            return 0;

        if (symbolheader.type != ELF_SECTION_TYPE_SYMTAB)
            continue;

        if (!readsectionheader(descriptor, &header, symbolheader.link, &stringheader))
            return 0;

        if (stringheader.size > BUFFER_SIZE * 4)
            return 0;

        if (!file_seekreadall(descriptor, strings, stringheader.size, stringheader.offset))
            return 0;

        value = findvalue(descriptor, &header, &symbolheader, strings, count, symbolname);

        if (value)
            return value;

    }

    return 0;

}

static unsigned int findmodulesymbol(unsigned int count, char *symbolname)
{

    unsigned int address = 0;
    unsigned int offset = 0;
    char module[32];

    offset += buffer_write(module, 32, symbolname, buffer_findbyte(symbolname, count, '_'), offset);
    offset += buffer_write(module, 32, ".ko", 4, offset);

    if (file_walk(FILE_L0, FILE_G0, module))
        address = findsymbol(FILE_L0, count, symbolname);

    if (!address)
        address = findsymbol(FILE_G1, count, symbolname);

    return address;

}

static unsigned int resolvesymbols(unsigned int descriptor, struct elf_sectionheader *relocationheader, struct elf_sectionheader *symbolheader, char *strings, unsigned int offset)
{

    unsigned int i;

    for (i = 0; i < relocationheader->size / relocationheader->esize; i++)
    {

        unsigned char index;
        unsigned int address;
        struct elf_relocation relocation;
        struct elf_symbol symbol;

        if (!file_seekreadall(descriptor, &relocation, relocationheader->esize, relocationheader->offset + i * relocationheader->esize))
            return 0;

        index = relocation.info >> 8;

        if (!file_seekreadall(descriptor, &symbol, symbolheader->esize, symbolheader->offset + index * symbolheader->esize))
            return 0;

        if (symbol.shindex)
            continue;

        address = findmodulesymbol(cstring_length(strings + symbol.name), strings + symbol.name);

        if (address)
        {

            unsigned int value;

            if (!file_seekreadall(descriptor, &value, 4, offset + relocation.offset))
                return 0;

            value += address;

            if (!file_seekwriteall(descriptor, &value, 4, offset + relocation.offset))
                return 0;

        }

    }

    return 1;

}

static unsigned int resolve(unsigned int descriptor)
{

    struct elf_header header;
    unsigned int i;

    if (!readheader(descriptor, &header))
        return 0;

    if (!elf_validate(&header))
        return 0;

    for (i = 0; i < header.shcount; i++)
    {

        struct elf_sectionheader relocationheader;
        struct elf_sectionheader dataheader;
        struct elf_sectionheader symbolheader;
        struct elf_sectionheader stringheader;
        char strings[BUFFER_SIZE];

        if (!readsectionheader(descriptor, &header, i, &relocationheader))
            return 0;

        if (relocationheader.type != ELF_SECTION_TYPE_REL)
            continue;

        if (!readsectionheader(descriptor, &header, relocationheader.info, &dataheader))
            return 0;

        if (!readsectionheader(descriptor, &header, relocationheader.link, &symbolheader))
            return 0;

        if (!readsectionheader(descriptor, &header, symbolheader.link, &stringheader))
            return 0;

        if (stringheader.size > BUFFER_SIZE)
            return 0;

        if (!file_seekreadall(descriptor, strings, stringheader.size, stringheader.offset))
            return 0;

        if (!resolvesymbols(descriptor, &relocationheader, &symbolheader, strings, dataheader.offset))
            return 0;

    }

    return 1;

}

static void onpath(unsigned int source, void *mdata, unsigned int msize)
{

    if (file_walk2(FILE_G2, mdata))
    {

        if (resolve(FILE_G2))
            call_load(FILE_G2);

    }

}

void init(void)
{

    if (!file_walk2(FILE_G0, "/kernel"))
        return;

    if (!file_walk(FILE_G1, FILE_G0, "fudge"))
        return;

    channel_bind(EVENT_PATH, onpath);

}

