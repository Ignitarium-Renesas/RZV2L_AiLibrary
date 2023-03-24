#include <string>
#include <iostream>
#include "sample_app_yolov3_cam.h"

using namespace std;


int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the Animal detection object*/
    Animal AD = Animal();

    /* Start the camera inference */
    AD.run_camera_inference();
    
    return 0;
}

