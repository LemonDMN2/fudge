TARGET2:=i386-unknown-elf

AR=$(TARGET2)-ar rcs
AS=$(TARGET2)-as -c -o
CC=$(TARGET)cc -S -no-leading-underscore
NM=$(TARGET2)-nm -gp
PP=$(TARGET)pp -I$(DIR_INCLUDE) -I$(DIR_LIB) -I$(DIR_SRC)
LD_BIN=$(TARGET)l -elf -entry _start -o
#LD_KBIN=$(TARGET)l -elf -origin 0x100000 -entry mboot_init -nobss -o
LD_KBIN=$(TARGET2)-ld -static -nostdlib -T$(DIR_SRC)/kernel/$(ARCH)/$(LOADER)/linker.ld -o
LD_KMOD=$(TARGET2)-ld -static -nostdlib -T$(DIR_SRC)/modules/linker.ld -r -o
NASM=nasm -f elf -o

%.d: %.s
	@echo DP $@
	@echo $*.o: > $*.d

%.d: %.c
	@echo DP $@
	@echo $*.o: > $*.d

%.i: %.c
	@echo PP $@
	@$(PP) $^ > $@

%.asm: %.i
	@echo CC $@
	@$(CC) $^

%.o: %.s
	@echo AS $@
	@$(AS) $@ $^

%.map: %
	@echo NM $@
	@$(NM) $^ > $@

%.o: %.asm
	@echo NASM $@
	@$(NASM) $@ $^
