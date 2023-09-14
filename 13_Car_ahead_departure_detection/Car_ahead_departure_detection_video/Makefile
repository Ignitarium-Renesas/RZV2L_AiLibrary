OPENCV_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/opencv4 \
			  -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_videoio
GST_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/gstreamer-1.0 \
			-lgstapp-1.0 -lgstpbutils-1.0 -lgstvideo-1.0
GLIB_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/glib-2.0
GSTAPP_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/gstreamer-app-1.0

GST_LIBS=`pkg-config --libs gstreamer-1.0`
GST_CFLAGS=`pkg-config --cflags gstreamer-1.0`

ALSA_LINK = -isystem ${SDKTARGETSYSROOT}/usr/include/alsa \
	-lasound

BSP_SDK_FLAG = \
			  -ljpeg -lwebp -ltiff -lz -ltbb -lgtk-3 -lpng16 -lgdk-3 -lcairo  \
			  -llzma -lrt -lcairo-gobject \
			  -lxkbcommon -lwayland-cursor -lwayland-egl -lwayland-client -lepoxy \
			  -lfribidi -lharfbuzz -lfontconfig \
			  -lglib-2.0 -lgobject-2.0 -lgdk_pixbuf-2.0 -lgmodule-2.0 -lpangocairo-1.0 \
			  -latk-1.0 -lgio-2.0 -lpango-1.0 -lfreetype -lpixman-1 -luuid -lpcre \
			  -lmount -lresolv -lexpat -lpangoft2-1.0 -lblkid \


all: src/sample_app_tinyyolov3_vid.cpp 
	${CXX} -std=c++14 src/sample_app_tinyyolov3_vid.cpp src/box.cpp src/kalman_filter.cpp src/matrix.cpp src/munkres.cpp src/track.cpp src/tracker.cpp \
	${OPENCV_LINK} ${GST_LINK} ${GLIB_LINK} ${GSTAPP_LINK} ${ALSA_LINK} $(GST_CFLAGS) $(GST_LIBS) ${BSP_SDK_FLAG} \
	-lpthread -O2 -ldl ${LDFLAGS} -o exe/car_ahead_departure_detection_video_app

clean:
	rm -rf *.o exe/car_ahead_departure_detection_video_app
