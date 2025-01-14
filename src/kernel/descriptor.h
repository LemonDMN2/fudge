struct descriptor
{

    struct service *service;
    unsigned int id;
    unsigned int offset;
    unsigned int current;
    unsigned int count;

};

unsigned int descriptor_check(struct descriptor *descriptor);
void descriptor_copy(struct descriptor *descriptor, struct descriptor *source);
void descriptor_reset(struct descriptor *descriptor);
void descriptor_init(struct descriptor *descriptor);
