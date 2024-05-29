///-------------------------------ALUT--------------------------------------
#include <AL/al.h>
#include <AL/alut.h>
//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------

/////////////////////////////////////////////////////////////////////////////
///------------------------------IMGUI--------------------------------------
#include "../../include/IMGUI/imgui.h"
#include "../../include/IMGUI/imgui_impl_glfw.h"
#include "../../include/IMGUI/imgui_impl_opengl3.h" 

//////////////////////////////////////////////////////////////////////////////
///------------------------Actors---------------------------------



#include "../../include/GameData/GameData.h"
#include "../../include/States/game_play_state.h"
#include <GLFW/glfw3.h>
#include <box2d/b2_math.h>
#include <complex>
#include <cstddef>
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <iostream>
#include <cstdlib>

#include "../../include/GameData/ObjectCounter.h"

//#define MAX_BOXES 1
#define MAX_BOXES 250
//#define MAX_BOXES_TEST 1
#define MAX_BOXES_TEST 200 

#define MAX_ENEMIES 1000




// settings
const uint16_t SCR_WIDTH = 800;
const uint16_t SCR_HEIGHT = 600;


//####################################################################################
//Organize all this 

//MY SHADERS




//std::vector<std::shared_ptr<Projectile_Actor>> boxes;



double game_state_last_time = glfwGetTime();
int i = 0;

double game_play_last_time = glfwGetTime();



int startingPoint = 0;
int enemiesKilled = 0;

int score;

float player_R, player_G, player_B;


GamePlayState::GamePlayState(GameData &data) : _data(std::move(&data)) 
{

	  	_data->gs.start_game = true;
}

int uframes = 0;

GamePlayState::~GamePlayState()
{
    printf("Deleting GAME PLAY State\n");
}

void GamePlayState::DeleteData()
{
    printf("Deleting GAME PLAY State\n");


}

float gameplay_BackgroundMusic = 0.5f;
float gameplay_SoundFX = 0.5f;

void GamePlayState::Init()
{
    printf("Hello GAME PLAY Init************************************************************************\n");

//###########################################################################################################################
    //x_cam = -5.0f;
    x_cam = 135.0f;
    y_cam = 230.0f;
	z_cam = 0.0f;
	r_cam = 0.0f;

	pause_counter = 0;
	
	cam_x_speed = 0.0f;
	cam_y_speed = 0.0f;

	//glfwSetTime(0.0);
	game_play_clock = 0;
	record_game_play_clock = 0;
	round = 0;
	score = 0;
	
	x_pos = 0.0f;
	y_pos = 0.0f;
	game_paused = false;
	counter = 0.0f;	

	player_R = 1.0f;
	player_G = 1.0f;
	player_B = 1.0f;

}

bool gun_shooting = false;

void GamePlayState::SoundUpdate()
{
}

void GamePlayState::Input(float delta)
{
	/*
		//Move camera Left
    	if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS
		//	|| _data->pd.RJOYX > 0.0f
		)
    	{
		    //x_cam -=5.0f;
			//std::cout << "X cam : " << x_cam  << std::endl;
    	}

		//Move camera Right
    	if (glfwGetKey(_data->window, GLFW_KEY_LEFT) == GLFW_PRESS
		//	|| _data->pd.RJOYX < 0.0f
		)
    	{
		    //x_cam +=5.0f;
			//std::cout << "X cam : " << x_cam  << std::endl;
    	}

		//Move camera UP
    	if (glfwGetKey(_data->window, GLFW_KEY_UP) == GLFW_PRESS
		//	|| _data->pd.RJOYY < 0.0f
		)
    	{
		    y_cam +=5.0f;
		    //std::cout << "Y cam : " << y_cam  << std::endl;
    	}

		//Move camera Down
    	if (glfwGetKey(_data->window, GLFW_KEY_DOWN) == GLFW_PRESS
		//	|| _data->pd.RJOYY > 0.0f
		)
    	{
		    y_cam -=5.0f;
			//std::cout << "Y cam : " << y_cam  << std::endl;
    	}

		//Rotate camera right
    	if (glfwGetKey(_data->window, GLFW_KEY_E) == GLFW_PRESS)
    	{
		    //r_cam +=0.001;
			//std::cout << "R cam : " << r_cam  << std::endl;
    	}

		//Rotate camera left
    	if (glfwGetKey(_data->window, GLFW_KEY_Q) == GLFW_PRESS)
    	{
		    //r_cam -=0.001;
			//std::cout << "R cam : " << r_cam  << std::endl;
    	}

		//Camera Zoom in
    	if (glfwGetKey(_data->window, GLFW_KEY_Z) == GLFW_PRESS
			|| _data->pd.LJOYY > 0.0f) 
    	{
		   //z_scale.x += 0.1;
		   //z_scale.y += 0.1;
    	   //std::cout << "Z-x cam : " << z_scale.x << std::endl;
    	   //std::cout << "Z-y cam : " << z_scale.y << std::endl;
    	}

		//Camera Zoom out
    	if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
			//|| _data->pd.LJOYY < 0.0f) 
    	{
		   z_scale.x -= 0.1;
		    z_scale.y -= 0.1;
    	    //std::cout << "Z-x cam : " << z_scale.x << std::endl;
    	    //std::cout << "Z-y cam : " << z_scale.y << std::endl;
    	}
	*/
		//std::cout << "X cam : " << x_cam  << std::endl;
		//std::cout << "Y cam : " << y_cam  << std::endl;
		//Move camera Left
    	if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS
		//	|| _data->pd.RJOYX > 0.0f
		)
    	{
		    x_cam -=5.0f;
			std::cout << "X cam : " << x_cam  << std::endl;
    	}

		//Move camera Right
    	if (glfwGetKey(_data->window, GLFW_KEY_LEFT) == GLFW_PRESS
		//	|| _data->pd.RJOYX < 0.0f
		)
    	{
		    x_cam +=5.0f;
			std::cout << "X cam : " << x_cam  << std::endl;
    	}

		//Move camera UP
    	if (glfwGetKey(_data->window, GLFW_KEY_UP) == GLFW_PRESS
		//	|| _data->pd.RJOYY < 0.0f
		)
    	{
		    y_cam +=5.0f;
		    //std::cout << "Y cam : " << y_cam  << std::endl;
    	}

		//Move camera Down
    	if (glfwGetKey(_data->window, GLFW_KEY_DOWN) == GLFW_PRESS
		//	|| _data->pd.RJOYY > 0.0f
		)
    	{
		    y_cam -=5.0f;
			//std::cout << "Y cam : " << y_cam  << std::endl;
    	}

		//Camera Zoom in
    	if (glfwGetKey(_data->window, GLFW_KEY_Z) == GLFW_PRESS)
//			|| _data->pd.LJOYY > 0.0f) 
    	{
    	   //std::cout << "Z-x cam : " << z_scale.x << std::endl;
    	   //std::cout << "Z-y cam : " << z_scale.y << std::endl;
    	}

		//Camera Zoom out
    	if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
//			//|| _data->pd.LJOYY < 0.0f) 
    	{
    	    //std::cout << "Z-x cam : " << z_scale.x << std::endl;
    	    //std::cout << "Z-y cam : " << z_scale.y << std::endl;
    	}

	if (glfwGetKey(_data->window, GLFW_KEY_L) == GLFW_PRESS || _data->pd.R == 1)
	{}

	if(_data->pd.touchPad_3 == 1)
		_data->state_switch = MAIN_MENU_STATE;

	if (glfwGetKey(_data->window, GLFW_KEY_P) == GLFW_PRESS || _data->pd.touchPad_1 == 1)
	{
		pause_counter++;
	}
/*	
	if (glfwGetKey(_data->window, GLFW_KEY_U) == GLFW_PRESS || _data->pd.touchPad_5 == 1)
	{
		pause_counter++;
		if(pause_counter > 0)
			game_paused = true;
		else if(pause_counter > 1)
		{
			game_paused = false;
			pause_counter = 0;
		}

		game_paused = false;
	
	}
	*/




}


double captureHealTime;
double captureEmergencyHealTime;
double captureAdrenalineTime;
double roundIncrement = 10;

double pauseCaputureTime;

void GamePlayState::Update(float delta)
{
    
}

void GamePlayState::Render(float delta)
{
}
