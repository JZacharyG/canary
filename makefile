objects = setgraph.o canary.o tests.o
targets = findMinor filterMinor runTests

NAUTY=../nauty25r9
CC = cc
CFLAGS = -Ofast -I$(NAUTY) $(NAUTY)/nauty.a
debug: CFLAGS = -O0 -I$(NAUTY) $(NAUTY)/nauty.a -g #-DDEBUG=1
nonauty-debug: CFLAGS = -O0 -DEXCLUDE_NAUTY -g #-DDEBUG=1
nonauty: CFLAGS = -Ofast -DEXCLUDE_NAUTY
SOURCE_DIR = src
OBJECT_DIR = obj
EXE_DIR = exe
TEST_DIR = tst
obj_paths = $(addprefix $(OBJECT_DIR)/,$(objects))
target_paths = $(addprefix $(EXE_DIR)/,$(targets))

.phony: all clean debug nonauty folders tests force

all: folders $(obj_paths) $(target_paths)
force:
clean: force
	-rm -r $(OBJECT_DIR)/*.o $(EXE_DIR)/*.dSYM $(target_paths)
nonauty: all
debug nonauty-debug: clean all
tests: all force
	./exe/runTests

folders: obj exe
obj exe:
	mkdir -p $@

$(EXE_DIR)/findMinor: $(SOURCE_DIR)/findMinor.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/filterMinor: $(SOURCE_DIR)/filterMinor.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/genmf: $(SOURCE_DIR)/genmf.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/vsplit: $(SOURCE_DIR)/vsplit.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/runTests: $(SOURCE_DIR)/runTests.c $(obj_paths) makefile
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@

$(OBJECT_DIR)/setgraph.o: $(addprefix $(SOURCE_DIR)/,setgraph.c setgraph.h bitset.h) makefile
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/canary.o: $(addprefix $(SOURCE_DIR)/,canary.c canary.h bitset.h setgraph.h debug.h) makefile
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/tests.o: $(addprefix $(SOURCE_DIR)/,test.c test.h canary.h bitset.h setgraph.h debug.h) makefile
	$(CC) -c $(CFLAGS) $< -o $@
