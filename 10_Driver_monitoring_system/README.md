# RZV2L AI Library - Driver Monitoring System

## Introduction

Driver Monitoring System application focuses at predicting the status of driver by determing if the driver is distracted or drowsy. Various states of driver predicted are looking top, down, left, right, blink and yawn.


sample video on YouTube -[Driver Monitoring System demo](https://youtu.be/LKe9k9XYWJY)
ToDo : Add image of camera setup.

## Application detailsl

```
|-- Driver_Monitoring_System_cam # Application with CORAL camera input
|   |-- Makefile
|   |-- etc # address map and pre/post process configuration of both deeppose and tiny-yolov2
|   |-- exe
|   |   |-- rf_gaze_dir.xml 
|   |   |-- driver_monitoring_system_cam_app # The executable
|   |   |-- deeppose_cam # DRP-AI files of deeppose
|   |   `-- tinyyolov2_cam # DRP-AI files of tiny-yolov2
|   `-- src # source code directory
|-- Driver_Monitoring_System_usbcam # Application with USB camera input
|   |-- Makefile
|   |-- etc # address map and pre/post process configuration of both deeppose and tiny-yolov2
|   |-- exe
|   |   |-- rf_gaze_dir.xml 
|   |   |-- driver_monitoring_system_usbcam_app # The executable
|   |   |-- deeppose_cam # DRP-AI files of deeppose
|   |   `-- tinyyolov2_cam # DRP-AI files of tiny-yolov2
|   `-- src # source code directory
`-- README.md
```

### Model details

#### DeepPose

- Model - [MMPose Facial Landmark Localization](https://mmpose.readthedocs.io/en/latest/topics/face.html#deeppose-resnet-on-wflw)
- Dataset - [WFLW](https://wywu.github.io/projects/LAB/WFLW.html)
- In this application DeepPose is used for detecting face landmarks.

#### Tiny Yolov2

- Official paper - [YOLO9000: Better, Faster, Stronger](https://arxiv.org/pdf/1612.08242.pdf)
- Dataset - [WIDERFACE](http://shuoyang1213.me/WIDERFACE/)
- In this application Tiny Yolov2 is used for detecting faces.

### Application with camera input

- Camera input is fed to object detector (tiny yolov2) to detect faces.
- Face landmarks are extracted from the detected faces.
- The face landmarks are used to classify the gaze directions (`CENTER`, `UP`, `DOWN`, `LEFT` & `RIGHT`) using a Random Forest algorithm.

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application once the desired changes in he source code are made:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary
cd RZV2L_AiLibrary/10_Driver_monitoring_system/Driver_Monitoring_System_<usbcam/cam>
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary
cd 10_Driver_monitoring_system/Driver_Monitoring_System_<usbcam/cam>/exe/
./driver_monitoring_system_<usbcam/cam>_app
```

### Example:

```
cd /home/root/RZV2L_AiLibrary
cd 10_Driver_monitoring_system/Driver_Monitoring_System_usbcam/exe/
./driver_monitoring_system_usbcam_app
```

## Limitations

1. Face keypoints based random forest classifier is not accurate enough.

## Known issues
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
