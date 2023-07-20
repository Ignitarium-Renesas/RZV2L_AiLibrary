all: 04_Safety_helmet_vest_cam \

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
SRC_HC_CAM = examples/Safety_helmet_vest_cam_example.cpp \
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
		
INC_DIR_HC_CAM = -I src/
CFLAGS_HC_CAM = ${INC_DIR_HC_CAM}

04_Safety_helmet_vest_cam: ${SRC_HC_CAM}
	${CXX} -std=c++14 $(filter %.cpp,${SRC_HC_CAM}) ${CFLAGS_HC_CAM} \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} \
	-o exe/safety_helmet_vest_usbcam_app

clean:
	rm -rf exe/safety_helmet_vest_usbcam_app
