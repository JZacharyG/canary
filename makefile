objects = setgraph.o canary.o
targets = findMinor filterMinor genmf

NAUTY=../nauty25r9
CC = cc
CFLAGS = -Ofast -I$(NAUTY) $(NAUTY)/nauty.a
debug: CFLAGS = -O0 -I$(NAUTY) $(NAUTY)/nauty.a -g -DDEBUG=1
nonauty-debug: CFLAGS = -O0 -DEXCLUDE_NAUTY -g -DDEBUG=1
nonauty: CFLAGS = -Ofast -DEXCLUDE_NAUTY
SOURCE_DIR = src
OBJECT_DIR = obj
EXE_DIR = exe
TEST_DIR = tst
obj_paths = $(addprefix $(OBJECT_DIR)/,$(objects))
target_paths = $(addprefix $(EXE_DIR)/,$(targets))

.phony: all clean debug nonauty

all: ($OBJECT_DIR) ($EXE_DIR) $(obj_paths) $(target_paths)
clean:
	-rm -r $(OBJECT_DIR)/*.o $(EXE_DIR)/*.dSYM $(target_paths)
nonauty: all
debug nonauty-debug: clean all

($OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)
($EXE_DIR):
	mkdir -p $(EXE_DIR)

$(EXE_DIR)/findMinor: $(SOURCE_DIR)/findMinor.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/filterMinor: $(SOURCE_DIR)/filterMinor.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/genmf: $(SOURCE_DIR)/genmf.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@

$(OBJECT_DIR)/setgraph.o: $(addprefix $(SOURCE_DIR)/,setgraph.c setgraph.h bitset.h) makefile
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/canary.o: $(addprefix $(SOURCE_DIR)/,canary.c canary.h bitset.h setgraph.h debug.h) makefile
	$(CC) -c $(CFLAGS) $< -o $@
