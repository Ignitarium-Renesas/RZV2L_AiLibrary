#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_headcount_topview_img.h"

using namespace std;
using namespace cv;

/* We initialize the demo class here */
class HCDemo
{
    public:
        void start(string img_path, int width, int height);
};

/* This method invokes the API */
void HCDemo::start(string img_path, int width, int height)
{
    /* Initialize the HeadCount class */
    HeadCountTop HC = HeadCountTop();

    HC.img_path = img_path;
    HC.width = width;
    HC.height = height;

    /* read image */
    Mat image = imread(HC.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    if(HC.width != image.size().width ||
       HC.height != image.size().height)
    {
        HC.width = image.size().width;
        HC.height = image.size().height;
    }

    /* Get inference */
    HC.PRET_HC(image.data, HC.width, HC.height);
    
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
    HCDemo demo = HCDemo();

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
