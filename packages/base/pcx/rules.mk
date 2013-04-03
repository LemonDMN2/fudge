BIN_DATA:=packages/base/pcx/pcxdata
OBJ_DATA:=packages/base/pcx/pcxdata.o
BIN_CMAP:=packages/base/pcx/pcxcmap
OBJ_CMAP:=packages/base/pcx/pcxcmap.o

$(BIN_DATA): $(OBJ_DATA) $(LIBFUDGE) $(EXTRA)
	$(LD) $(LDFLAGS) -o $@ $^

$(BIN_CMAP): $(OBJ_CMAP) $(LIBFUDGE) $(EXTRA)
	$(LD) $(LDFLAGS) -o $@ $^

PACKAGES+=$(BIN_DATA) $(BIN_CMAP)
PACKAGES_OBJECTS+=$(OBJ_DATA) $(OBJ_CMAP)
