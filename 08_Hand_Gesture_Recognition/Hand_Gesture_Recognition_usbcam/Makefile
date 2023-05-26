OPENCV_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/opencv4 \
              -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_ml

OPENCV_LINK_TRAIN = -isystem ${SDKTARGETSYSROOT}/usr/include/opencv4 \
              -lopencv_core -lopencv_highgui -lopencv_ml

BSP_SDK_FLAG1 = \
			  -ljpeg -lwebp -ltiff -lz -ltbb -lgtk-3 -lpng16 -lgdk-3 -lcairo  \
			  -llzma -lrt -lcairo-gobject \
			  -lxkbcommon -lwayland-cursor -lwayland-egl -lwayland-client -lepoxy \
			  -lfribidi -lharfbuzz -lfontconfig \
			  -lglib-2.0 -lgobject-2.0 -lgdk_pixbuf-2.0 -lgmodule-2.0 -lpangocairo-1.0 \
			  -latk-1.0 -lgio-2.0 -lpango-1.0 -lfreetype -lpixman-1 -luuid -lpcre \
			  -lmount -lresolv -lexpat -lpangoft2-1.0 -lblkid \


all: src/sample_gesture_prediction.cpp
	${CXX} -std=c++14 src/sample_gesture_prediction.cpp src/camera.cpp src/image.cpp src/wayland.cpp \
	${OPENCV_LINK} ${BSP_SDK_FLAG1} \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/hand_gesture_recognition_usbcam_app



clean:
	rm -rf exe/hand_gesture_recognition_usbcam_app
