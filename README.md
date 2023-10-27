# # Renesas_RZV2L_DRP-AI TVM - Sample Applications

## Overview

This Library has API functions for leveraging AI applications that will run on Renesas RZ/V2L Board. Currently this library has following sample applications: 

1. Head Counter
2. Animal Detection
3. Head Count Top View

## Prerequisites
### 1. Board bring up sequence
These steps are required to make the Renesas RZV2L board working.
There are 2 major sections:
1. Set Up AI SDK on the RZ/V2L Evaluation Board.
2. Setup RZ/V2L AI SDK  environment on host Linux PC.

#### 1.Set Up AI SDK on the RZ/V2L Evaluation Board.
Follow [Getting Started ](https://renesas-rz.github.io/rzv_ai_sdk/getting_started.html) from the Renesas Official Website to bring up RZV2L board. Below mentioned steps are from the same document.

#### 2.Setup RZ/V2L AI SDK environment to build application (on host Linux PC).
Follow from Step 5 in [Getting Started ](https://renesas-rz.github.io/rzv_ai_sdk/getting_started.html) to install docker. After following the steps, user should see a docker image `rzv2l_ai_sdk_image` running on host machine.

### Requirements
- A laptop with Ubuntu 20.04 LTS (64 bit) operating system.
- 100GB free space on hard disk is necessary. If this is not available, laptop will freeze during build process.

### Configuring Git
```
git config --global user.email "you@example.com"
git config --global user.name "Your Name"
```
> Note: Copy all files obtained from Renesas into your home directory prior to the steps below.

## Hardware details 
 
These sample applications run on a Renesas RZ/V2L-SMARC board.

<img src="./Renesas_RZV2L_image.jpg?raw=true" alt="Markdown Monster icon"
     margin-right=10px; 
     width=600px;
     height=334px />

## Editing and building sample applications

Please follow the steps available in README files of individual application.
