# RZV2L AI Library - Elderly Fall Detection

## Introduction

Falling while standing is one of the major accident that occurs with elderly people causing severe injuries. Therefore, fall detection systems are becoming increasingly important  at old age homes, hospitals and even normal residence. Fall detection systems enabled with vision based Ai examine human movement and perform activity recognition to detect falling events in input videos from camera.

sample video on YouTube - https://youtu.be/kXZFmK02VQk

## Application details

```
├── Elderly_fall_detection_cam # Elderly fall detection application on CORAL camera input
│   ├── etc/ # address map and pre/post process configuration of both hrnet and tiny-yolov2
│   ├── exe/
│   │   ├── Elderly_fall_detection_cam_app # The executable
│   │   ├── hrnet_cam/ # DRP-AI files of hrnet
│   │   └── tinyyolov2_cam/ # DRP-AI files of tiny-yolov2
│   ├── Makefile
│   ├── src # source code directory
|   └── README.md
└── Elderly_fall_detection_usbcam # Elderly fall detection application on USB camera input
│    ├── etc # address map and pre/post process configuration of both hrnet and tiny-yolov2
│    ├── exe
│    │   ├── Elderly_fall_detection_usbcam_app # The executable
│    │   ├── hrnet_cam/ # DRP-AI files of hrnet
│    │   └── tinyyolov2_cam/ # DRP-AI files of tiny-yolov2
│    ├── Makefile
│    ├── src # source code directory
|    └── README.md
```

### Model details

#### HRNet

- Official paper - [Deep High-Resolution Representation Learning for Visual Recognition](https://arxiv.org/pdf/1908.07919.pdf)
- Dataset - [COCO](https://cocodataset.org/#home)
- In this application HRNet is used for detecting human pose.

#### Tiny Yolov2

- Official paper - [YOLO9000: Better, Faster, Stronger](https://arxiv.org/pdf/1612.08242.pdf)
- Dataset - [PASCAL VOC](http://host.robots.ox.ac.uk/pascal/VOC/)
- In this application Tiny Yolov2 is used for detecting person.

> Note: No transfer learning on custom data was performed for this application.

### Application with camera input

- The user has to input the co-ordinates of the line differentiating the bed or ground and normal standing position. This is required depending on camera's mounting height.
- Camera input is fed to object detector (tiny yolov2) to detect multiple people.
- Pose estimation is done on the detected people. 
- Keypoints & Bounding box ratios are used to detect the fall. Sudden fall or slower falls, both are detected.
- Fall detection message will be printed on the screen.

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application once the desired changes in he source code are made:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/03_Elderly_fall_detection/Elderly_fall_detection_cam
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 03_Elderly_fall_detection/Elderly_fall_detection_cam/exe/
./Elderly_fall_detection_cam_app
```

### Example:

```
cd /home/root/RZV2L_AiLibrary 
cd 03_Elderly_fall_detection/Elderly_fall_detection_cam/exe/
./Elderly_fall_detection_cam_app
```

## Limitations

1. Full body of the person should be visible in the camera. This is the limitation from HR-Net keypoints detector. 

## Known issues
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.  
 
