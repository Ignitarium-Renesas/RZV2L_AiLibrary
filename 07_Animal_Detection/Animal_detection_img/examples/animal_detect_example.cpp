#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_yolov3_img.h"

using namespace std;
using namespace cv;

/* We initialize the demo class here */
class ADDemo
{
    public:
        void start(void);
};

/* This method invokes the API */
void ADDemo::start(void)
{
    /* Initialize the Animal class */
    Animal AD = Animal();

    /* Obtain the relative image path of the input image */
    AD.get_image_path();

    /* read image */
    Mat image = imread(AD.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    /* Get inference */
    AD.PRET_AD(image.data, AD.width, AD.height, AD.animal, AD.alarm);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the demo object*/
    ADDemo demo = ADDemo();

    /* Start the demo */
    demo.start();
    
    return 0;
}
