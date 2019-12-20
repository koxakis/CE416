APP = app
CC=g++
CLEANBROKEN = '1'
LIBS = -g -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lglad -Lusr/lib/libGLEW.so -lGLEW -lglfw -lXxf86vm -lXinerama -lXcursor -fpermissive -I.
SRC = app.cpp
OBJ = $(SRC:.ccp=.o)

all: build

build: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(DEPS) $(LIBS) -o $@ 

%.o: %.cpp 
	$(CC) $< $(DEPS) $(LIBS) -c -o $@ 

clean:
	rm -rf app *.o
