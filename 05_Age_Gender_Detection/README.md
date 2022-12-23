# RZV2L AI Library - Age and Gender detection
# WORK IN PROGRESS. LIMITED ACCURACY AVAILABLE CURRENTLY.

## Introduction

This particular application showcases the capability of deep neural networks to predict age and detect the gender of a person.
This is a work in progress application. Only image based gender recognition code is pushed as of Dec 2022 last week. More updates will be available during January 2023.

## Application details

### How to build the sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/05_Age_Gender_Detection/05_Gender_classifier_img
make
```

## Running the application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:


### Application with image input


Enter the relative path for the test_image with respect to the directory of the executable when prompted

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/05_Gender_classifier_img/exe
./05_gender_img_app
# When prompted for the image path
./sample.bmp
```
