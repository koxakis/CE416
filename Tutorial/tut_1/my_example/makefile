APP = test
CC=g++
CLEANBROKEN = '1'
LIBS = -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lGLU -lglfw -lXxf86vm -lXinerama -lXcursor -lglad
#LIBS = -lGAEW -lglfw -lGL -L/run/media/nikolas/Linux_Data/Git_Repos/glfw/include/GLFW/glfw3.h
SRC = test.cpp
OBJ = $(SRC:.ccp=.o)

all: build

build: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(DEPS) $(LIBS) -o $@ 

%.o: %.cpp 
	$(CC) $< $(DEPS) $(LIBS) -c -o $@ 

clean:
	rm -rf test *.o
