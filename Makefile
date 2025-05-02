CC ?= gcc
CXX ?= g++
CPP ?= g++

APP_NAME = main

# Lista de archivos objeto
APP_OBJ_FILES = main.o \
                mfrc522/mfrc522.o \
                database/db.o \
                third_party/cjson/cJSON.o \
                alsa/alsa_play.o\
				pantalla/pantalla.o

# Librerías y banderas de compilación
LIBS = `pkg-config --libs opencv4` -lasound
CXXFLAGS = `pkg-config --cflags opencv4`

all: $(APP_NAME)

$(APP_NAME): $(APP_OBJ_FILES)
	$(CXX) $^ -o $@ $(LIBS)

# Reglas para compilar .cpp y .cc a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o mfrc522/*.o database/*.o third_party/cjson/*.o alsa/*.o $(APP_NAME)
