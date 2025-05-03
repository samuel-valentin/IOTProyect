CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = main
APP_OBJ_FILES = main.o pantalla/pantalla.o mfrc522/mfrc522.o database/db.o third_party/cjson/cJSON.o alsa/alsa_play.o

LIBS = -lasound -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs

all: $(APP_NAME)

$(APP_NAME) : $(APP_OBJ_FILES)
	$(CXX) $^ -o $@  $(LIBS)

%.o : %.cc
	$(CXX) -c $^ -o $@ $(LIBS)

clean:
	rm -f *.o mfrc522/*.o database/*.o third_party/cjson/*.o alsa/*.o pantalla/*.o $(APP_NAME)
