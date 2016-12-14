objects = setgraph.o canary.o tests.o
targets = findMinor filterMinor runTests

NAUTY=nauty26r7
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

.phony: all clean very-clean debug nonauty folders tests force

all: $(NAUTY)/nauty.a all-canary
all-canary: folders $(obj_paths) $(target_paths)
force:
clean: force
	-rm -r $(OBJECT_DIR)/*.o $(EXE_DIR)/*.dSYM $(target_paths)
very-clean: clean
	#cd $(NAUTY); make clean
	-rm -r $(NAUTY)
debug: clean all
nonauty: all-canary
nonauty-debug: clean all-canary
tests: all force
	./exe/runTests

folders: obj exe
obj exe:
	mkdir -p $@

$(NAUTY)/nauty.a:
	-test ! -d $(NAUTY) && tar xf $(NAUTY).tar.gz
	cd $(NAUTY); ./configure
	cd $(NAUTY); make

$(EXE_DIR)/findMinor: $(SOURCE_DIR)/findMinor.c $(obj_paths) makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/filterMinor: $(SOURCE_DIR)/filterMinor.c $(obj_paths) makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/runTests: $(SOURCE_DIR)/runTests.c $(obj_paths) makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@

$(OBJECT_DIR)/setgraph.o: $(addprefix $(SOURCE_DIR)/,setgraph.c setgraph.h bitset.h) makefile
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/canary.o: $(addprefix $(SOURCE_DIR)/,canary.c canary.h bitset.h setgraph.h debug.h) makefile
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/tests.o: $(addprefix $(SOURCE_DIR)/,test.c test.h canary.h bitset.h setgraph.h debug.h) makefile
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@
