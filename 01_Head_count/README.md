# RZV2L AI Library - Head counting application

## Introduction

This application is used to count the human heads present in an image.
It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [01_Head_count/Head_count_img/examples/head_count_example.cpp](01_Head_count/Head_count_img/examples/head_count_example.cpp) for an example usage


## Application details

### Model details
add yoloV3 details here. training dataset etc.

### Application with image input
Add details like how image is read, resized, preprocessed, inferred, post processed etc.

### Application with camera input
Add details like how inference happends on video stream. is it the stream or frame?

## Running the application

### Building the sample application:

TODO
Mention that this needs to be done on laptop and not on the board.
SDK steps.
Code changes.

Please follow the below steps for image application:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/01_Head_count/Head_count_img
make
```

Please follow the below steps for camera application:
TODO

### Running the sample application
Praparation on laptop:
1. Download the weights file `yolov3_Head_counting_weights.dat` file from the release.
2. Place this file at location `01_Head_count/Head_count_img/exe/yolov3_bmp/` or `01_Head_count/Head_count_cam/exe/yolov3_cam/`
3. Now, copy the `RZV2L_AiLibrary` directory to the RZV2L board (/home/root/).
4. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/<path to sample application>
./sample_application
```
3. Enter the relative path for the test_image with respect to the directory of the executable when prompted

### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/
./01_head_count_app
# When prompted for the image path
./test_images/Ben_001_640_480.bmp
```


## Limitations
TODO:
enter the limitations. constraints etc.

