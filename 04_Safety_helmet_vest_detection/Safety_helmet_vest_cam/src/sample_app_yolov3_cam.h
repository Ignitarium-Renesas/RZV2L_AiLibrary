#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t Safety_helmet_vest_cam(void);

/* The Safety application class*/
class Safety
{
    public:
        int32_t run_camera_inference(void);
};

/* Run Safety inference */
int32_t Safety::run_camera_inference(void)
{
    return Safety_helmet_vest_cam();
}
