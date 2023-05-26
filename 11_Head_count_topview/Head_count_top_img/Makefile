all: 11_headcount_top_img_app \

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
SRC_HC_IMG=examples/headcount_top_img_example.cpp \
	src/sample_app_headcount_top_img.h \
	src/sample_app_headcount_top_img.cpp \
        src/ascii.cpp \
        src/ascii.h \
        src/box.cpp \
        src/box.h \
        src/image.cpp \
        src/image.h \
        src/define.h \


INC_DIR_HC_IMG = -I src/
CFLAGS_HC_IMG = ${INC_DIR_HC_IMG}
CFLAGS_HC_IMG += -DYOLOV3


11_headcount_top_img_app: ${SRC_HC_IMG}
	${CXX} -std=c++14 $(filter %.cpp,${SRC_HC_IMG}) ${CFLAGS_HC_IMG} \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/11_headcount_top_img_app

clean:
	rm -rf exe/11_headcount_top_img_app
