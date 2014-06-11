MOD:=$(MODULES_ARCH_PATH)/pci/pci.ko
OBJ:=$(MODULES_ARCH_PATH)/pci/main.o

$(MOD): $(OBJ) $(LIBFUDGE)
	$(LD) $(LDFLAGS) -o $@ $^

MODULES+=$(MOD)
MODULES_OBJECTS+=$(OBJ)
