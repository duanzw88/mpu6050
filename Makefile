VPATH = src include
MKDIR = mkdir
INCLUDE_DIR = include

CC=gcc
CFLAGS= -std=c99
CXX = g++

OBJ_DIR=../objs
objects = I2Cdev.o MPU6050.o
OBJS = $(addprefix $(OBJ_DIR)/,$(objects))

LIB_DIR = ../libs
lib = libmpu6050
LIB_S=$(addprefix $(LIB_DIR)/,$(lib))
LIB_STATIC=$(addsuffix .a,$(LIB_S))
LIB_D=$(addprefix $(LIB_DIR)/,$(lib))



all:$(OBJ_DIR) $(LIB_DIR) $(LIB_STATIC) $(LIB_DYNAMIC)

$(OBJ_DIR):
	$(MKDIR) $@
$(LIB_DIR):
	$(MKDIR) $@
$(LIB_STATIC):$(OBJS)
	ar crs $@ $(OBJS)
$(LIB_DYNAMIC):$(OBJS)
	$(CC) -shared -Wall -fPIC $^ -o $@

$(OBJ_DIR)/%.o:src/%.cpp
	$(CXX) -g -o $@ -c $^ -I$(INCLUDE_DIR)



clean:
	$(RM) $(OBJS)
