# RZV2L AI Library - Animal detection application

## Introduction

This application is used to detect specific set of animals in a given image or camera input.

| Classes | Animal |
| :---: | :---: |
| 1 | Boar |
| 2 | Deer |
| 3 | Crow |
| 4 | Monkey |
| 5 | Bear |
| 6 | Racoon |
| 7 | Fox |
| 8 | Weasal |
| 9 | Skunk |
| 10 | Dog |
| 11 | Cat |


It can utilize the API, "PRET_AD()" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. The camera application utilizes the API "get_camera_inference" to initiate the inference process.
> Refer [Animal_detection_img/examples/animal_detection_example.cpp](animal_detection_img/examples/animal_detection_example.cpp) for an example usage

sample video on YouTube -[Animal Detection demo](https://youtu.be/o-3oeOCRHws)

## Application details
```
|-- 07_Animal_detection.mp4 # sample demo
|-- Animal_detection_cam/ # Animal detection application on images
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- animal_detection_cam_example.cpp # example inference code on image
|   |-- exe/
|   |   |-- 07_animal_detection_cam_app # the executable
|   |   `-- animal_yolov3_cam # DRP-AI files of yolov3
|   `-- src/ # source code directory
|-- Animal_detection_img # Animal detection application on camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- animal_detection_example.cpp #example inference code on camera input
|   |-- exe/
|   |   |-- 07_animal_detection_img_app # the executable
|   |   |-- labels.txt # label map
|   |   `-- animal_yolov3_bmp # DRP-AI files of yolov3
|   |-- src/ # source code directory
|-- Animal_detection_usbcam/ # Animal detection application on images
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- animal_detection_usbcam_example.cpp # example inference code on image
|   |-- exe/
|   |   |-- 07_animal_detection_usbcam_app # the executable
|   |   `-- animal_yolov3_cam # DRP-AI files of yolov3
|   `-- src/ # source code directory
|   `-- test_images # test images directory
`-- README.md
```

### Model details

#### YOLOv3
In this application YOLOv3 is used for detecting animals.
- Official paper - [YOLOv3: An Incremental Improvement](https://arxiv.org/pdf/1804.02767.pdf)
- Official website - [YoloV3](https://github.com/AlexeyAB/darknet)
- Datasets :  

| classes | Animals | Dataset |
| --- | --- | :---: |
|<p> 1 <p> 2 <p> 3 <p> 4 <p> 5 <p> 6 |<p> Fox <p> Deer <p> Crow <p> Monkey <p> Bear <p> Raccoon | [Animals Detection Images Dataset](https://www.kaggle.com/datasets/antoreepjana/animals-detection-images-dataset) |
|<p> 7 <p> 8 <p> 9 |<p> Boar <p> Weasal <p> Skunk | [Images.cv](https://images.cv) |
|<p> 10 <p> 11 |<p> Dog <p> Cat | [Coco Dataset](https://cocodataset.org/#download) |

### Application with image input
- The user should pass the path to the image as an argument, optionally it's width and height.
- The specified set of animal(s) in the image is(are) detected and confidence value of each animal class is displayed on terminal.

### Application with camera input
- Camera input is fed to the model with proper pre-processing.
- The specified set of animal(s) in the image is(are) detected and confidence value of each animal class is displayed on terminal.

### Building the sample application:

This is an optional step since already compiled application is available in the `exe/` folder.
These steps must be performed in Laptop/PC with the support of RZV2L toolchain. Find the steps in the repository's Readme file available at home page.

After doing desired modifications in the source code, please follow the below steps to create the executable: 

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/07_Animal_detection/<app dir>
make
```

### Running the sample application
Praparation on laptop:
1. Download the weights file `animal_yolov3_weight.dat` or `animal_yolov3_weight_img.dat` from the release [release v1.2.3](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v1.2.3) depending on which application is being used.Precise file links: [animal_yolov3_weight.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/v1.2.3/animal_yolov3_weight.dat) and [animal_yolov3_weight_img.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/v1.2.3/animal_yolov3_weight_img.dat)
2. Place the weight files at location proper location `07_Animal_detection/Animal_detection_img/exe/yolov3_bmp/` or `07_Animal_detection/Animal_detection_cam/exe/yolov3_cam/`
3. Now, copy the `RZV2L_AiLibrary` directory to the RZV2L board (/home/root/).
4. Please follow these steps:
For image input continue with steps 5 to 7.
5. The user should pass the path to the image as an argument, optionally it's width and height.
6. Now enter the animal name(Note: case sensitive) to be detected from the list displayed on terminal.
7. Enter the corresponding alarm threshold for that animal in the closed range[0,1].

#### Application with image input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/Animal_detection_img/exe/
root@smarc-rzv2l:~/RZV2L_AiLibrary/07_Animal_detection/Animal_detection_img/exe# ./07_animal_detect_img_app ../test_images/raccoon.bmp 
----------------------------------------
Running Animal detection demo
----------------------------------------

Enter animal to detect from the list provided below:

Boar
Deer
Crow
Monkey
Bear
Raccoon
Fox
Weasel
Skunk
Dog
Cat
----------------------------------------

Raccoon

Enter alarm threshold in closed range[0,1]: 0.5
RZ/V2L DRP-AI Sample Application
Model : Darknet YOLO      | animal_yolov3
Loading : animal_yolov3/drp_desc.bin
Loading : animal_yolov3/animal_yolov3_drpcfg.mem
Loading : animal_yolov3/drp_param.bin
Loading : animal_yolov3/aimac_desc.bin
Loading : animal_yolov3/animal_yolov3_weight.dat
Inference -----------------------------------------------
[START] DRP-AI
[END] DRP-AI
DRP-AI Time: 290.367432 msec
Result 4 -----------------------------------------*
Class           : Raccoon
(X, Y, W, H)    : (50, 107, 33, 38)
Probability     : 82.0 %

Result 20 -----------------------------------------*
Class           : Raccoon
(X, Y, W, H)    : (394, 474, 482, 2060)
Probability     : 72.4 %

Result 21 -----------------------------------------*
Class           : Raccoon
(X, Y, W, H)    : (492, 456, 354, 756)
Probability     : 56.9 %

```

#### Application with coral camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/Animal_detection_cam/exe/
./07_animal_detection_cam_app
```

#### Application with usb camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/Animal_detection_usbcam/exe/
./07_animal_detection_usbcam_app
```


### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.

