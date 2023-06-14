CC := g++

DIR := .
SRC_DIR := .
ASSET_DIR := .
WIN_DIR := .
BUILD_DIR := build
INCLUDE_DIR := include
STD := c++14
OPTPARAMS := -O1

.PHONY: clean

build_dir:
	mkdir -p ${BUILD_DIR}

compile: build_dir \
		${BUILD_DIR}/main.o \
		${BUILD_DIR}/glad.o 

${BUILD_DIR}/glad.o:${SRC_DIR}/glad.cpp
	${CC} -std=${STD} -c ${OPTPARAMS} ${SRC_DIR}/glad.cpp -o ${BUILD_DIR}/glad.o -I ${INCLUDE_DIR}

${BUILD_DIR}/main.o:main.cpp
	${CC} -std=${STD} ${OPTPARAMS} -c main.cpp -o ${BUILD_DIR}/main.o -I ${INCLUDE_DIR} -I ../ -I ${SRC_DIR}
	
link:compile
	${CC} ${BUILD_DIR}/*.o -o ${BUILD_DIR}/main.exe -Llib -lglfw3dll
	
run:link
	${BUILD_DIR}/main.exe

clean:
	rm -f ${BUILD_DIR}/*.o 
	rm -f ${BUILD_DIR}/*.exe
	rm -f ${BUILD_DIR}/main.exe