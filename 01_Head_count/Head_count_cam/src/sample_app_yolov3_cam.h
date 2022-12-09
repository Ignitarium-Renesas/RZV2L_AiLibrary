#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_head_count_cam(void);

/* The head count application class*/
class HeadCount
{
    public:
        int32_t run_camera_inference(void);

};

/* Run camera inference */
int32_t HeadCount::run_camera_inference(void)
{
    return get_head_count_cam();
}
