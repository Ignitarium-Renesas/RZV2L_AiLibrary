#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_safety_helmet_vest(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The Safety Helmet Vest application class*/
class SafetyHelmetVest
{
    public:
        string img_path;
        uint32_t width, height;
        int32_t PRED_SHV(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method wraps the get_safety_helmet_vest function which is declared above. 
This function will be exposed to the user */
int32_t SafetyHelmetVest::PRED_SHV(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return get_safety_helmet_vest(in_img_addr, width, height);
}
