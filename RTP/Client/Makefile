CC = gcc
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -I$(INCLUDE_DIR)
CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

EXECUTABLE_FILE = client-PCMA.out

OBJECTS = \
	$(BUILD_DIR)/client-PCMA.o

DynamicSensitivityList = \
	$(SRC_DIR)/client-PCMA.c \

all: $(BUILD_DIR) $(EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/client-PCMA.o: $(DynamicSensitivityList)
	$(CC) -c $(SRC_DIR)/client-PCMA.c -o $(BUILD_DIR)/client-PCMA.o $(CFLAGS)

$(EXECUTABLE_FILE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE_FILE) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out