#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <memory>

#include "../States/state_machine.h"
#include "../Camera/Camera.h"
//#include "../../include/Loaders/LoadingGameObjects.h"

///#include "../Render/VulkanRenderer.h"

enum States
{
	SPLASH_STATE 					= 0,
	MAIN_MENU_STATE 				= 1,
	LOADING_PLAY_STATE 				= 2,
	PLAY_STATE 						= 3,
	GAME_OVER_STATE 				= 4,
	CURRENT 						= 5,
	TEMPLATE_STATE = 6
};

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

struct Player_Data
{
	float x_pos;
	float y_pos;
	int playerHealth;
	int totalMedkit;
	int totalEmergencykit;
	int totalAntidotes;
	int playerArmor;
	int totalAddrenaline;
	int totalRepairKit;
	int totalFortify;
	int totalTraps;


	//int x_pos;
	//int y_pos;

	std::string player_data;
	std::string x_pos_text;
	std::string y_pos_text;


	uint8_t dpadD;
	uint8_t dpadU;
	uint8_t dpadL;
	uint8_t dpadR;

	uint8_t A;
	uint8_t B;
	uint8_t X;
	uint8_t Y;

	uint8_t L;
	uint8_t R;
	float L2;
	float R2;

	float LJOYX;
	float LJOYY;
	
	float RJOYX;
	float RJOYY;

	uint8_t start;
	uint8_t select;


	uint8_t touchPad_1;
	uint8_t touchPad_2;
	uint8_t touchPad_3;
	uint8_t touchPad_4;
	uint8_t touchPad_5;
	uint8_t touchPad_6;
	uint8_t touchPad_7;
	uint8_t touchPad_8;
	uint8_t touchPad_9;
	uint8_t touchPad_10;
	uint8_t touchPad_11;
	uint8_t touchPad_12;
	uint8_t touchPad_13;
};

struct Game_Window
{

};

struct Game_Setting
{
	bool exit_app;
	bool server_listening;
	bool server_on;
	bool searching_for_connection;
	bool connection_established;
	bool sending_test_packet;
	
	bool start_game;
	bool fullScreen;
	bool windowMode;


	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
};






struct GameData 
{
	GLFWwindow* 			window;
	StateMachine* 			machine;
	//LoadingGameObjects*		ld;
	States					state_switch;
	Levels					level;
	Player_Data				pd;
	Game_Setting 			gs;
	Camera*					cm;
	void*					vk;
	//VulkanRenderer			vk;


	//Move Constructor
	GameData(){
		std::cout << "Default Constructor" << std::endl;
	}

	GameData(const GameData& gameData) : window(gameData.window), machine(gameData.machine)
	//GameData(const GameData& gameData) : window(gameData.window), o_cam(gameData.o_cam), world(gameData.world)
	{
		std::cout << "Copy Constructor" << std::endl;
	}

	GameData(GameData&& moveData) noexcept
	{
		std::cout << "Move Constructor" << std::endl;
		window = moveData.window;
		machine = moveData.machine;
		//ld    = moveData.ld;
		//state_switch = moveData.state_switch;

		moveData.window = nullptr;
		moveData.machine = nullptr;
		//moveData.ld    = nullptr;
		//moveData.state_switch = nullptr;
	}

	~GameData()
	{
		std::cout << "GameData Destroyed" << std::endl;
	}
	
	public:



};

using GameDataRef = std::shared_ptr<GameData>;

