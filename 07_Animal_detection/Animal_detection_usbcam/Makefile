all: 07_Animal_detection_usbcam_app \

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
SRC_AD_CAM = examples/animal_detect_usbcam_example.cpp \
        src/ascii.cpp \
        src/ascii.h \
        src/box.cpp \
        src/box.h \
        src/image.cpp \
        src/image.h \
        src/define.h \
        src/camera.cpp \
        src/camera.h \
        src/wayland.cpp \
        src/wayland.h \
        src/sample_app_yolov3_cam.cpp \
        src/sample_app_yolov3_cam.h \

INC_DIR_AD_CAM = -I src/
CFLAGS_AD_CAM = ${INC_DIR_AD_CAM}

07_Animal_detection_usbcam_app: ${SRC_AD_CAM}
	${CXX} -std=c++14 $(filter %.cpp,${SRC_AD_CAM}) ${CFLAGS_AD_CAM} \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/animal_detection_usbcam_app

clean:
	rm -rf exe/animal_detection_usbcam_app
