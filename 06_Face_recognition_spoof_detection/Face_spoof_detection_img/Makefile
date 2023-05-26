all: src/sample_app_resnet50_img.cpp
	${CXX} -std=c++14 src/sample_app_resnet50_img.cpp \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/face_spoof_detection_img_app

clean:
	rm -rf exe/face_spoof_detection_img_app
