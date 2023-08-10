#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_animal_detect_cam(void);

/* The Animal detection application class*/
class Animal
{
    public:
        int32_t run_camera_inference(void);

};

/* Run camera inference */
int32_t Animal::run_camera_inference(void)
{
    return get_animal_detect_cam();
}

