# RZV2L AI Library - Safety Helmet Vest Detection application

## Introduction

Safety Helmet Vest Detection application is used to detect the safety helmet and vest present in an image.
It can utilize the API, "*PRED_SHV()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [Safety_helmet_vest_cam/examples/Safety_helmet_vest_cam_example.cpp](04_Safety_helmet_vest_cam/examples/Safety_helmet_vest_cam_example.cpp) for an example usage

sample video on YouTube -[Safety helmet & vest detection demo video](https://youtu.be/GwyEKvzptm0)

## Application details

```
|
|-- Safety_helmet_vest_cam/ # Safety Helmet Vest Detection application on camera
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- Safety_helmet_vest_cam_example.cpp # example inference code on camera
|   |-- exe/
|   |   |-- safety_helmet_vest_cam_app # the executable
|   |   `-- yolov3_cam # DRP-AI files of yolov3
|   `-- src/ # source code directory
|-- Safety_helmet_vest_img # Safety Helmet Vest Detection application on images input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- Safety_helmet_vest_example.cpp #example inference code on image input
|   |-- exe/
|   |   |-- safety_helmet_vest_img_app # the executable
|   |   |-- labels.txt # label map
|   |   `-- yolov3_bmp # DRP-AI files of yolov3
|   |-- src/ # source code directory
|   `-- test_images # test images directory
|-- Safety_helmet_vest_usbcam # Safety Helmet Vest Detection application on usb camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- Safety_helmet_vest_example.cpp #example inference code on usb camera input
|   |-- exe/
|   |   |-- safety_helmet_vest_usbcam_app # the executable
|   |   |-- labels.txt # label map
|   |   `-- yolov3_bmp # DRP-AI files of yolov3
|   `-- src/ # source code directory
`-- README.md
```

## Model details

### Yolov3

A commonly known object detection model, YoloV3 is used in this application as object detector model. official link [YoloV3](https://pjreddie.com/darknet/yolo/).
Yolov3 is trained on a open source dataset.
You can find the [dataset](https://github.com/MinhNKB/helmet-safety-vest-detection) and [Kaggle dataset](https://www.kaggle.com/datasets/muhammetzahitaydn/hardhat-vest-dataset-v3) here.

### Application with image input
- The user should pass the path to the image as an argument, optionally it's width and height.
- The image is then resized to 640x480.
- The count is calculated based on the detected heads.

### Application with camera input
- Camera input is fed to the model with proper pre-processing.
- The detections are calculated based on the detected helmets and vests.

### How to build the sample application

Please follow the below steps once the desired changes are made in the source code:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_<cam/img/usbcam>
make
```

## Running the application
1. Download the `yolov3_Helmet_vest_detection_weight.dat` weights file from the [release v1.2.2](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v1.2.2). Precise file link : [yolov3_Helmet_vest_detection_weight.dat]( https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/v1.2.2/yolov3_Helmet_vest_detection_weight.dat)
2. Place `yolov3_Helmet_vest_detection_weights.dat` file at the location `04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_cam/exe/yolov3/` or `04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_img/exe/yolov3/` or `04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_usbcam/exe/yolov3/` according to to the application that needs to be run.
3. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
4. Please follow these steps:

### Application with camera input

```
cd /home/root/RZV2L_AiLibrary 
cd 04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_<cam/usbcam>/exe
./safety_helmet_vest_<cam/usbcam>_app
```
### Application with image input

The user should pass the path to the image as an argument, optionally it's width and height.

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 04_Safety_Helmet_Vest_Detection/Safety_helmet_vest_img/exe
./safety_helmet_vest_img_app ../test_images/20.jpg
```
### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.
