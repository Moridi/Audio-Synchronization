CC = gcc
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = `pkg-config --cflags --libs gstreamer-1.0`
CFLAGS += -I$(INCLUDE_DIR)

EXECUTABLE_FILE = client.out

OBJECTS = \
	$(BUILD_DIR)/Main.o \
	$(BUILD_DIR)/Logger.o \
	$(BUILD_DIR)/Client.o \

MainSensitivityList = \
	$(SRC_DIR)/Main.c \
	$(INCLUDE_DIR)/Client.h \

LoggerSensitivityList = \
	$(SRC_DIR)/Logger.c \
	$(INCLUDE_DIR)/Logger.h \

ClientSensitivityList = \
	$(SRC_DIR)/Client.c \
	$(INCLUDE_DIR)/Client.h \
	$(INCLUDE_DIR)/Config.h \
	$(INCLUDE_DIR)/Logger.h \

all: $(BUILD_DIR) $(EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Main.o: $(MainSensitivityList)
	$(CC) -c $(SRC_DIR)/Main.c -o $(BUILD_DIR)/Main.o $(CFLAGS)

$(BUILD_DIR)/Logger.o: $(LoggerSensitivityList)
	$(CC) -c $(SRC_DIR)/Logger.c -o $(BUILD_DIR)/Logger.o $(CFLAGS)

$(BUILD_DIR)/Client.o: $(ClientSensitivityList)
	$(CC) -c $(SRC_DIR)/Client.c -o $(BUILD_DIR)/Client.o $(CFLAGS)

$(EXECUTABLE_FILE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE_FILE) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out