all:	07_animal_detect_img \

OPENCV_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/opencv4 \
	-lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui
	
BSP_SDK_FLAG = \
	-ljpeg -lwebp -ltiff -lz -ltbb -lgtk-3 -lpng16 -lgdk-3 -lcairo  \
	-llzma -lrt -lcairo-gobject \
	-lxkbcommon -lwayland-cursor -lwayland-egl -lwayland-client -lepoxy \
	-lfribidi -lharfbuzz -lfontconfig \
	-lglib-2.0 -lgobject-2.0 -lgdk_pixbuf-2.0 -lgmodule-2.0 -lpangocairo-1.0 \
	-latk-1.0 -lgio-2.0 -lpango-1.0 -lfreetype -lpixman-1 -luuid -lpcre \
	-lmount -lresolv -lexpat -lpangoft2-1.0 -lblkid \
	
# List all the files in src directory
SRC_AD_IMG=examples/animal_detect_example.cpp \
	src/sample_app_yolov3_img.h \
	src/sample_app_yolov3_img.cpp \
	src/ascii.cpp \
	src/ascii.h \
	src/box.cpp \
	src/box.h \
	src/image.cpp \
	src/image.h \
	src/define.h \


INC_DIR_AD_IMG = -I src/
CFLAGS_AD_IMG = ${INC_DIR_AD_IMG}
CFLAGS_AD_IMG += -DYOLOV3

07_animal_detect_img: ${SRC_HC_IMG}
	${CXX} -std=c++14 $(filter %.cpp,${SRC_AD_IMG}) ${CFLAGS_AD_IMG} \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/animal_detection_img_app



clean:
	rm -rf exe/animal_detection_img_app
