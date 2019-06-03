CC = gcc
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -I$(INCLUDE_DIR)
CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

EXECUTABLE_FILE = server-alsasrc-OPUS.out

OBJECTS = \
	$(BUILD_DIR)/server-alsasrc-OPUS.o

DynamicSensitivityList = \
	$(SRC_DIR)/server-alsasrc-OPUS.c \

all: $(BUILD_DIR) $(EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/server-alsasrc-OPUS.o: $(DynamicSensitivityList)
	$(CC) -c $(SRC_DIR)/server-alsasrc-OPUS.c -o $(BUILD_DIR)/server-alsasrc-OPUS.o $(CFLAGS)

$(EXECUTABLE_FILE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE_FILE) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out