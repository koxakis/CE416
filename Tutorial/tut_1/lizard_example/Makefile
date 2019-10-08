
APP = example
CXX=g++
CLEANBROKEN = '1'
LIBS =  -lglut -lGLU -lGL
SRC = example.cpp  
OBJ = $(SRC:.cpp=.o)


all: build

build: $(APP)
	
clean:
	  rm -rf example *.o
$(APP): $(OBJ) 
	$(CXX)  $^  $(DEPS) $(LIBS)  -o $@ -fpermissive

%.o: %.cpp 
	$(CXX)  $<  $(DEPS) $(LIBS) -c -o $@ -fpermissive

#.PHONY:clean