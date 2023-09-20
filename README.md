# RZV2L AI Library - Sample Applications

## Overview

This Library has API functions for leveraging AI applications that will run on Renesas RZ/V2L Board. Currently this library has following sample applications: 

1. Human Head Counter
2. Line Crossing Object Counter
3. Elderly People Fall Detection
4. Safety Helmet and Vest Detection
5. Human Age and Gender Detection
6. Face Recognition and Spoof Detection
7. Animal Detection
8. Hand Gesture Recognition
9. Human Gaze Recognition
10. Driver Monitoring System
11. Head Count Top View
12. Hand Gesture Recognition V2
13. Car Ahead Departure Detection

## Prerequisites
### 1. Board bring up sequence
These steps are required to make the Renesas RZV2L board working.
There are 2 major sections:
1. Building the image and preparing the micro SD card.
2. Board setup

#### 1.Building the image and preparing the micro SD card.
Follow **R01US0556EJ0102** from the Renesas Official Website to bring up RZV2L board. Below mentioned steps are from the same document.

##### Requirements
- A laptop with Ubuntu 20.04 LTS (64 bit) operating system.
- 100GB free space on hard disk is necessary. If this is not available, laptop will freeze during build process.

##### Build Instructions
Install dependencies:

```
sudo apt-get update
sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib \
build-essential chrpath socat cpio python python3 python3-pip python3-pexpect \
xz-utils debianutils iputils-ping libsdl1.2-dev xterm p7zip-full libyaml-dev
```

##### Configuring Git
```
git config --global user.email "you@example.com"
git config --global user.name "Your Name"
```
> Note: Copy all files obtained from Renesas into your home directory prior to the steps below.

##### Create a working directory at your home directory, and decompress Yocto recipe package
```
mkdir ~/rzv_vlp_v3.0.0
cd ~/rzv_vlp_v3.0.0
unzip ~/RTK0EF0045Z0024AZJ-v3.0.0-update2.zip
tar zxvf ./RTK0EF0045Z0024AZJ-v3.0.0-update2/rzv_bsp_v3.0.0.tar.gz
```

##### Enable Graphics and Video Codec
Please copy the Graphics package (RTK0EF0045Z13001ZJ-v1.xx_EN.zip or RTK0EF0045Z13001ZJ-v1.xx_JP.zip) to working directory and run the commands below.
```
cd ~/rzv_vlp_v3.0.0
unzip ~/RTK0EF0045Z13001ZJ-v1.xx_EN.zip
tar zxvf ./RTK0EF0045Z13001ZJ-v1.xx_EN/meta-rz-features.tar.gz
```

Please copy the video codec package (RTK0EF0045Z15001ZJ-v0.xx_EN.zip or RTK0EF0045Z15001ZJ-v0.xx_JP.zip) to working directory and run the commands below.
```
cd ~/rzv_vlp_v3.0.0
unzip ~/RTK0EF0045Z15001ZJ-v0.xx_EN.zip
tar zxvf ./RTK0EF0045Z15001ZJ-v0.xx_EN/meta-rz-features.tar.gz
```

##### Unzip the DRP-AI Support Package Recipe
```
cd $WORK
tar -zxvf ./rzv2l_drpai-driver/meta-rz-features.tar.gz
```

##### Setup a build environment
```
cd ~/rzv_vlp_v3.0.0
source poky/oe-init-build-env
```

##### Prepare the default configuration files for the target board
```
cd ~/rzv_vlp_v3.0.0/build
cp ../meta-renesas/docs/template/conf/smarc-rzv2l/*.conf
```

##### Expected directory structure

```
|- WORK
   |- build
   |- meta-gplv2
   |- meta-openembedded
   |- meta-qt5
   |- meta-renesas
   |- meta-rz-features
      |- conf
      |- include
      |- recipes-drpai
      |- recipes-graphics
   |- meta-virtualization
   |- poky
   |- rzv2l_drpai-driver
      |- meta-rz-features.tar.gz
```

##### Build
```
cd $WORK/build
bitbake core-image-weston
```

After the Build, following files will be generated under $WORK/build/tmp/deploy/images/smarc-rzv2l

```
Image-smarc-rzv2l.bin
Image-r9a07g054l2-smarc.dtb
core-image-weston-smarc-rzv2l.tar.bz2
```
#### 2.Board setup
Carefully follow instructions from the document R01US0556EJ0102. 

### 2. SDK installation steps
The directory structure should look like this:

```
|- WORK
   |- build
   |- meta-gplv2
   |- meta-openembedded
   |- meta-qt5
   |- meta-renesas
   |- meta-rz-features
      |- conf
      |- include
      |- recipes-drpai
      |- recipes-graphics
   |- meta-virtualization
   |- poky
   |- rzv2l_drpai-driver
      |- meta-rz-features.tar.gz
```

#### Install ISP Support package

Unzip the ISP support package. 
```
cd $WORK
$ unzip ~/r11an0561ej0120-rzv2l-isp-sp.zip
$ tar zxvf ./r11an0561ej0120-rzv2l-isp-sp/meta-rz-features.tar.gz
```
Now, the directory structure should look like this:

```
|- WORK
   |- build
   |- meta-gplv2
   |- meta-openembedded
   |- meta-qt5
   |- meta-renesas
   |- meta-rz-features
      |- conf
      |- include
      |- recipes-drpai
      |- recipes-graphics
      |- recipes-isp
   |- meta-virtualization
   |- poky
   |- rzv2l_drpai-driver
      |- meta-rz-features.tar.gz
```
Set up build environment for ISP Support :

```
cd $WORK
source poky/oe-init-build-env
cp ../meta-renesas/docs/template/conf/smarc-rzv2l/* conf/
```

Build the image with  bitbake

```
cd $WORK/build
bitbake core-image-weston
```

Now, Build the SDK.

Before executing following commands, apply all necessary recipes required for the Compile
Environment.

The directory structure looks like this: 

```
|- WORK
   |- build
   |- meta-gplv2
   |- meta-openembedded
   |- meta-qt5
   |- meta-renesas
   |- meta-rz-features
      |- conf
      |- include
      |- recipes-drpai
      |- recipes-graphics
      |- recipes-isp
   |- meta-virtualization
   |- poky
   |- rzv2l_drpai-driver
      |- meta-rz-features.tar.gz
```

Populate the sdk with bitbake. Use following command:

```
cd $WORK
source poky/oe-init-build-env
bitbake core-image-weston -c populate_sdk
```

After the Build process is completed, following files will be generated under **$WORK/build/tmp/deploy/sdk**.

Filename:- **poky-glibc-x86_64-core-image-weston-aarch64-smarc-rzv2l-toolchain-
3.1.14.sh**

#### Install SDK
Install the SDK using this command:
```
cd $WORK/build
./tmp/deploy/sdk/poky-glibc-x86_64-core-image-weston-aarch64-smarc-rzv2l-toolchain-3.1.14.sh
```

In the Installer, specify the SDK installation directory and press the enter key.

Example below installs the SDK to default directory, “/opt/poky/3.1.14”.

Note:
“/opt/poky/3.1.14” will be replaced with your SDK installed path.

Confirm that following directories and files are generated under the specified directory.

```
|-opt
     |-poky
          |-3.1.14
               |- sysroots
               |- environment-setup-aarch64-poky-linux
               |- environment-setup-armv7vet2hf-neon-vfpv4-pokymllib32-linux-gnueabi
               |- site-config-aarch64-poky-linux
               |- site-config-armv7vet2hf-neon-vfpv4-pokymllib32-linux-gnueabi
               |- version-aarch64-poky-linux
               |- version-armv7vet2hf-neon-vfpv4-pokymllib32-linux-gnueabi
```

To use the SDK run the bellow command

```
source /opt/poky/3.1.14/environment-setup-aarch64-poky-linux
```

Once the source is done, it can be used as SDK for the RZV2L board.

#### Sample command to use SDK and build the executable application

~~~
source /opt/poky/3.1.14/environment-setup-aarch64-poky-linux
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary
cd RZV2L_AiLibrary/<application_name>
make
~~~

Output of the make i.e executable will be stored int the **exe** folder.

## Hardware details 
 
These sample applications run on a Renesas RZ/V2L-SMARC board.

<img src="./Renesas_RZV2L_image.jpg?raw=true" alt="Markdown Monster icon"
     margin-right=10px; 
     width=600px;
     height=334px />


## Software requirements

1. Install RZ/V2L Linux Standard Development Kit (SDK). For more details refer [Renesas website](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzv2l-general-purpose-microprocessor-equipped-renesas-original-ai-accelerator-drp-ai-12ghz-dual#overview). For SDK build, please refer [here](https://github.com/renesas-rz/meta-rzv)
2. The installation and compilation of the sample application was tested on a laptop with Ubuntu 20.04 operating system.

## Editing and building sample applications

Please follow the steps available in README files of individual application.
