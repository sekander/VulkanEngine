#pragma once

#include <memory>
#include <iostream>
#include <cstdint>



#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <future>

#include <bits/stdc++.h>
#include <cstddef>
#include <ctime>


#include "../GameData/GameData.h"

//States 
#include "../States/splash_state.h"
#include "../States/main_menu_state.h"
#include "../States/game_play_state.h"
#include "../States/game_over_state.h"
#include "../States/loading_play_state.h"
#include "../States/template_state.h"



class Game {

	public:
		Game();
		~Game();

		void Initialize();


		GLFWwindow* create_vulkan_window(std::string w_name = "Nahid Window", 
										   const int width = 1366, 
										   const int height = 768);

		void Run();
		void NetworkUpdate();
		void SoundUpdate();
    void Update();
		void Render();                                                                                  

		void processInput(GLFWwindow *window);

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);   
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	
		static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );
		static void error_callback( int error, const char* description );

		
	
	private:
		// settings
		const double fpsLimit = 1.0 / 60.0;
		double lastRenderTime = 0;  // number of seconds since the last loop
		double lastUpdateTime = 0;  // number of seconds since the last loop
		double lastFrameTime = 0;   // number of seconds since the last frame
		unsigned int renderFrames = 0;

		const unsigned int SCR_WIDTH = 800;
		const unsigned int SCR_HEIGHT = 600;


		bool update_flag 	= false;
		bool keepLooping 	= true;
		bool process_Input_flag = false;
		std::mutex g_Mutex;

		const char *test;
		//Actor *networkActor;
    	GLFWmonitor* monitor;
    	const GLFWvidmode* mode;

		GameData *_data;

		float lastX = 0.0f;
		float lastY = 0.0f;

		bool firstMouse = true;



};

