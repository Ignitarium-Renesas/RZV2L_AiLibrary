# RZV2L AI Library

This Library has API functions for leveraging AI applications that will run on Rz/V2L Board. Currently this library will support following applications: 

1. Human Head Counter. 
2. Object Line Counter (TODO)

#### Human Head Counter

This application is used to count the human heads present in an image.
It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [examples/01_head_count/head_count_example.cpp](examples/01_head_count/head_count_example.cpp) for an example usage


#### Object Line Counter (TODO)

This application is used to count the number of objects passed the specified line.


 ## Prerequisites
 ### Hardware 
 
 1. Inorder to run any sample application, you need a Rz/V2L-SMARC board ( whose Power supply changed from discrete to dedicated PMIC) 

<img src="./Renesas_RZV2L_image.jpg?raw=true" alt="Markdown Monster icon"
     margin-right=10px; 
     width=600px;
     height=334px />


### Software

1. Install RZ/V2L Linux Standard Development Kit (SDK). For more details refer [Renesas website](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzv2l-general-purpose-microprocessor-equipped-renesas-original-ai-accelerator-drp-ai-12ghz-dual#overview). For SDK build, please refer [here](https://github.com/renesas-rz/meta-rzv)
2. The installation and compilation of the sample application was tested on Ubuntu 20.04. 

 
## How to build sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary 
make
```

## How to run sample application

1. Download the weights file `01_head_count_weights.zip` file from the release.
2. Extract it. Place `yolov3_bmp_weight.dat` file at location `exe/01_head_count/yolov3_bmp/`
3. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
4. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd exe/<path to sample application>
./sample_application
```
3. Enter the relative path for the test_image with respect to the directory of the executable when prompted

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd exe/01_head_count
./01_head_count_app
# When prompted for the image path
../../test_images/01_head_count/eagle_office.bmp
```
