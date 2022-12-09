#include <string>
#include <iostream>
#include "sample_app_yolov3_cam.h"

using namespace std;


int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the head count object*/
    HeadCount HC = HeadCount();

    /* Start the camera inference */
    HC.run_camera_inference();
    
    return 0;
}
