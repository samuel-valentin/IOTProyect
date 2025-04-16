CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = main
APP_OBJ_FILES = main.o mfrc522/mfrc522.o

LIBS = 

all: $(APP_NAME)

$(APP_NAME) : $(APP_OBJ_FILES)
	$(CXX) $^ -o $@  $(LIBS)

%.o : %.cc
	$(CXX) -c $^ -o $@


clean:
	rm *.o mfrc522/*.o $(APP_NAME)