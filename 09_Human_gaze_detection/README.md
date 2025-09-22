# RZV2L AI Library - Human Gaze Detection

## Introduction

Gaze Detection is a task to predict where a person is looking at. Human gaze is important information for applications such as Driver Monitor Systems to monitor driver's attentiveness. It can also help to analyse viewer's attention data in case of digital signage and retail industry.

sample video on YouTube -[Gaze detection demo](https://youtu.be/X_eH5UcThrc)

## Application details

```
|-- 09_Human_Gaze_Detection
|-- Human_Gaze_Detection_cam
|  |-- Makefile
|  |-- README.md
|  |-- exe
|  |  |-- human_gaze_detection_cam_app # The executable
|  |  |-- resnet18_cam # DRP-AI files of resnet-18
|  |  |-- tinyyolov3_cam # DRP-AI files of tinyyolov3
|  |-- src # source code directory
|-- Human_Gaze_Detection_usbcam
|  |-- Makefile
|  |-- README.md
|  |-- exe
|  |  |-- human_gaze_detection_usbcam_app # The executable
|  |  |-- resnet18_cam # DRP-AI files of resnet-18
|  |  |-- tinyyolov3_cam # DRP-AI files of tinyyolov3
|  |-- src # source code directory
```

### Model details

#### ResNet-18

- Model - [pytorch_mpiigaze](https://github.com/hysts/pytorch_mpiigaze_demo/releases/download/v0.2.2/eth-xgaze_resnet18.pth)
- In this application ResNet-18 is used for gaze detection.

#### Tiny Yolov3

- TinyYoloV3 is a state of the art neural network model. It belongs to well known Yolo family of object detectors. It requires less parameters to get  trained properly. 
- official link : [ Official Yolo website](https://pjreddie.com/darknet/yolo/)
- Dataset: [HollywoodHeads](https://www.di.ens.fr/willow/research/headdetection/)
- In this application Tiny Yolov3 is used for detecting faces.

### Application with camera input

- Camera input is fed to object detector (tiny yolov3) to detect faces in first step.
- The detected faces are fed for gaze estimation (resnet-18).

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application once the desired changes in he source code are made:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary
cd RZV2L_AiLibrary/09_Human_gaze_detection/Human_gaze_detection_usbcam/
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary
cd 09_Human_gaze_detection/Human_gaze_detection_usbcam/exe/
./human_gaze_detection_usbcam_app
```

### Example:

```
cd /home/root/RZV2L_AiLibrary
cd 09_Human_gaze_detection/Human_gaze_detection_usbcam/exe/
./human_gaze_detection_usbcam_app
```

## Limitations

## Known issues
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
