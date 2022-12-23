all: src/sample_app_resnet50_img.cpp
	${CXX} -std=c++14 src/sample_app_resnet50_img.cpp \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/06_Face_recognition_spoof_detection

clean:
	rm -rf exe/06_Face_recognition_spoof_detection
