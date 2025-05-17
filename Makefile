# Compilers vienen del environment-setup
APP_NAME   = main

C_SRCS     = main.c \
             mfrc522/mfrc522.c \
             database/db.c \
             third_party/cjson/cJSON.c \
             alsa/alsa_play.c

CXX_SRCS   = pantalla/pantalla.cpp

C_OBJS     = $(C_SRCS:.c=.o)
CXX_OBJS   = $(CXX_SRCS:.cpp=.o)
APP_OBJS   = $(C_OBJS) $(CXX_OBJS)

LIBS       = -lasound `pkg-config --cflags --libs opencv4`

all: $(APP_NAME)

$(APP_NAME): $(APP_OBJS)
	$(CXX) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC)   -Ithird_party/cjson -c $< -o $@

%.o: %.cpp
	$(CXX) -std=c++11 -Ithird_party/cjson -c $< -o $@

clean:
	rm -f $(APP_OBJS) $(APP_NAME)
