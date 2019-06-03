CC = gcc
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -I$(INCLUDE_DIR)
CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`

EXECUTABLE_FILE = DynamicElements.out

OBJECTS = \
	$(BUILD_DIR)/DynamicElements.o

DynamicSensitivityList = \
	$(SRC_DIR)/DynamicElements.c \

all: $(BUILD_DIR) $(EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/DynamicElements.o: $(DynamicSensitivityList)
	$(CC) -c $(SRC_DIR)/DynamicElements.c -o $(BUILD_DIR)/DynamicElements.o $(CFLAGS)

$(EXECUTABLE_FILE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE_FILE) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out