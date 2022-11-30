all: 01_head_count \

# List all the files
SRC=examples/01_head_count/head_count_example.cpp \
	src/01_head_count/sample_app_yolov3_img.h \
	src/01_head_count/sample_app_yolov3_img.cpp \
        src/01_head_count/ascii.cpp \
        src/01_head_count/ascii.h \
        src/01_head_count/box.cpp \
        src/01_head_count/box.h \
        src/01_head_count/image.cpp \
        src/01_head_count/image.h \
        src/01_head_count/define.h

INC_DIR = -I src/01_head_count/
INC_DIR += -I ${SDKTARGETSYSROOT}/usr/include/opencv4/
CFLAGS = ${INC_DIR}
CFLAGS += -DYOLOV3
LIBFLAGS = -lopencv_core -lopencv_imgproc -lopencv_imgcodecs

01_head_count: ${SRC}
	${CXX} -std=c++14 $(filter %.cpp,${SRC}) ${CFLAGS} -L ${SDKTARGETSYSROOT}/usr/lib64/ ${LIBFLAGS} -lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/01_head_count/01_head_count_app

clean:
	rm -rf exe/01_head_count/01_head_count_app
