
DIR_INC = -I./include
# LIBS_PATH = -L./opencv -L./libs
# LIBS_PATH = -L/home/tech3/work0/3D/gesture/gesture4arm/libs_firefly/opencv
# LIBS_PATH = -L/home/tech3/work0/opencv/opencv-3.4.3/build/install/lib
DIR_SRC = .
DIR_OBJ = .
#DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp)
# SRC += $(wildcard ../common/*.cpp)

OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC}))
OBJ += ../common/common.o
OBJ += ../common/protocol.o

TARGET = main_client

BIN_TARGET = ${TARGET}

CC = g++
#CC = aarch64-linux-gnu-g++

# CFLAGS = -O2 -Wall ${DIR_INC} ${LIBS_PATH} -lopencv_core \
# -lopencv_highgui  -lopencv_imgcodecs -lopencv_imgproc \
# -lopencv_videoio -lopencv_video -lncnn -ldl -fopenmp -lncnn -std=c++11
CFLAGS = -O2 -Wall  -std=c++11

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ)  -o $@ $(CFLAGS)

${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@

.PHONY:clean
clean:
	rm *.o
	rm ${TARGET}
