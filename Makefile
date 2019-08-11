CC?=gcc
CXX?=g++
CXXFLAGS=-std=c++17 -I./muduo/include
LFLAGS=-L./lib -lmuduo_base -lmuduo_net -lpthread

OBJ_DIR=objs
SRC_DIR=src

INC=$(wildcard $(SRC_DIR)/*.h)
SRC=$(wildcard $(SRC_DIR)/*.cc)
OBJ=$(patsubst %.cc, ${OBJ_DIR}/%.o, $(notdir ${SRC}))

all:prepare hades

prepare:prepare-dep

prepare-dep:
	@mkdir -p $(OBJ_DIR)

hades:$(OBJ)
	$(CXX) -o $@ $(OBJ) $(LFLAGS) 

${OBJ_DIR}/%.o:$(SRC_DIR)/%.cc $(INC)
	${CXX} ${CXXFLAGS} -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR) hades
