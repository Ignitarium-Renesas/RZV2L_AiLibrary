# RZV2L AI Library - Safety Helmet Vest Detection application

## Introduction

Safety Helmet Vest Detection application is used to detect the safety helmet and vest present in an image.
It can utilize the API, "*PRED_SHV()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [Safety_helmet_vest_cam/examples/Safety_helmet_vest_cam_example.cpp](04_Safety_helmet_vest_cam/examples/Safety_helmet_vest_cam_example.cpp) for an example usage

## Application details

### Model details
A commonly known object detection model, YoloV3 is used in this application as object detector model. official link [YoloV3](https://pjreddie.com/darknet/yolo/).
Yolov3 is trained on a open source dataset.
You can find the [dataset](https://github.com/MinhNKB/helmet-safety-vest-detection) and [Kaggle dataset](https://www.kaggle.com/datasets/andrewmvd/hard-hat-detection) here.

### How to build the sample application

Please follow the below steps once the desired changes are made in the source code:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_cam 
make
```

## Running the application
1. Download the `yolov3_Helmet_vest_detection_weights.dat` weights file from the release v1.2.2. 
2. Place `yolov3_Helmet_vest_detection_weights.dat` file at the location `04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_cam/exe/yolov3/` or `04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_img/exe/yolov3/` according to to the application that needs to be run.
3. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
4. Please follow these steps:

### Application with camera input

```
cd /home/root/RZV2L_AiLibrary 
cd 04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_cam/exe
./04_Safety_helmet_vest_cam_app
```
### Application with image input

Enter the relative path for the test_image with respect to the directory of the executable when prompted

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_img/exe
./04_Safety_helmet_vest_img
# When prompted for the image path
./test_images/6.jpg
```
