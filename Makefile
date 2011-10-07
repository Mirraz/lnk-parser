CC=gcc
LD=gcc
LIBFILES=
INCLUDES=
DEFINES=
OPTIMIZATIONS=-Os -fomit-frame-pointer -pipe
CFLAGS=-Wall $(OPTIMIZATIONS) $(DEFINES) $(INCLUDES)
LDFLAGS=-Wall -ansi $(LIBFILES)
SRC_DIR=src
BUILD_DIR=build
EXECUTABLE=$(BUILD_DIR)/lnk_parser



all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(EXECUTABLE): $(BUILD_DIR)/main.o
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/main.h
	$(CC) -o $@ $< -c $(CFLAGS)



clean:
	rm -rf $(BUILD_DIR)

