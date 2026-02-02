XDG_SHELL_LINK = -isystem ${SDKTARGETSYSROOT}/usr/src/debug/gstreamer1.0-plugins-base/1.22.12/gst-libs/gst/gl \
		 -lwayland-cursor -lwayland-egl -lwayland-client
OPENCV_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/opencv4 \
			  -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_video 
MMNGR_LINK =  -isystem ${SDKTARGETSYSROOT}/usr/local/include \
			  -lmmngr -lmmngrbuf
BSP_SDK_FLAG = \
			  -ljpeg -lwebp -ltiff -lz -ltbb -lgtk-3 -lpng16 -lgdk-3 -lcairo  \
			  -llzma -lrt -lcairo-gobject \
			  -lxkbcommon -lepoxy \
			  -lfribidi -lharfbuzz -lfontconfig \
			  -lglib-2.0 -lgobject-2.0 -lgdk_pixbuf-2.0 -lgmodule-2.0 -lpangocairo-1.0 \
			  -latk-1.0 -lgio-2.0 -lpango-1.0 -lfreetype -lpixman-1 -luuid -lpcre \
			  -lmount -lresolv -lexpat -lpangoft2-1.0 -lblkid -lGLESv2 -lEGL \


all: src/sample_app_tinyyolov3_cam.cpp
	${CXX} -std=c++17 src/sample_app_tinyyolov3_cam.cpp src/camera.cpp src/image.cpp src/wayland.cpp src/ascii.cpp src/box.cpp src/kalman_box_tracker.cpp src/kuhn_munkres.cpp src/sort.cpp \
	${XDG_SHELL_LINK} ${OPENCV_LINK} ${MMNGR_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/line_crossing_object_counting_usbcam_app

clean:
	rm -rf *.o exe/line_crossing_object_counting_usbcam_app
