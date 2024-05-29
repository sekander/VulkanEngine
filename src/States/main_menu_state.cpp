/////////////////////////////////////////////////////////////////////////////
///-------------------------------ALUT--------------------------------------
#include <AL/al.h>
#include <AL/alut.h>
/////////////////////////////////////////////////////////////////////////////
///------------------------------IMGUI--------------------------------------
#include "../../include/IMGUI/imgui.h"
#include "../../include/IMGUI/imgui_impl_glfw.h"
#include "../../include/IMGUI/imgui_impl_opengl3.h" 
//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------

#include "../../include/GameData/GameData.h"
#include "../../include/States/main_menu_state.h"

#include "../../include/GameData/ObjectCounter.h"
#include <GLFW/glfw3.h>
#include <cstdio>


#include <chrono>
#include <memory>
#include <mutex>
#include <future>


#include "../../include/Render/VulkanRenderer.h"


#include "../../include/Camera/Camera.h"

Camera mcamera;


float mx_cam, my_cam, mz_cam, mr_cam;
float	mx = 1.0f;
float	my = 1.0f;
float	mz = 1.0f;

	float mangle = 0.0f;
	float mdeltaTime = 0.0f;
	float mlastTime = 0.0f;
  
float cam_degree = 0.0f;

	float mwave0;
	float mwave1;
	float mwave2;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



int windowWidth = 0;
int windowHeight = 0;
float main_menu_volume = 0.25f;

using namespace std::chrono_literals;

MainMenuState::MainMenuState(GameData& data)
{
    _data = std::move(&data);
    printf("Main Menu State!\n");
    
}

MainMenuState::~MainMenuState()
{
    printf("Deleting Main Menu\n");
      // Cleanup
//    DeleteData();
    auto v = static_cast<VulkanRenderer*>(_data->vk);
	  //v->cleanup();
    v->recreateSwapChain();
}

void MainMenuState::DeleteData()
{
//    delete _data.get()->ld;
      printf("Deleting Main Menu DATA\n");


}


void MainMenuState::Init()
{
      printf("Hello Main Menu\n");
      x_cam = 0.0f;
      y_cam = 0.0f;
      z_cam = 0.0f;
      r_cam = 0.0f;

    auto v = static_cast<VulkanRenderer*>(_data->vk);
	  v->createMeshModel("Mario.obj", "mario_mime.png");
	  //v->createMeshModel("Mario.obj", "mario_fire.png");
}


void MainMenuState::SoundUpdate()
{
}

void MainMenuState::Input(float delta)
{
    //printf("Main Menu Input \n");
    if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
	    x_cam -=0.05;
      mx -= 5;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
	    x_cam +=0.05;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
	    y_cam +=0.05;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
	    y_cam -=0.05;
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
      z_cam += 0.05;
    }

    if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
    {
      z_cam -= 0.05;
    }
    
    if (glfwGetKey(_data->window, GLFW_KEY_A) == GLFW_PRESS)
    {
      cam_degree += 1.0f;
      if (cam_degree >= 360.0f)
        cam_degree = 0.0f;
    }
    
    if (glfwGetKey(_data->window, GLFW_KEY_D) == GLFW_PRESS)
    {
      cam_degree -= 1.0f;
      if (cam_degree <= 0.0f)
        cam_degree = 360.0f;
    }

}

void MainMenuState::Update(float delta)
{
  mcamera.UpdateViewMatrix();
  //mcamera.SetRotation(mz_cam);
  mcamera.SetRotation(r_cam);
  mcamera.SetPosition(glm::vec3(x_cam, y_cam, z_cam));

  mcamera.RotateAroundPosition(cam_degree, glm::vec3(-1.0f, 0.0f, -2.5f));
  //printf("Maim Menu Updatting...");

  //Input(delta);
  mangle += 1.0f;

  if (mangle >= 360.0f)
    mangle = 0.0f;


  if(fade >= 0.0f)
  {
    fade -= 0.01f;
  }

}

bool menu_screen = false;

void MainMenuState::Render(float delta)
{
//  mcamera.UpdateViewMatrix();
  

  float now = glfwGetTime();
  //deltaTime = now - lastTime;
  //lastTime = now;
  // per-frame time logic
  //
  float current_frame = glfwGetTime();


  // mangle += 10.0f * delta;
  // if(mangle > 360.0f){mangle -= 360.0f;}

  // printf("Model Angle: %f\n", mangle);

  glm::mat4 firstModel(1.0f);

  firstModel = glm::translate(firstModel, glm::vec3(-1.0f, 0.0f, -2.5f));
  firstModel = glm::rotate(firstModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  firstModel = glm::rotate(firstModel, glm::radians(mangle), glm::vec3(0.0f, 1.0f, 0.0f));

  
  auto v = static_cast<VulkanRenderer*>(_data->vk);

  v->updateModel(0, firstModel);

  mwave0 = 1.0f * sin(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave1 = 1.0f * cos(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave2 = 1.0f * sin(2 * 3.14 * 0.0001f * (int)(glfwGetTime() * 100)); 

  //vk_render.pushData.push_constant_colour = vec4(1.0f, 1.0f, 0.0f, 0.0f);	
  //Move Red light with keyboard
  v->lightData.position[0] = glm::vec3(mx, my, mz);
  

  //Red Light
  v->lightData.colour[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);	
  //Blue Light
  v->lightData.colour[1]   = glm::vec4(0.0f, 0.0f, abs(mwave0), 0.0f);
  //Green Light
  v->lightData.colour[2]   = glm::vec4(0.0f, abs(mwave1), 0.0f, 0.0f);
  v->draw();

  v->uboViewProjection.projection = glm::perspective(glm::radians(1000.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  v->uboViewProjection.projection[1][1] *= -1;
  v->uboViewProjection.view = mcamera.GetViewMatrix();

  //std::cout << "Cos wave : " << abs(fadein_wave) << std::endl;
    /*

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    glfwGetWindowSize(_data->window, &windowWidth, &windowHeight);
    //std::cout << "Window Width " << windowWidth << ", " << windowHeight << std::endl;

    ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoMove );
      ImGui::SetWindowPos("main", ImVec2(10.0f, 10.0f));
      ImGui::SetWindowSize("main", ImVec2(windowWidth - 25 , windowHeight - 25));
      ImGui::CaptureMouseFromApp(false);
      ImGui::SetWindowFontScale(1.0f);
    

        ImGui::BeginChild("Sub", ImVec2(600, 100), false, ImGuiWindowFlags_NoBackground);
          ImGui::SetWindowFontScale(2.0f);
          //ImGui::SetCursorPos(ImVec2(500, 100));
          ImGui::Text("                 2D Zombie \n                   Survival", ImVec2(400, 50));
        ImGui::EndChild();
      

      //ImGui::SetCursorPos(ImVec2(100, 50));
      //ImGui::Text("Title", ImVec2(200, 600));

      static bool start;
      ImGui::SetCursorPos(ImVec2(windowWidth/2 - 50, windowHeight/2));
      start = ImGui::Button("START", ImVec2(100, 50));
      if(start || _data->gs.start_game)
        _data->state_switch = LOADING_PLAY_STATE;

      static bool network;
      ImGui::SetCursorPos(ImVec2(windowWidth/2 - 50, windowHeight/2 + 60));
      //ImGui::SetCursorPos(ImVec2(130, 200));
      network = ImGui::Button("SERVER", ImVec2(100, 50));
      if(network){
//         ImGui::BeginChild("Network", ImVec2(100, 100), false, ImGuiWindowFlags_NoBackground);
          _data->gs.server_on = true;
          _data->gs.searching_for_connection = true;
		    //glfwSetWindowShouldClose(_data->window, true);
      }

      if(_data->gs.connection_established && !_data->gs.searching_for_connection)
      {
          ImGui::SetCursorPos(ImVec2(400, 100));
          ImGui::TextColored(ImColor(0, 255, 0), "Connection Established");
      }



      
      static bool game_over;
      ImGui::SetCursorPos(ImVec2(windowWidth/2 - 50, windowHeight/2 + 120));
      //ImGui::SetCursorPos(ImVec2(130, 240));
      game_over = ImGui::Button("MENU", ImVec2(100, 50));
      if(game_over){
          menu_screen = true;
        //_data->state_switch = GAME_OVER_STATE;
        //_data->gs.server_on = true;
        //glfwSetWindowShouldClose(_data->window, true);
      }

      static bool exit;
      ImGui::SetCursorPos(ImVec2(windowWidth/2 - 50, windowHeight/2 + 180));
      //ImGui::SetCursorPos(ImVec2(100, 160));
      exit = ImGui::Button("EXIT", ImVec2(100, 50));
      if(exit){
        _data->gs.server_listening = false;
        _data->gs.connection_established = false;
        _data->gs.exit_app = true;
		    glfwSetWindowShouldClose(_data->window, true);
      }




    ImGui::End();
    
    if(menu_screen)
    {
        ImGui::Begin("MENU", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos("MENU", ImVec2(windowWidth/2 - 200, windowHeight/2 - 100));
        ImGui::SetWindowSize("MENU", ImVec2(400 , 300));
        ImGui::CaptureMouseFromApp(true);
        ImGui::SetWindowFontScale(1.0f);
        
          ImGui::SetCursorPos(ImVec2(150, 10));
          ImGui::Text("Menu Screen", ImVec2(100, 50));
        
          //ImGui::SetCursorPos(ImVec2(50, 50));
          //ImGui::Text("Background Music", ImVec2(100, 50));

          ImGui::SliderFloat("Background Music", &main_menu_volume, 0.0f, 1.0f); 
          ImGui::Checkbox("Fullscreen", &_data->gs.fullScreen);
          ImGui::Checkbox("Window Mode", &_data->gs.windowMode);
          
          //ImGui::SetCursorPos(ImVec2(50, 150));
          //ImGui::Text("Sound FX", ImVec2(100, 50));


        static bool exit_menu;
        ImGui::SetCursorPos(ImVec2(150, 230));
        exit_menu = ImGui::Button("EXIT MENU", ImVec2(100, 50));
        if(exit_menu)
            menu_screen = false;
        ImGui::End();
    }


    if(_data->gs.searching_for_connection)
    {
        ImGui::Begin("Network", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos("Network", ImVec2(windowWidth/2 - 200, windowHeight/2 - 100));
        //ImGui::SetWindowSize("Network", ImVec2(windowWidth - 200 , windowHeight - 200));
        ImGui::SetWindowSize("Network", ImVec2(400 , 200));
        ImGui::CaptureMouseFromApp(false);
          ImGui::SetWindowFontScale(1.0f);
          ImGui::SetCursorPos(ImVec2(150, 100));
          ImGui::Text("Listening for Conection...", ImVec2(100, 50));
        ImGui::End();
    }

	  ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    */
}
