# RZV2L AI Library - Age and Gender detection with image

## Introduction

This application showcases the capability of deep neural networks to predict age-group and detect the gender of a person.
Camera based age and gender classification application is available now with better usability. 

sample video on YouTube -[Age and gender detection demo video](https://youtu.be/-DpAGb7q4pM)

## Age&Gender Application details
```
├── etc # address map and pre/post process configuration of both fair face
├── exe
│   ├── fairface_bmp # DRP-AI files of fairface
│   ├── 05_age_gender_img_app # The executable
│   └── sample.bmp #Sample image for testing
├── Makefile
├── README.md
|── src # source code directory
|── test_files #test images
```

### Application with image input


1)Enter the relative path for the test_image with respect to the directory of the executable when prompted
2)Along with the test_image path pass the test_image dimension in the order width X height 
3)For sample test images the image dimensions are provided in the 'test_images_dimensions.txt'

## Running the application
### How to build the sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/05_Age_Gender_Detection/05_Age_Gender_Classifier_img
make
```

### Running the sample application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/05_Age_Gender_Classifier_img/exe
./05_age_gender_img_app
```

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/05_Age_Gender_Classifier_img/exe
./05_age_gender_img_app
```
## Limitation
1. The classification is dependent on face detected by tiny yolov2.
2. The age-group classification appears to lose its accuracy in camera application.

### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.