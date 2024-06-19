#include "../../include/Window/game.h"
#include <GLFW/glfw3.h>
#include <future>
#include <memory>
#include <thread>
#include <cstddef>


#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


using namespace std::chrono_literals;
using namespace std;


#include "../../include/Render/VulkanRenderer.h"

// Rotation and Mouse
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


//GLFWwindow* window;
VulkanRenderer vk_render;
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));





Game::Game(){
    
  _data = new GameData();

  _data->vk = &vk_render;
    
	_data->machine =  new StateMachine();

}
Game::~Game(){
  cout << "Deleting GAME !!!!!!!" << endl;

  // ImGui_ImplOpenGL3_Shutdown();
  // ImGui_ImplGlfw_Shutdown();
  // ImGui::DestroyContext();

}

void Game::Initialize()
{
  _data->window = create_vulkan_window();
  
  //Instiate Camera
  _data->cm = new Camera();

	  // _data->machine->AddState(St(new MainMenuState(*_data)), false);	
	  _data->machine->AddState(St(new TemplateState(*_data)), false);	
//	  _data->machine->AddState(St(new GameOverState(*_data)), false);	
//	    _data->machine->AddState(St(new SplashState(*_data)), false);	
//	  _data->machine->AddState(St(new LoadingPlayState(*_data)), false);	

  	//Create Vulkan Renderere Instance
	  if(vk_render.init(_data->window) == EXIT_FAILURE)
	  {
		  //return 0;
      
      glfwSetWindowShouldClose(_data->window, true);
	  }
    vk_render.initUI();



	  _data->state_switch = CURRENT;
	  _data->level = LEVEL_1;
    _data->gs.fullScreen = false;
    _data->gs.windowMode = false;
   
   // networkActor->AddComponent<NetworkComponent>(nullptr);
	  update_flag = false;
	  _data->machine->GetActiveState()->Init();
    std::cout << "Done Initializing ... "  << std::endl;
	  update_flag 	   = true;
      
      
      
      
      // Setup Dear ImGui context
      // IMGUI_CHECKVERSION();
      // ImGui::CreateContext();
      // ImGuiIO &io = ImGui::GetIO();
      // Setup Platform/Renderer bindings
      /*
      ImGui_ImplGlfw_InitForOpenGL(_data->window, true);
      ImGui_ImplOpenGL3_Init("#version 330 core");

      // Setup Dear ImGui style
      ImGui::StyleColorsDark();
      ImGuiStyle& style = ImGui::GetStyle();
     

      ImVec4* colors = ImGui::GetStyle().Colors;
      colors[ImGuiCol_Text]                   = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
      colors[ImGuiCol_TextDisabled]           = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
      colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
      colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
      colors[ImGuiCol_Border]                 = ImVec4(0.75f, 0.00f, 0.00f, 0.50f);
      colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.50f, 0.00f);
      colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
      colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.37f, 0.14f, 0.14f, 0.67f);
      colors[ImGuiCol_FrameBgActive]          = ImVec4(0.39f, 0.20f, 0.20f, 0.67f);
      colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
      colors[ImGuiCol_TitleBgActive]          = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
      colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
      colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
      colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
      colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
      colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
      colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
      colors[ImGuiCol_CheckMark]              = ImVec4(0.56f, 0.10f, 0.10f, 1.00f);
      colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
      colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
      colors[ImGuiCol_Button]                 = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
      colors[ImGuiCol_ButtonHovered]          = ImVec4(0.80f, 0.17f, 0.00f, 1.00f);
      colors[ImGuiCol_ButtonActive]           = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
      colors[ImGuiCol_Header]                 = ImVec4(0.33f, 0.35f, 0.36f, 0.53f);
      colors[ImGuiCol_HeaderHovered]          = ImVec4(0.76f, 0.28f, 0.44f, 0.67f);
      colors[ImGuiCol_HeaderActive]           = ImVec4(0.47f, 0.47f, 0.47f, 0.67f);
      colors[ImGuiCol_Separator]              = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
      colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
      colors[ImGuiCol_SeparatorActive]        = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
      colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
      colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
      colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
      colors[ImGuiCol_Tab]                    = ImVec4(0.07f, 0.07f, 0.07f, 0.51f);
      colors[ImGuiCol_TabHovered]             = ImVec4(0.86f, 0.23f, 0.43f, 0.67f);
      colors[ImGuiCol_TabActive]              = ImVec4(0.19f, 0.19f, 0.19f, 0.57f);
      colors[ImGuiCol_TabUnfocused]           = ImVec4(0.05f, 0.05f, 0.05f, 0.90f);
      colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.13f, 0.13f, 0.74f);
      colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
      colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
      colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
      colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
      colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
      colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
      colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
      colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
      colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
      colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
      colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
      colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

      //io.Fonts->AddFontFromFileTTF("assets/font/Minecraft.ttf", 24.0f);
      io.Fonts->AddFontFromFileTTF("assets/font/Antonio-Bold.ttf", 24.0f);
      */


	    //x_cam = 0.0f;
	    //y_cam = -5.0f;
	    //z_cam = 3.0f;


	    //vk_render.createMeshModel("Mario.obj", "mario_main.png");
}


void Game::Run()
{
 // std::thread t([this]( ) {
  std::future<void> iotaFuture = std::async(std::launch::async,
  [this](){
    SoundUpdate();
  });
 
  //std::thread t1([this]( ) {
  std::future<void> iotaFuture2 = std::async(std::launch::async,
  [this](){
    Update();
  });
  
  std::future<void> iotaFuture3 = std::async(std::launch::async,
  [this](){
    NetworkUpdate();
  });


  iotaFuture.get();
  iotaFuture2.get();
  iotaFuture3.get();
  //t.join();
  //t1.join();

  Render();
}

void Game::NetworkUpdate()
{
    std::cout << "Begin Network Loop" << std::endl;
}



void Game::SoundUpdate()
{
    std::cout << "Begin Audio Loop" << std::endl;
    while(keepLooping)
    {
      if(update_flag)
      {
        //std::lock_guard<std::mutex> mtx(g_Mutex);
        _data->machine->GetActiveState()->SoundUpdate();
      }  
    }
}

void Game::Update()
{
    std::cout << "Begin Update Loop" << std::endl;
  
            clock_t start, end;
	          start = clock();
	          unsigned int updateFrame = 0;
 
              //write_to_json();
              std::this_thread::sleep_for(50ms);
              while(keepLooping )
              {
                double now = glfwGetTime();
                double deltaTime = now - lastUpdateTime;

                processInput( _data->window);
                
                if(update_flag)
                {	
                  std::lock_guard<std::mutex> mtx(g_Mutex);

                  // This if-statement only executes once every 60th of a second
                      if ((now - lastFrameTime) >= fpsLimit)
                      {
                      _data->machine->GetActiveState()->Input(0.0f);
                      _data->machine->GetActiveState()->Update(0.0f);
                          
                        updateFrame++;
                    // only set lastFrameTime when you actually draw something
                        lastFrameTime = now;
                  }	
                  // set lastUpdateTime every iteration
                      lastUpdateTime = now;
                }
              }
               
              end = clock();
              double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
              std::cout << "UPDATE THREAD : Time taken by program is :" << std::fixed
                  << time_taken << std::setprecision(5);
              std::cout << " sec" <<std::endl;
              //std::cout << "UPDATE THREAD : Frames per sec " << updateFrame/time_taken << std::endl;
              std::cout << "UPDATE THREAD : Frames per sec " << updateFrame/glfwGetTime() << std::endl;
              //_data->gs.server_listening = false;
              //_data->gs.server_on = false;

}

void Game::Render()
{
    std::cout << "Begin Render Loop" << std::endl;
	  clock_t start, end;
	  start = clock();
    // render loop
    // -----------
    std::cout << "Beginning Render Loop" << std::endl;
    while (!glfwWindowShouldClose(_data->window))
    //while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        double deltaTime = now - lastRenderTime;

		// This if-statement only executes once every 60th of a second
        if ((now - lastFrameTime) >= fpsLimit)
        {
        	if(_data->state_switch == SPLASH_STATE)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
            vk_render.clearScreen();
        		_data->machine->GetActiveState()->DeleteData();
        		_data->machine->AddState(St(new SplashState(*_data)), true);
        		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            	update_flag = true;
				    _data->state_switch = CURRENT;
        	}
        	if(_data->state_switch == MAIN_MENU_STATE)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
            vk_render.clearScreen();
         		_data->machine->GetActiveState()->DeleteData();
         		_data->machine->AddState(St(new MainMenuState(*_data)), true);
         		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            update_flag = true;
				    _data->state_switch = CURRENT;
        	}
        	if(_data->state_switch == LOADING_PLAY_STATE)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
        		_data->machine->GetActiveState()->DeleteData();
        		_data->machine->AddState(St(new LoadingPlayState(*_data)), true);
        		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            update_flag = true;
				    _data->state_switch = CURRENT;
        	}
        	if(_data->state_switch == PLAY_STATE)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
        		_data->machine->GetActiveState()->DeleteData();
        		_data->machine->AddState(St(new GamePlayState(*_data)), true);
        		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            update_flag = true;
				    _data->state_switch = CURRENT;
        	}
        	if(_data->state_switch == GAME_OVER_STATE)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
        		_data->machine->GetActiveState()->DeleteData();
        		_data->machine->AddState(St(new GameOverState(*_data)), true);
        		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            update_flag = true;
				    _data->state_switch = CURRENT;
        	}
        	
          if(_data->state_switch == 6)
        	{
            update_flag = false;
            vk_render.clearScreen();
    			  std::this_thread::sleep_for(50ms);
        		_data->machine->GetActiveState()->DeleteData();
        		_data->machine->AddState(St(new TemplateState(*_data)), true);
        		_data->machine->ProcessState();
        		_data->machine->GetActiveState()->Init();
            update_flag = true;
				    _data->state_switch = CURRENT;
        	}


        // render
        // ------
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
			  _data->machine->GetActiveState()->Render(0.0f);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(_data->window);
        glfwPollEvents();
		// only set lastFrameTime when you actually draw something
        lastFrameTime = now;
        renderFrames++;
		}
		// set lastUpdateTime every iteration
        lastRenderTime = now;

      // glfwSetWindowShouldClose(_data->window, true);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------


    update_flag = false;
    keepLooping = false;
	

    std::this_thread::sleep_for(50ms);

		_data->gs.connection_established = false;
    vk_render.cleanup();
    _data->machine->GetActiveState()->DeleteData();
    //_data->gs.connection_established = false;

    _data->machine->Clear();
    delete _data->machine;
    // vk_render.cleanup();
    delete _data; 

    glfwTerminate();

    end = clock();
	  double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
	  std::cout << "Time taken by program is :" << std::fixed
		  << time_taken << std::setprecision(5);
	  std::cout << " sec" <<std::endl;

	  //std::cout << "Frames per sec " << renderFrames/time_taken << std::endl;
	  std::cout << "Frames per sec " << renderFrames/glfwGetTime() << std::endl;
		//server_listening = false;
    _data->gs.server_listening = false;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
    _data->state_switch = SPLASH_STATE;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			{
				_data->state_switch = MAIN_MENU_STATE;
			}
		    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			{
				_data->state_switch = LOADING_PLAY_STATE;
			}
        /*
		    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			  {
				  _data->state_switch = PLAY_STATE;
			  }
        */

			  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
			  {
				  _data->state_switch = GAME_OVER_STATE;
			  }
			  
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
			  {
				  _data->state_switch = TEMPLATE_STATE;
			  }

			  if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
			  {
				  _data->machine->GetActiveState()->DeleteData();
			  }

			// if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS || _data->gs.fullScreen )
			// //if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
      // {
      //   printf("Enter Full Screen\n");      
      //   _data->gs.windowMode = false;
      //   glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      //   _data->gs.fullScreen = false;
      // }
      // else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS || _data->gs.windowMode)
      // //if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
      // {
      //   printf("Enter Window Screen\n");      
      //   _data->gs.fullScreen = false;
      //   glfwSetWindowMonitor(window, NULL, 0, 0, SCR_WIDTH, SCR_HEIGHT, 0);
      //   _data->gs.windowMode = false;
      // }


}

//error handling function
void error_callback( int error, const char* description )
{
    fprintf( stderr, "Error: %s\n", description );
}

void Game::key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GLFW_TRUE );
    }
    if( key == GLFW_KEY_F && action == GLFW_PRESS )
    {
      printf("Enter Full Screen\n");      
      glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1366, 768, NULL);
    }
    if( key == GLFW_KEY_G && action == GLFW_PRESS )
    {
      printf("Enter Full Screen\n");      
      glfwSetWindowMonitor(window, NULL, 0, 0, 800, 600, 0);
    }



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that widthand 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    printf("Window Width: %d", width);
    printf("Window Height: %d", height);
}



// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Game::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  /*
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
  */
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}


GLFWwindow* Game::create_vulkan_window(std::string w_name, const int width, const int height)
{

	//Initialzie glfw
	glfwInit();

	//Set GLFW to NOT work with openGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

 	//Window Mode
  GLFWwindow* window = glfwCreateWindow(1000, 800, w_name.c_str(), NULL, NULL);
    
    //Full Screen Mode   
    // window = glfwCreateWindow(width, height, w_name.c_str(), glfwGetPrimaryMonitor(),NULL);
  //Window 
    // window = glfwCreateWindow(800, 600, w_name.c_str(), glfwGetPrimaryMonitor(),NULL);
    


	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);

    //Mouse Calls
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetWindowPos(window, 0, 0);
    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  return window;
}