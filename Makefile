CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = main
APP_OBJ_FILES = main.o mfrc522/mfrc522.o database/db.o third_party/cjson/cJSON.o

LIBS = 

all: $(APP_NAME)

$(APP_NAME) : $(APP_OBJ_FILES)
	$(CXX) $^ -o $@  $(LIBS)

%.o : %.cc
	$(CXX) -c $^ -o $@

clean:
	rm -f *.o mfrc522/*.o database/*.o third_party/cjson/*.o $(APP_NAME)