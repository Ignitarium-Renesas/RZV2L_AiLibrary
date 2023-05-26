# RZV2L AI Library - Age and Gender detection with Camera mode

## Introduction

This application showcases the capability of deep neural networks to predict age-group and detect the gender of a person.
Camera based age and gender classification application is available now with better usability. 

sample video on YouTube -[Age and gender detection demo video](https://youtu.be/azJW8TZ2-Hg)

## Age&Gender Application details
```
├── 05_Age_Gender_Detection
├── Age_Gender_Classification_cam
|   ├── etc # address map and pre/post process configuration of both age_gender and tiny-yolov2
    ├── exe
│       ├── fairface_cam # DRP-AI files of age_gender
│       ├── Age_gender_classification_cam_app # The executable
│       └── tinyyolov2_cam # DRP-AI files of tiny-yolov2
|   ├── Makefile
|   ├── README.md
    └── src # source code directory
├── Age_Gender_Classification_usbcam
|   ├── etc # address map and pre/post process configuration of both age_gender and tiny-yolov2
    ├── exe
│       ├── fairface_cam # DRP-AI files of age_gender
│       ├── Age_gender_classification_usbcam_app # The executable
│       └── tinyyolov2_cam # DRP-AI files of tiny-yolov2
|   ├── Makefile
|   ├── README.md
    └── src # source code directory
├── Age_Gender_Classification_img
|   ├── etc # address map and pre/post process configuration of both age_gender and tiny-yolov2
    ├── exe
│       ├── fairface_cam # DRP-AI files of age_gender
│       ├── Age_gender_classification_img_app # The executable
│       └── tinyyolov2_cam # DRP-AI files of tiny-yolov2
|   ├── Makefile
|   ├── README.md
    └── src # source code directory
```

### Application with camera input


1)Camera input is fed to object detector (tiny yolov2) to detect faces.
2)Facial features are extracted from the detected faces. 
3)The facial features are used to determine the age & gender of the person.

## Running the application
### How to build the sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/05_Age_Gender_Detection/Age_Gender_Classification_usbcam
make
```

### Running the sample application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/Age_Gender_Classification_usbcam/exe
./Age_gender_classification_usbcam_app
```

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/Age_Gender_Classification_usbcam/exe
./Age_gender_classification_usbcam_app
```
## Limitation
1. The classification is dependent on face detected by tiny yolov2.
2. The age-group classification appears to lose its accuracy in camera application.

### Known issues:
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
