# RZV2L AI Library - Human Gaze Detection

## Introduction

Gaze Detection is a task to predict where a person is looking at. Human gaze is important information for applications such as Driver Monitor Systems to monitor driver's attentiveness. It can also help to analyse viewer's attention data in case of digital signage and retail industry.

sample video on YouTube -[Gaze detection demo](https://youtu.be/mFzOm5VhIjc)

## Application details

```
├── etc # address map and pre/post process configuration of both deeppose and tiny-yolov2
├── exe
│   ├── deeppose_cam # DRP-AI files of deeppose
│   ├── rf_gaze_dir.xml
│   ├── 09_Gaze_detection # The executable
│   └── tinyyolov2_cam # DRP-AI files of tiny-yolov2
├── Makefile
├── README.md
└── src # source code directory
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

- Camera input is fed to object detector (tiny yolov2) to detect faces in first step.
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
cd RZV2L_AiLibrary/09_Human_Gaze_detection/
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary
cd 09_Human_Gaze_detection/exe/
./09_Human_Gaze_detection
```

### Example:

```
cd /home/root/RZV2L_AiLibrary
cd 09_Human_Gaze_detection/exe/
./09_Human_Gaze_detection
```

## Limitations

1. Identifying the gaze direction by using forest classifier results in limited accuracy.

## Known issues
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
