#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_animal_detect(uint8_t* in_img_addr, uint32_t width, uint32_t height, string animal, float alarm);

/* The animal detection application class*/
class Animal
{
    public:
        string img_path,animal;
        const string animal_list[11] = { "Boar", "Deer", "Crow", "Monkey", "Bear", "Raccoon", "Fox", "Weasel", "Skunk", "Dog", "Cat" };
        uint32_t width, height;
        float alarm;
        void get_user_input(void);
        int32_t PRET_AD(uint8_t* in_img_addr, uint32_t width, uint32_t height, string animal, float alarm);

};

/* This method is used to get the input image relative path with 
respect to the executable directory and the animal to detect with its corresponding alram threshold*/
void Animal::get_user_input(void)
{
    cout << "----------------------------------------" << endl;
    cout << "Running Animal detection demo" << endl;
    cout << "----------------------------------------" << endl << endl;

    cout << "Enter animal to detect from the list provided below:\n" << endl;
    for (int i = 0; i < 11; i++)
      cout << animal_list[i] << "\n";
    cout << "----------------------------------------" << endl << endl;
    cin >> animal;
    bool found = false;
    for (int i = 0; i < 11; ++i)
    {
    	if (animal_list[i] == animal)
		found = true;
    }
     while(!(found))
    {
        cout << "----------------------------------------" << endl << endl;
        cout << "Incorrect animal name" << endl;
        cout << "Enter animal to detect from the list provided below:" << endl;
        for (int i = 0; i < 11; i++)
            cout << animal_list[i] << "\n";
        cout <<"Animal to detect:";
        cin >> animal;
        for (int i = 0; i < 11; ++i)
    	{
		if (animal_list[i] == animal)
			found = true;
	}
    }
    
    cout << "\nEnter alarm threshold in closed range[0,1]: ";
    cin >> alarm;
    while(alarm<0 || alarm>1)
    {
        cout << "----------------------------------------" << endl << endl;
        cout << "Alarm threshold not in range[0,1] \nAnimal to detect:" << animal;
        cout << "\nEnter threshold in closed range[0,1]: ";
        cin >> alarm;
    }
}

/* This method wraps the get_animal_detect function which is declared above. 
This function will be exposed to the user */
int32_t Animal::PRET_AD(uint8_t* in_img_addr, uint32_t width, uint32_t height, string animal, float alarm)
{
    return get_animal_detect(in_img_addr, width, height, animal, alarm);
}
