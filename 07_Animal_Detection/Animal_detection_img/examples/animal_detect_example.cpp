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
        void start(string img_path, int width, int height);
};

/* This method invokes the API */
void ADDemo::start(string img_path, int width, int height)
{
    /* Initialize the Animal class */
    Animal AD = Animal();

    AD.img_path = img_path;
    AD.width = width;
    AD.height = height;

    /* Obtain animal name and alarm type */
    AD.get_user_input();

    /* read image */
    Mat image = imread(AD.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    if(AD.width != image.size().width ||
       AD.height != image.size().height)
    {
        AD.width = image.size().width;
        AD.height = image.size().height;
    }

    /* Get inference */
    AD.PRET_AD(image.data, AD.width, AD.height, AD.animal, AD.alarm);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    if(argc != 2 && argc != 4)
    {
        printf("Usage :\n");
        printf("\t%s [image_path]\n", argv[0]);
        printf("\t%s [image_path] [width] [height]\n\n", argv[0]);
        printf("Note : width and height are optional\n");
    }

    /* Initialize the demo object*/
    ADDemo demo = ADDemo();

    /* Start the demo */
    if(argc == 2)
    {
        demo.start(argv[1], 0, 0);
    }
    else if(argc == 4)
    {
        demo.start(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}
