all: src/sample_app_arcface_img.cpp
	${CXX} -std=c++14 src/sample_app_arcface_img.cpp \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/face_recognition_img_app

clean:
	rm -rf exe/face_recognition_img_app
