objects = setgraph.o canary.o
targets = findMinor filterMinor #genmf

CC = cc
CFLAGS = -Ofast
debug: CFLAGS = -O0 -g -DDEBUG=1
SOURCE_DIR = src
OBJECT_DIR = obj
EXE_DIR = exe
TEST_DIR = tst
obj_paths = $(addprefix $(OBJECT_DIR)/,$(objects))
target_paths = $(addprefix $(EXE_DIR)/,$(targets))

.phony: all clean debug

all: $(obj_paths) $(target_paths)
clean:
	-rm $(OBJECT_DIR)/*.o $(EXE_DIR)/*.dSYM $(target_paths)
debug: clean all

$(EXE_DIR)/findMinor: $(SOURCE_DIR)/findMinor.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/filterMinor: $(SOURCE_DIR)/filterMinor.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@
$(EXE_DIR)/genmf: $(SOURCE_DIR)/genmf.c $(obj_paths)
	$(CC) $(CFLAGS) $< $(obj_paths) -o $@

$(OBJECT_DIR)/setgraph.o: $(addprefix $(SOURCE_DIR)/,setgraph.c setgraph.h set.h)
	$(CC) -c $(CFLAGS) $< -o $@
$(OBJECT_DIR)/canary.o: $(addprefix $(SOURCE_DIR)/,canary.c canary.h set.h setgraph.h debug.h)
	$(CC) -c $(CFLAGS) $< -o $@
