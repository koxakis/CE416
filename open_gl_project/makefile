APP = app
CC=g++
CLEANBROKEN = '1'
LIBS = -Wall -g -lGL -lX11 -lpthread -lXrandr -lXi -ldl -L./glad.h -Lusr/lib/libGLEW.so -lGLEW -Lusr/lib64/libglfw.so -lglfw -lXxf86vm -lXinerama -lXcursor -fpermissive -I.
SRC = app.cpp glad.c
OBJ = $(SRC:.ccp=.o)

all: build

build: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(DEPS) $(LIBS) -o $@ 

%.o: %.cpp 
	$(CC) $< $(DEPS) $(LIBS) -c -o $@ 

clean:
	rm -rf app *.o
