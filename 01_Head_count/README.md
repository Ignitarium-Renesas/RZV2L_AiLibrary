# RZV2L AI Library - Head counting application

#### Human Head Counter

This application is used to count the human heads present in an image.
It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [01_Head_count/Head_count_img/examples/head_count_example.cpp](01_Head_count/Head_count_img/examples/head_count_example.cpp) for an example usage

## Work in progress... 

## How to build sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/01_Head_count/Head_count_img
make
```

## How to run sample application

1. Download the weights file `01_head_count_weights.zip` file from the release.
2. Extract it. Place `yolov3_bmp_weight.dat` file at location `01_Head_count/Head_count_img/exe/yolov3_bmp/`
3. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
4. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/<path to sample application>
./sample_application
```
3. Enter the relative path for the test_image with respect to the directory of the executable when prompted

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/
./01_head_count_app
# When prompted for the image path
./test_images/Ben_001_640_480.bmp
```
