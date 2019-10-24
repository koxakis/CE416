APP = assignment_part1
CC=g++
CLEANBROKEN = '1'
LIBS = -g -lGL -lGLU -lX11 -lpthread -lXrandr -lXi -ldl -Lusr/lib/libGLEW.so -lGLEW -lglfw -lXxf86vm -lXinerama -lXcursor -fpermissive
SRC = assignment_part1.cpp
OBJ = $(SRC:.ccp=.o)

all: build

build: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(DEPS) $(LIBS) -o $@ 

%.o: %.cpp 
	$(CC) $< $(DEPS) $(LIBS) -c -o $@ 

clean:
	rm -rf assignment_part1 *.o
