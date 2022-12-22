# RZV2L AI Library - Elderly Fall Detection

## Introduction

Falling while standing is one of the major accident that occurs with elderly people causing severe injuries. Therefore, fall detection systems are becoming increasingly important  at old age homes, hospitals and even normal residence. Fall detection systems enabled with vision based Ai examine human movement and perform activity recognition to detect falling events in input videos from camera.

## Application details

```
|-- Makefile
|-- README.md
|-- etc/ # address map and pre/post process configuration of both hrnet and tiny-yolov2
|-- exe/
|   |-- 03_Fall_detection # The executable
|   |-- hrnet_cam/ # DRP-AI files of hrnet
|   `-- tinyyolov2_cam/ # DRP-AI files of tiny-yolov2
`-- src/ # source code directory
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
- 6 upper body keypoints (Head, sholders etc.) are used to detect the fall. Sudden fall or slower falls, both are detected.
- Fall detection message will be printed on the screen as well as in the console.

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/03_Elderly_fall_detection/
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 03_Elderly_fall_detection/exe/
./03_Elderly_fall_detection <x1> <y1> <x2> <y2> <direction>
```
3. Argument description;
    - <x1> - xmin of line of separation (int)
    - <y1> - ymin of line of separation (int)
    - <x2> - xmax of line of separation (int)
    - <y2> - ymax of line of separation (int)
    - <direction> - 0/1 (to recognize region of fall with respect to line of separation)

### Example:

```
cd /home/root/RZV2L_AiLibrary 
cd 03_Elderly_fall_detection/exe/
./03_Elderly_fall_detection 0 240 640 240 0
```


## Limitations

1. Inference time is around 200 ms. 
2. Full body of the person should be visible in the camera. 
