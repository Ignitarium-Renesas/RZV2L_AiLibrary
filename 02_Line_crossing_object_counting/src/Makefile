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


all: sample_app_tinyyolov2_cam.cpp
	${CXX} -std=c++14 sample_app_tinyyolov2_cam.cpp camera.cpp image.cpp wayland.cpp ascii.cpp box.cpp centroidtracker.cpp \
	${OPENCV_LINK} ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} -o sample_app_tinyyolov2_cam

clean:
	rm -rf *.o sample_app_tinyyolov2_cam
