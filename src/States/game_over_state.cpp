//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------

#include "../../include/GameData/GameData.h"
#include "../../include/States/game_over_state.h"

/////////////////////////////////////////////////////////////////////////////
///------------------------------IMGUI--------------------------------------
#include "../../include/IMGUI/imgui.h"
#include "../../include/IMGUI/imgui_impl_glfw.h"
#include "../../include/IMGUI/imgui_impl_opengl3.h" 

#include <cstdio>
#include <math.h>


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GameOverState::GameOverState( GameData& data)
{
	_data = std::move(&data);

    printf("Game Over State!\n");




	x_cam = 0.0f;
	y_cam = 0.0f;
	z_cam = 0.0f;
	r_cam = 0.0f;

}


GameOverState::~GameOverState()
{
    printf("Deleting Game Over State\n");
//    DeleteData();
}

void GameOverState::DeleteData()
{
//    delete _data.get()->ld;
    printf("Deleting Game Over State\n");

}

void GameOverState::Init()
{
    printf("Hello Game Over State\n");
}

void GameOverState::SoundUpdate()
{
    //printf("Hello Game Over Sound UPdate\n");
}

void GameOverState::Input(float delta)
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

void GameOverState::Update(float delta)
{
   // printf("Main Menu Update\n");
 //       _data.get()->manager->Update();

  if(go_fade_in >= 0.0f)
  {
    go_fade_in -= 0.01f;
  }


}

void GameOverState::Render(float delta)
{
   // printf("Main Menu Render\n");
 
    /*
    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    //glfwGetWindowSize(_data->window, &windowWidth, &windowHeight);
    //std::cout << "Window Width " << windowWidth << ", " << windowHeight << std::endl;

    ImGui::Begin("main", nullptr, ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoMove );
      ImGui::SetWindowPos("main", ImVec2(10.0f, 10.0f));
      ImGui::SetWindowSize("main", ImVec2(_data->gs.SCR_WIDTH - 25 , _data->gs.SCR_HEIGHT - 25));
      ImGui::CaptureMouseFromApp(false);
      ImGui::SetWindowFontScale(1.0f);
    

        ImGui::BeginChild("Sub", ImVec2(600, 100), false, ImGuiWindowFlags_NoBackground);
          ImGui::SetWindowFontScale(2.0f);
          //ImGui::SetCursorPos(ImVec2(500, 100));
          ImGui::Text("GAME OVER!!!", ImVec2(400, 50));
        ImGui::EndChild();
      

      //ImGui::SetCursorPos(ImVec2(100, 50));
      //ImGui::Text("Title", ImVec2(200, 600));

      static bool start;
      ImGui::SetCursorPos(ImVec2(_data->gs.SCR_WIDTH/2 - 50, _data->gs.SCR_HEIGHT/2));
      start = ImGui::Button("PLAY AGAIN", ImVec2(100, 50));
      //if(start || _data->gs.start_game)
      if(start )
        _data->state_switch = LOADING_PLAY_STATE;


      static bool exit;
      ImGui::SetCursorPos(ImVec2(_data->gs.SCR_WIDTH/2 - 50, _data->gs.SCR_HEIGHT/2 + 180));
      //ImGui::SetCursorPos(ImVec2(100, 160));
      exit = ImGui::Button("EXIT", ImVec2(100, 50));
      if(exit){
        _data->gs.server_listening = false;
        _data->gs.connection_established = false;
        _data->gs.exit_app = true;
		    glfwSetWindowShouldClose(_data->window, true);
      }




    ImGui::End();
    
	ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
*/



























}
