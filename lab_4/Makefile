APP = movelight
CXX=g++
CLEANBROKEN = '1'
LIBS =  -lglut -lGLU -lGL
SRC = movelight.c  
OBJ = $(SRC:.cpp=.o)


all: build

build: $(APP)
	
clean:
	  rm -rf movelight *.o
$(APP): $(OBJ) 
	$(CXX)  $^  $(DEPS) $(LIBS)  -o $@ -fpermissive

%.o: %.cpp 
	$(CXX)  $<  $(DEPS) $(LIBS) -c -o $@ -fpermissive

#.PHONY:clean
