/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : sample_app_arcface_img.cpp
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Sample Application for Image version
***********************************************************************************************************************/

#include "define.h"

using namespace std;
using namespace cv;

static void make_cap_dir(void)
{
    struct stat sb;

    if (stat(CAPTURE_DIR, &sb) != 0)
        mkdir(CAPTURE_DIR, ACCESSPERMS);

    return;
}

#ifdef INPUT_CORAL
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
#endif /* INPUT_CORAL */

static int capture_face(void){

    string file_name;
    cout << "---------------------------" << endl;
    cout << "Capturing from USB Camera" << endl;
    cout << "---------------------------" << endl << endl;
    cout << "Enter the name of the picture" << endl;
    cin >> file_name;
    cout << "---------------------------" << endl;
    cout << "..Enter c (c+Enter) to capture.." << endl;
    cout << "---------------------------" << endl;

#ifdef INPUT_CORAL
    // init Coral camera
    coral_cam_init();
#endif /* INPUT_CORAL */

    /*Set Standard Input to Non Blocking*/
    errno = 0;
    int8_t ret = fcntl(0, F_SETFL, O_NONBLOCK);
    if (-1 == ret)
    {
        fprintf(stderr, "[ERROR] Failed to run fctnl(): errno=%d\n", errno);
        return -1;
    }


    /* create capture dir */
    make_cap_dir();

    /* Create a VideoCapture object. Change the constructor argument based on the video feed (/dev/video0 is being captured below) */
    VideoCapture cap(0);

    /* Check if camera opened successfully */
    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    /* Draw Detection Area*/
    Point center(640/2, 480/2);
    int bbox_radius = 112;
    int thickness = 2;

    while(1){

        Mat frame;
        /*Capture frame-by-frame*/
        cap >> frame;
    
        /* If the frame is empty, break immediately*/
        if (frame.empty())
        break;
    
        /* Display the resulting frame*/
        circle(frame, center, bbox_radius, cv::Scalar(0, 0, 255, 255), thickness);
        imshow("Capture", frame);
    
        waitKey(1);

        /*Press c+Enter on keyboard to exit*/
        int32_t c = getchar();
        if (99 == c)
        {
            /* Save the cropped image */
            Mat resized_frame;
            Mat cropped_frame = frame(Range(128, 352), Range(208, 432));
            resize(cropped_frame, resized_frame, Size(112, 112), INTER_LINEAR);
            cv::imwrite((string)CAPTURE_DIR+"/"+file_name+".bmp", resized_frame);
            break;
        }
    }

    /* When everything done, release the video capture object*/
    cap.release();

    /* Closes all the frames */
    destroyAllWindows();

    return 0;
}
