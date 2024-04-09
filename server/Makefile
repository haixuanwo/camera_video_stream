DIR_INC = -I./include
LIBS_PATH = -L./lib/opencv
#LIBS_PATH = -L/home/tech3/work0/3D/gesture/gesture4arm/libs_firefly/opencv
#LIBS_PATH = -L/home/tech3/work0/opencv/opencv-3.4.3/build/install/lib
DIR_SRC = .
DIR_OBJ = .
#DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = ./main_server

BIN_TARGET = ${TARGET}

CC = g++
#CC = aarch64-linux-gnu-g++

CFLAGS = -O2 -Wall ${DIR_INC} ${LIBS_PATH} -lopencv_core \
-lopencv_highgui  -lopencv_imgcodecs -lopencv_imgproc \
-lopencv_videoio -lopencv_video -ldl -fopenmp -std=c++11

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  -o $@ $(CFLAGS)

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

.PHONY:clean

clean:
	rm ${DIR_OBJ}/*.o
	rm ${TARGET}
