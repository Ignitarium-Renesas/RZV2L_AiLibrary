all: src/sample_app_resnet50_img.cpp
	${CXX} -std=c++14 src/sample_app_resnet50_img.cpp \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/Face_spoof_detection_img

clean:
	rm -rf exe/Face_spoof_detection_img
