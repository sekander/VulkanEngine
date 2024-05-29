//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------
#include "../../include/GameData/GameData.h"
#include "../../include/States/splash_state.h"


#include <algorithm>
#include <cstdio>



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;




double last_time = glfwGetTime();

unsigned int frame_x;

using namespace std;

float splash_fadein_wave;
bool  splash_pause_fade;

SplashState::SplashState(GameData& data)
{


	_data = std::move(&data);

  printf("Game Play State!\n");


	x_cam = 0.0f;
	y_cam = 0.0f;
	z_cam = 0.0f;
	r_cam = 0.0f;


	splash_pause_fade = false;
}


SplashState::~SplashState()
{
    printf("Deleting Game Play State\n");
//    DeleteData();
}

void SplashState::DeleteData()
{
//    delete _data.get()->ld;
    printf("Deleting Game Play State\n");
}

void SplashState::Init()
{

    printf("Hello Game Play\n");
	

}

void SplashState::SoundUpdate()
{
    //printf("Hello Splah State Sound UPdate\n");
}


void SplashState::Input(float delta)
{
    //printf("Main Menu Input \n");
    if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
	    x_cam -=5;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
	    x_cam +=5;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
	    y_cam +=5;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
	    y_cam -=5;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_E) == GLFW_PRESS)
    {
	    r_cam +=0.01;
    }

    if (glfwGetKey(_data->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
	    r_cam -=0.01;
    }

    if (glfwGetKey(_data->window, GLFW_KEY_Z) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
    {
    }

}

void SplashState::Update(float delta)
{
}

void SplashState::Render(float delta)
{
    if(glfwGetTime() > 7.5)
		_data->state_switch = MAIN_MENU_STATE;
		

}
