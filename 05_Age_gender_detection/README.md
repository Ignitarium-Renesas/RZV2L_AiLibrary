# RZV2L AI Library - Age and Gender detection with Camera mode

## Introduction

This application showcases the capability of deep neural networks to predict age-group and detect the gender of a person.
Camera based age and gender classification application is available now with better usability. 

sample video on YouTube -[Age and gender detection demo video](https://youtu.be/mJlwSfazT7I)

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
### Model details

#### Tiny Yolov2

- Official paper - [YOLO9000: Better, Faster, Stronger](https://arxiv.org/pdf/1612.08242.pdf)
- Dataset - [WIDERFACE](http://shuoyang1213.me/WIDERFACE/)
- In this application Tiny Yolov2 is used for detecting faces.

#### FairFace model
- FairFace model is used to recognise the gender and age of a person. 
- Pretrained model is obtained from this GitHub : https://github.com/dchen236/FairFace

### Application with image input

1)The user should pass the path to the image as an argument, optionally it's width and height.
2)Facial features are extracted from the detected faces.
3)The facial features are used to determine the age & gender of the person.

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
cd RZV2L_AiLibrary/05_Age_Gender_Detection/Age_Gender_Classification_<img/cam/usbcam>
make
```

### Running the sample application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:

#### Running the camera application
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/Age_Gender_Classification_<cam/usbcam>/exe
./Age_gender_classification_<cam/usbcam>_app
```

#### Running the image application

The user should pass the path to the image as an argument, optionally it's width and height.

```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/Age_Gender_Classification_img/exe
./Age_gender_classification_img_app ../test_files/21-40_02.bmp
```

## Limitation
1. The classification is dependent on face detected by tiny yolov2.
2. The age-group classification appears to lose its accuracy in camera application.

### Known issues:
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
