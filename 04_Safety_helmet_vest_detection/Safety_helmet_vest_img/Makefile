all: 04_Safety_helmet_vest_img \

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

# List all the files
SRC_SHV_IMG=examples/Safety_helmet_vest_img_example.cpp \
	src/sample_app_yolov3_img.h \
	src/sample_app_yolov3_img.cpp \
        src/ascii.cpp \
        src/ascii.h \
        src/box.cpp \
        src/box.h \
        src/image.cpp \
        src/image.h \
        src/define.h \


INC_DIR_SHV_IMG = -I src/
CFLAGS_SHV_IMG = ${INC_DIR_SHV_IMG}
CFLAGS_SHV_IMG += -DYOLOV3


04_Safety_helmet_vest_img: ${SRC_SHV_IMG}
	${CXX} -std=c++14 $(filter %.cpp,${SRC_SHV_IMG}) ${CFLAGS_SHV_IMG} \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/safety_helmet_vest_img_app

clean:
	rm -rf exe/safety_helmet_vest_img_app
