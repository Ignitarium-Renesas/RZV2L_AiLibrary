#include "define.h"

using namespace std;
using namespace cv;

static int coral_cam_init(void)
{
    const char* commands[4] =
    {
        "media-ctl -d /dev/media0 -r",
        "media-ctl -d /dev/media0 -V \"\'ov5645 0-003c\':0 [fmt:UYVY8_2X8/640x480 field:none]\"",
        "media-ctl -d /dev/media0 -l \"\'rzg2l_csi2 10830400.csi2\':1 -> \'CRU output\':0 [1]\"",
        "media-ctl -d /dev/media0 -V \"\'rzg2l_csi2 10830400.csi2\':1 [fmt:UYVY8_2X8/640x480 field:none]\""
    };

    /* media-ctl command */
    for (int i=0; i<4; i++)
    {
        int ret = system(commands[i]);
        if (ret<0)
        {
            printf("%s: failed media-ctl commands. index = %d\n", __func__, i);
            return -1;
        }
    }
    return 0;
}

static void make_cap_dir(void)
{
    struct stat sb;

    if (stat(CAPTURE_DIR, &sb) != 0)
        mkdir(CAPTURE_DIR, ACCESSPERMS);

    return;
}

static int capture_face(void){

    string file_name;
    cout << "---------------------------" << endl;
    cout << "Capturing from Coral Camera" << endl;
    cout << "---------------------------" << endl << endl;
    cout << "Enter the name of the picture" << endl;
    cin >> file_name;
    cout << "---------------------------" << endl;
    cout << "..Press Enter to capture.." << endl;
    cout << "---------------------------" << endl;

    /* init Coral camera */
    coral_cam_init();

    /* create capture dir */
    make_cap_dir();

    /* Create a VideoCapture object.*/
    VideoCapture cap(0); 

    /* Check if camera opened successfully */
    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    /* Top Left Coordinates*/
    int thickness = 2;
    Point p1(208-thickness, 128-thickness);
    /* Bottom Right Coordinates*/
    Point p2(432+thickness, 352+thickness);

    while(1){

        Mat frame;
        /*Capture frame-by-frame*/
        cap >> frame;
    
        /* If the frame is empty, break immediately*/
        if (frame.empty())
        break;
    
        /* Display the resulting frame*/
        rectangle(frame, p1, p2, Scalar(255, 0, 0), thickness, LINE_8);
        imshow("Coral Capture", frame);
    
        /*Press Enter on keyboard to exit*/
        char c = (char)waitKey(1);
        if(c == 13)
        {
            /* Save the cropped image */
            Mat cropped_frame = frame(Range(128, 352), Range(208, 432));
            cv::imwrite((string)CAPTURE_DIR+"/"+file_name+".bmp", cropped_frame);
            break;
        }
    }

    /* When everything done, release the video capture object*/
    cap.release();

    /* Closes all the frames */
    destroyAllWindows();

    return 0;
}
