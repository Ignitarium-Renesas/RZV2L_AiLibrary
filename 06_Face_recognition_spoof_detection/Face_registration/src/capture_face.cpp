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

static int capture_face(void){

    string file_name;
    cout << "---------------------------" << endl;
    cout << "Capturing from USB Camera" << endl;
    cout << "---------------------------" << endl << endl;
    cout << "Enter the name of the picture" << endl;
    cin >> file_name;
    cout << "---------------------------" << endl;
    cout << "..Press Enter to capture.." << endl;
    cout << "---------------------------" << endl;

    /* create capture dir */
    make_cap_dir();

    /* Create a VideoCapture object. Change the constructor argument based on the video feed (/dev/video1 is being captured below) */
    VideoCapture cap(1); 

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
        imshow("Capture", frame);
    
        /*Press Enter on keyboard to exit*/
        char c = (char)waitKey(1);
        if(c == 13)
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
