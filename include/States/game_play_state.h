#pragma once

#include <glm/fwd.hpp>

#include "state.h"
#include "../GameData/GameData.h"


enum _entityCategory {
    ALL        =    0x0001,
    PLAYER     =    0x0002,
    NPC        =    0x0004,
    FLOOR      = 		0x0008,
    BULLETS    = 		0x0016,
    PLATFORM   = 		0x0032,
	  GUN 	     = 		0x0064,		
	  MAINPALYER = 		0x0128		
};

/*
enum Levels
{
    LEVEL_1 = 0,
    LEVEL_2, 
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,
    LEVEL_6,
    LEVEL_7,
    LEVEL_8,
    LEVEL_9,
    LEVEL_0
};
*/


class GamePlayState : public State {

    public :
        //GamePlayState(GameDataRef data );
        //GamePlayState(GameDataRef& data );
        GamePlayState(GameData& data );


        void Init();
        void Input(float delta);
        void Update(float delta);
        void Render(float delta);
        void SoundUpdate();
        void DeleteData();
        ~GamePlayState();
    private :

	//GameDataRef _data;
	GameData* _data;
        float x, y;
		float x_cam, y_cam, z_cam, r_cam;

	double delay;
	double jump_delay;
	float counter;

	uint16_t platform_delay = 0;
	bool reachedPos = false;
	bool prevPos = true;


    //Levels level;



int x_pos;
int y_pos;

float xpos_o = 0.0f;
float ypos_o = 0.0f;
float xpos_d = 0.0f;
float ypos_d = 0.0f;

bool bullet_collides = false;
bool ncp_collides = false;

float gun_angle = 0.0f;


bool shooting_left = false;
bool shooting_right = true;


bool game_paused;
glm::uint8_t pause_counter;


float cam_x_speed;
float cam_y_speed;


//Game play clock
double game_play_clock;
double record_game_play_clock;

//Waves
int round;
float gp_fade = 0.0f;

//srand(unsigned) time(NULL);

};
