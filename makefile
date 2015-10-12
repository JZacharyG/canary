objects = setgraph.o canary.o
targets = findMinor filterMinor #genmf

NAUTY=../nauty25r9
#MNE=../mne-files
#MNELIB=199805
#-I$(NAUTY) -I$(MNE) -L$(MNE) -L$(NAUTY) -l$(MNELIB)
CC = cc
CFLAGS = -Ofast -I$(NAUTY) $(NAUTY)/nauty.a
debug: CFLAGS = -O0 -I$(NAUTY) $(NAUTY)/nauty.a -g -DDEBUG=1
nonauty: CFLAGS = -Ofast -DEXCLUDE_NAUTY
SOURCE_DIR = src
OBJECT_DIR = obj
EXE_DIR = exe
TEST_DIR = tst
obj_paths = $(addprefix $(OBJECT_DIR)/,$(objects))
target_paths = $(addprefix $(EXE_DIR)/,$(targets))

.phony: all clean debug nonauty

all: $(obj_paths) $(target_paths)
clean:
	-rm -d $(OBJECT_DIR)/*.o $(EXE_DIR)/*.dSYM $(target_paths)
debug: clean all
nonauty: clean all

$(EXE_DIR)/findMinor: $(SOURCE_DIR)/findMinor.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/filterMinor: $(SOURCE_DIR)/filterMinor.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/genmf: $(SOURCE_DIR)/genmf.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@

$(OBJECT_DIR)/setgraph.o: $(addprefix $(SOURCE_DIR)/,setgraph.c setgraph.h bitset.h)
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/canary.o: $(addprefix $(SOURCE_DIR)/,canary.c canary.h bitset.h setgraph.h debug.h)
	$(CC) -c $(CFLAGS) $< -o $@
