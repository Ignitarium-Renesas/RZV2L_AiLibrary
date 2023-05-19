all: src/sample_app_arcface_img.cpp
	${CXX} -std=c++14 src/sample_app_arcface_img.cpp \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/Face_recognition_img

clean:
	rm -rf exe/Face_recognition_img
