/////////////////////////////////////////////////////////////////////////////
///-------------------------------ALUT--------------------------------------
#include <AL/al.h>
#include <AL/alut.h>
/////////////////////////////////////////////////////////////////////////////
///------------------------------IMGUI--------------------------------------
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
      z_cam = 2.0f;
      r_cam = 0.0f;
      xpos = 0.0f; 
      ypos = 0.0f; 
      zpos = 0.0f; 
      ui_rotation_control_x = 0.0f;
      ui_rotation_control_y = 0.0f;
      ui_rotation_control_z = 0.0f;
      selectedColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default color is white

      // _data->gs.fullScreen = true;
    auto v = static_cast<VulkanRenderer*>(_data->vk);
        v->recreateSwapChain();
    if (v->modelList.size() > 0)
    {
      
      // v->cleanModels();
      // v->recreateSwapChain();
    }
	  v->createMeshModel("Mario.obj", "mario_mime.png");
    previousModelListSize = v->modelList.size();
	  //v->createMeshModel("Mario.obj", "mario_fire.png");
}


void MainMenuState::SoundUpdate()
{
}

void MainMenuState::Input(float delta)
{
    auto v = static_cast<VulkanRenderer*>(_data->vk);
    //printf("Main Menu Input \n");
   
     // Get the current time
    long long now = currentTimeMillis();

    // Helper function to check if enough time has passed since the last press
    auto canPressKey = [&](int key) {
        if (now - lastKeyPressTime[key] >= KEY_DELAY) {
            lastKeyPressTime[key] = now;
            return true;
        }
        return false;
    };


    
    
    if (glfwGetKey(_data->window, GLFW_KEY_W) == GLFW_PRESS ) {
        xpos += 0.01;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_S) == GLFW_PRESS ) {
        xpos -= 0.01;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_A) == GLFW_PRESS ) {
        ypos += 0.01;
    }
    if (glfwGetKey(_data->window, GLFW_KEY_D) == GLFW_PRESS) {
        ypos -= 0.01;
    }




    if (glfwGetKey(_data->window, GLFW_KEY_H) == GLFW_PRESS && canPressKey(GLFW_KEY_H)) {
        // v->cleanModels();
        printf("Cleared Model Size: %d", v->modelList.size());
        //v->cleanModels();
        v->modelList[0].getMesh(0)->setTexID(v->createTexture("mario_main.png"));

        // v->removeModel(0);
    }
    if (glfwGetKey(_data->window, GLFW_KEY_J) == GLFW_PRESS && canPressKey(GLFW_KEY_J)) {
	    v->createMeshModel("Mario.obj", "mario_mime.png");
    }
    if (glfwGetKey(_data->window, GLFW_KEY_P) == GLFW_PRESS && canPressKey(GLFW_KEY_P)) {
      quaternion_options++;
      if (quaternion_options > 9)
        quaternion_options = 1;
    }


}

void MainMenuState::Update(float delta)
{
/*   mcamera.UpdateViewMatrix();
  //mcamera.SetRotation(mz_cam);
  mcamera.SetRotation(r_cam);
  mcamera.SetPosition(glm::vec3(x_cam, y_cam, z_cam));

  mcamera.RotateAroundPosition(cam_degree, glm::vec3(-1.0f, 0.0f, -2.5f));
  //printf("Maim Menu Updatting...");
 */
  //Input(delta);
  // mangle += 1.0f;

  // if (mangle >= 360.0f)
  //   mangle = 0.0f;


  if(fade >= 0.0f)
  {
    fade -= 0.01f;
  }

 auto v = static_cast<VulkanRenderer*>(_data->vk);

  // for (int i = 0; i < v->modelList.size(); i++)
  // {
  //   // std::string quaternionData;
  //   // std::string modelPostionData; 
  //   std::string matrixDataInfo;


  //   glm::mat4 firstModel(1.0f);
  //   // Model's initial position
  //   glm::vec3 modelPosition = glm::vec3(xpos + i, ypos, zpos);


  //   // Define the center of rotation (if it's different from modelPosition)
  //   glm::vec3 rotationCenter = glm::vec3(0.0f); // Change this if needed

  //     // Translate to the model's position
  //   firstModel = glm::translate(firstModel, modelPosition);

  //   // Translate to the center of rotation
  //   firstModel = glm::translate(firstModel, rotationCenter);

  //   {
  //   // Quaternion representing no rotation
  //   glm::quat totalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  //   glm::quat rotationY = glm::angleAxis(glm::radians((i == 0) ? ui_rotation_control_y: 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  //   // glm::quat rotationY = glm::angleAxis(glm::radians((i == 0) ? ui_rotation_control_y: -mangle), glm::vec3(0.0f, 1.0f, 0.0f));

  //   totalRotation = rotationY * totalRotation;

  //   // Rotate 30 degrees around the X-axis
  //   glm::quat rotationX = glm::angleAxis(glm::radians(ui_rotation_control_x), glm::vec3(1.0f, 0.0f, 0.0f));
  //   totalRotation = rotationX * totalRotation;

  //   // Rotate 60 degrees around the Z-axis
  //   glm::quat rotationZ = glm::angleAxis(glm::radians(ui_rotation_control_z), glm::vec3(0.0f, 0.0f, 1.0f));
  //   totalRotation = rotationZ * totalRotation;

  //   // Convert the total quaternion rotation to a rotation matrix
  //   glm::mat4 rotationMatrix = glm::mat4(totalRotation);

  //   // Apply the rotation to the model matrix
  //   if (i == 0){
  //     firstModel = rotationMatrix * firstModel;                 // Apply rotation
  //     matrixDataInfo +=  demonstrateQuaternionProperties(quaternion_options, firstModel);

  //     // quaternionData[i] += demonstrateQuaternionProperties(quaternion_options, firstModel);
  //     // matrixData[i] += demonstrateQuaternionProperties(quaternion_options, firstModel);
  //     firstModel = glm::translate(firstModel, -rotationCenter);   // Translate back to position
  //   }
  //   else{
  //     // Apply translation and rotation to the model matrix
  //     // firstModel = rotationMatrix * firstModel;                 // Apply rotation
  //     firstModel =  glm::rotate(firstModel, glm::radians(-mangle), glm::vec3(0.0f, 1.0f, 0.0f));
  //     firstModel = glm::translate(firstModel, -rotationCenter);  // Translate to position
  //   }


  //   }
  //   v->updateModel(i, firstModel);
  //   //std::cout << "Model Matrix for model " << i << ":\n" << glm::to_string(firstModel) << std::endl;
  //   std::cout << "\nModel Matrix for model " << i  << std::endl;
  //   std::cout << "###############################" << i  << std::endl;

  //   matrixDataInfo += printMat4ToString(firstModel);
  //   matrixDataInfo += glm::to_string(modelPosition);
  //   std::cout << matrixDataInfo << std::endl;
  //   // modelData[i] += printMat4ToString(firstModel);
  //   // modelPostionData[i] += glm::to_string(modelPosition);
  //   // std::string collectedModelData = quaternionData[i] + modelData[i] + modelPostionData[i];
  //   // matrixData.push_back(matrixDataInfo);

  //   models.push_back(firstModel);

  // }
}

bool menu_screen = false;

void MainMenuState::Render(float delta)
{
	mcamera.UpdateViewMatrix();
	mcamera.SetRotation(r_cam);
  mcamera.SetPosition(glm::vec3(x_cam, y_cam, z_cam));
  mcamera.RotateAroundPosition(cam_degree, glm::vec3(-1.0f, 0.0f, -2.5f));
  float now = glfwGetTime();

  
  auto v = static_cast<VulkanRenderer*>(_data->vk);


 mangle += 1.0f;

 if (mangle >= 360.0f)
    mangle = 0.0f;

 for (int i = 0; i < v->modelList.size(); i++)
 {
    modelData model;
    std::string matrixDataInfo;


    glm::mat4 firstModel(1.0f);
    // Model's initial position
    // model.position = glm::vec3(xpos + i, ypos, zpos);
    // model.position = glm::vec3(0.0f + i, 0.0f, 0.0f);
    // glm::vec3 modelPosition = glm::vec3(xpos + i, ypos, zpos);


    // Define the center of rotation (if it's different from modelPosition)
    glm::vec3 rotationCenter = glm::vec3(0.0f); // Change this if needed

      // Translate to the model's position
    // firstModel = glm::translate(firstModel, modelPosition);
    if(!models.empty())
      firstModel = glm::translate(firstModel, models[i].position);

    // Translate to the center of rotation
    firstModel = glm::translate(firstModel, rotationCenter);

    {
    // Quaternion representing no rotation
    glm::quat totalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::quat rotationY;
    if(!models.empty())
      rotationY = glm::angleAxis(glm::radians((i == 0) ? models[i].rotation_control_ui_y: 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::quat rotationY = glm::angleAxis(glm::radians((i == 0) ? ui_rotation_control_y: -mangle), glm::vec3(0.0f, 1.0f, 0.0f));

    totalRotation = rotationY * totalRotation;

    // Rotate 30 degrees around the X-axis
    glm::quat rotationX;
    if(!models.empty())
      // rotationX = glm::angleAxis(glm::radians(ui_rotation_control_x), glm::vec3(1.0f, 0.0f, 0.0f));
      rotationX = glm::angleAxis(glm::radians(models[i].rotation_control_ui_x), glm::vec3(1.0f, 0.0f, 0.0f));
    
    totalRotation = rotationX * totalRotation;

    // Rotate 60 degrees around the Z-axis
    glm::quat rotationZ;
    
    if(!models.empty())
      rotationZ = glm::angleAxis(glm::radians(models[i].rotation_control_ui_z), glm::vec3(0.0f, 0.0f, 1.0f));
    totalRotation = rotationZ * totalRotation;

    // Convert the total quaternion rotation to a rotation matrix
    glm::mat4 rotationMatrix = glm::mat4(totalRotation);

    // Apply the rotation to the model matrix
    if (i == 0){
      firstModel = rotationMatrix * firstModel;                 // Apply rotation
      matrixDataInfo +=  demonstrateQuaternionProperties(quaternion_options, firstModel);
      firstModel = glm::translate(firstModel, -rotationCenter);   // Translate back to position
    }
    else{
      // Apply translation and rotation to the model matrix
      firstModel = rotationMatrix * firstModel;                 // Apply rotation
      firstModel =  glm::rotate(firstModel, glm::radians(-mangle), glm::vec3(0.0f, 1.0f, 0.0f));
      firstModel = glm::translate(firstModel, -rotationCenter);  // Translate to position
    }


    }
    v->updateModel(i, firstModel);
    //std::cout << "Model Matrix for model " << i << ":\n" << glm::to_string(firstModel) << std::endl;
    // std::cout << "\nModel Matrix for model " << i  << std::endl;
    // std::cout << "###############################" << i  << std::endl;

    matrixDataInfo += printMat4ToString(firstModel);
    // matrixDataInfo += glm::to_string(modelPosition);
    matrixDataInfo += glm::to_string(model.position);

    if (matrixData.empty())
    {
      matrixData.push_back(matrixDataInfo);
      models.push_back(model);
    }
    else if(v->modelList.size() > previousModelListSize)
    {
      matrixData.push_back(matrixDataInfo);
      models.push_back(model);
    }
    else{
      matrixData[i] = matrixDataInfo;
      models[i].name = "Model: " + std::to_string(i);
      models[i].data = matrixDataInfo;
      // if (selectedModelIndex == i)
        // models[i].position = glm::vec3(xpos, ypos, zpos);
      // models[i].position = modelPosition;
      // models[i].position.x = modelPosition.x + xpos;
      // models[i].position.y = modelPosition.y + ypos;
      // models[i].position.z = modelPosition.z + zpos;


        std::cout << "Name: " << models[i].name << std::endl;
        // std::cout << "Data: " << models[i].data << std::endl;
        std::cout << "Position: (" << models[i].position.x << ", " << models[i].position.y << ", " << models[i].position.z << ")" << std::endl;

    }


    previousModelListSize = v->modelList.size();
  }



  v->drawUI( [&]() { this->_ui(matrixData); });

  mwave0 = 1.0f * sin(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave1 = 1.0f * cos(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave2 = 1.0f * sin(2 * 3.14 * 0.0001f * (int)(glfwGetTime() * 100)); 

  // v->pushData.push_constant_colour = vec4(1.0f, 1.0f, 0.0f, 0.0f);	
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

std::string MainMenuState::demonstrateQuaternionProperties(int choice, glm::mat4& modelMatrix) {
    std::string output;
    glm::mat4 matrix = modelMatrix; // Use the provided model matrix

    switch(choice) {
        case 1: { // Unit quaternion
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            output += printQuaternion(rotationQuat, "Rotation Quaternion (45 degrees around Y-axis)");
            output += "Length of quaternion: " + std::to_string(glm::length(rotationQuat)) + "\n";
            glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
            glm::mat4 transformedMatrix = rotationMatrix * matrix;
            modelMatrix = rotationMatrix * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix, "Transformed Matrix");
            output += "\n";
            break;
        }
        case 2: { // Identity quaternion
            output = "";
            glm::quat identityQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            output += printQuaternion(identityQuat, "Identity Quaternion");
            glm::mat4 identityMatrix = glm::mat4_cast(identityQuat);
            glm::mat4 transformedMatrix = identityMatrix * matrix;
            modelMatrix = identityMatrix * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix, "Transformed Matrix");
            output += "\n";
            break;
        }
        case 3: { // Conjugate quaternion
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat conjugateQuat = glm::conjugate(rotationQuat);
            output += printQuaternion(rotationQuat, "Rotation Quaternion (45 degrees around Y-axis)");
            output += printQuaternion(conjugateQuat, "Conjugate of Rotation Quaternion");
            glm::mat4 conjugateMatrix = glm::mat4_cast(conjugateQuat);
            glm::mat4 transformedMatrix = conjugateMatrix * matrix;
            modelMatrix = conjugateMatrix * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix, "Transformed Matrix");
            output += "\n";
            break;
        }
        case 4: { // Inverse quaternion
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat inverseQuat = glm::inverse(rotationQuat);
            output += printQuaternion(rotationQuat, "Rotation Quaternion (45 degrees around Y-axis)");
            output += printQuaternion(inverseQuat, "Inverse of Rotation Quaternion");
            glm::mat4 inverseMatrix = glm::mat4_cast(inverseQuat);
            glm::mat4 transformedMatrix = inverseMatrix * matrix;
            modelMatrix = inverseMatrix * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix, "Transformed Matrix");
            output += "\n";
            break;
        }
        case 5: { // Apply quaternion and its inverse to rotate a point
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat inverseQuat = glm::inverse(rotationQuat);
            glm::vec3 point(1.0f, 0.0f, 0.0f);
            glm::vec3 rotatedPoint = rotationQuat * point;
            glm::vec3 originalPoint = inverseQuat * rotatedPoint;
            output += printQuaternion(rotationQuat, "Rotation Quaternion (45 degrees around Y-axis)");
            output += "Original Point: (" + std::to_string(point.x) + ", " + std::to_string(point.y) + ", " + std::to_string(point.z) + ")\n";
            output += "Rotated Point: (" + std::to_string(rotatedPoint.x) + ", " + std::to_string(rotatedPoint.y) + ", " + std::to_string(rotatedPoint.z) + ")\n";
            output += "Point after applying inverse: (" + std::to_string(originalPoint.x) + ", " + std::to_string(originalPoint.y) + ", " + std::to_string(originalPoint.z) + ")\n";
            output += "\n";
            break;
        }
        case 6: { // Two possible representations for the same rotation
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat negativeQuat = -rotationQuat;
            output += printQuaternion(rotationQuat, "Rotation Quaternion (45 degrees around Y-axis)");
            output += printQuaternion(negativeQuat, "Negative of Rotation Quaternion");
            if (glm::angle(rotationQuat) == glm::angle(negativeQuat) &&
                glm::axis(rotationQuat) == glm::axis(negativeQuat)) {
                output += "q and -q represent the same rotation\n";
            }
            glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
            glm::mat4 negativeMatrix = glm::mat4_cast(negativeQuat);
            glm::mat4 transformedMatrix1 = rotationMatrix * matrix;
            glm::mat4 transformedMatrix2 = negativeMatrix * matrix;
            modelMatrix = transformedMatrix1 * matrix;
            
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix1, "Transformed Matrix (q)");
            output += printMatrix(transformedMatrix2, "Transformed Matrix (-q)");
            output += "\n";
            break;
        }
        case 7: { // Combining rotations
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat rotationQuat2 = glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat combinedQuat = rotationQuat2 * rotationQuat;
            output += printQuaternion(rotationQuat, "Rotation Quaternion 1 (45 degrees around Y-axis)");
            output += printQuaternion(rotationQuat2, "Rotation Quaternion 2 (30 degrees around Y-axis)");
            output += printQuaternion(combinedQuat, "Combined Rotation Quaternion (45 degrees + 30 degrees around Y-axis)");
            glm::mat4 combinedMatrix = glm::mat4_cast(combinedQuat);
            glm::mat4 transformedMatrix = combinedMatrix * matrix;
            modelMatrix = combinedMatrix * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix, "Transformed Matrix");
            output += "\n";
            break;
        }
        case 8: { // Associativity of quaternion multiplication
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat rotationQuat2 = glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat rotationQuat3 = glm::angleAxis(glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat associativeQuat1 = (rotationQuat3 * rotationQuat2) * rotationQuat;
            glm::quat associativeQuat2 = rotationQuat3 * (rotationQuat2 * rotationQuat);
            output += printQuaternion(rotationQuat, "Rotation Quaternion 1 (45 degrees around Y-axis)");
            output += printQuaternion(rotationQuat2, "Rotation Quaternion 2 (30 degrees around Y-axis)");
            output += printQuaternion(rotationQuat3, "Rotation Quaternion 3 (15 degrees around Y-axis)");
            output += printQuaternion(associativeQuat1, "Associative Quaternion 1");
            output += printQuaternion(associativeQuat2, "Associative Quaternion 2");
            if (associativeQuat1 == associativeQuat2) {
                output += "Quaternion multiplication is associative\n";
            }
            glm::mat4 associativeMatrix1 = glm::mat4_cast(associativeQuat1);
            glm::mat4 associativeMatrix2 = glm::mat4_cast(associativeQuat2);
            glm::mat4 transformedMatrix1 = associativeMatrix1 * matrix;
            glm::mat4 transformedMatrix2 = associativeMatrix2 * matrix;
            modelMatrix = associativeMatrix1 * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix1, "Transformed Matrix (Associative 1)");
            output += printMatrix(transformedMatrix2, "Transformed Matrix (Associative 2)");
            output += "\n";
            break;
        }
        case 9: { // Non-commutativity of quaternion multiplication
            output = "";
            glm::quat rotationQuat = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat rotationQuat2 = glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat nonCommQuat1 = rotationQuat2 * rotationQuat;
            glm::quat nonCommQuat2 = rotationQuat * rotationQuat2;
            output += printQuaternion(rotationQuat, "Rotation Quaternion 1 (45 degrees around Y-axis)");
            output += printQuaternion(rotationQuat2, "Rotation Quaternion 2 (30 degrees around Y-axis)");
            output += printQuaternion(nonCommQuat1, "Non-commutative Quaternion 1 (q2 * q1)");
            output += printQuaternion(nonCommQuat2, "Non-commutative Quaternion 2 (q1 * q2)");
            if (nonCommQuat1 != nonCommQuat2) {
                output += "Quaternion multiplication is not commutative\n";
            }
            glm::mat4 nonCommMatrix1 = glm::mat4_cast(nonCommQuat1);
            glm::mat4 nonCommMatrix2 = glm::mat4_cast(nonCommQuat2);
            glm::mat4 transformedMatrix1 = nonCommMatrix1 * matrix;
            glm::mat4 transformedMatrix2 = nonCommMatrix2 * matrix;
            modelMatrix = transformedMatrix1 * matrix;
            output += printMatrix(matrix, "Original Matrix");
            output += printMatrix(transformedMatrix1, "Transformed Matrix (Non-commutative 1)");
            output += printMatrix(transformedMatrix2, "Transformed Matrix (Non-commutative 2)");
            output += "\n";
            break;
        }
        default: {
            output += "Invalid choice\n";
            break;
        }
    }
    return output;
}


void MainMenuState::_ui(std::vector<std::string> gui_data)
{
   // Variables for the second window
    static bool enable_feature = false;
    static int slider_value = 0;

    // Second window
    auto v = static_cast<VulkanRenderer*>(_data->vk);

    // Start a new ImGui window
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.1f)); // Set background color with transparency
    ImGui::Begin("Model Data");
    // Iterate over all models in the modelList and display their matrices
    // for (auto& modelData : matrixData)
    // {
    //     //ImGui::Text("Model Matrix Data:");
    //     //ImGui::Text(modelData.c_str());
    //     //ImGui::Separator(); // Add a separator between models
    // }
    for (int i = 0; i < v->modelList.size(); i++) {
        ImGui::Text("Model %d Matrix:", i);
        // ImGui::Text(matrixData.c_str());
        if (!gui_data.empty())
          // ImGui::Text(matrixData_package[i].c_str());
          ImGui::Text(gui_data[i].c_str());
        // ImGui_PrintMat4(models[i]); // Assuming you store matrices in a list calleid modelMatrices
        ImGui::Separator(); // Add a separator between models
    }
    // Buttons to increase and decrease quaternion options
    // if (ImGui::Button("Increase Quaternion Options")) {
    //     quaternion_options++;
    //     if (quaternion_options > 9)
    //       quaternion_options = 1;
    // }
    // if (ImGui::Button("Decrease Quaternion Options")) {
    //     quaternion_options--;
    //     if (quaternion_options > 9)
    //       quaternion_options = 1;
    // }


    // ImGui::Text("Model Matrix for model ");
    // printMat4(firstModel);
    // ImGui::Checkbox("Enable Feature", &enable_feature);
    // ImGui::SliderInt("Value", &slider_value, 0, 100);
    // if (ImGui::Button("Reset")) {
    //     slider_value = 0;
    // }
    ImGui::End();
    ImGui::PopStyleColor(); // Restore default window background color
    // Start a new ImGui window
    ImGui::Begin("Model Controls");


      ImGui::ColorPicker4("Pick a Color", (float*)&selectedColor, ImGuiColorEditFlags_Float);
      v->pushData.push_constant_colour = vec4(selectedColor.x, selectedColor.y, selectedColor.z, 0.0f);	
      // Button to clear models
      if (ImGui::Button("Clear Models")) {
          v->cleanModels();
      }



    // Pulldown list to select model and texture
    static int selectedItem = 0;
    static const char* items[] = { "Model 1", "Model 2", "Model 3" , "Model 4"}; // Add model names here
    if (ImGui::Combo("Select Model", &selectedItem, items, IM_ARRAYSIZE(items))) {
        // Load selected model based on selectedItem
        switch (selectedItem) {
            case 0:
              v->createMeshModel("Mario.obj", "mario_main.png");
                // Load Model 1
                // v->loadModel("model1.obj", "texture1.png");
                break;
            case 1:
              v->createMeshModel("Mario.obj", "mario_fire.png");
                // Load Model 2
                // v->loadModel("model2.obj", "texture2.png");
                break;
            case 2:
              v->createMeshModel("Mario.obj", "mario_mime.png");
                // Load Model 3
                // v->loadModel("model3.obj", "texture3.png");
                break;
            case 3:
              v->createMeshModel("Dragon.3ds", "Dragon_ground_color.jpg");

                // Load Model 3
                // v->loadModel("model3.obj", "texture3.png");
                break;
            // Add more cases for additional models
        }
    }

    // // Mode selection
    // static int mode = 0;
    // static const char* modes[] = { "Mode 1", "Mode 2", "Mode 3" }; // Add mode names here
    // if (ImGui::Combo("Select Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
    //     // Set mode based on selection
    //     switch (mode) {
    //         case 0:
    //             // Set mode to Mode 1
    //             // v->setMode(MODE1);
    //             break;
    //         case 1:
    //             // Set mode to Mode 2
    //             // v->setMode(MODE2);
    //             break;
    //         case 2:
    //             // Set mode to Mode 3
    //             // v->setMode(MODE3);
    //             break;
    //         // Add more cases for additional modes
    //     }
    // }

      // Render dropdown list to select the model
    // if (ImGui::BeginCombo("Available Models", v->modelList[selectedModelIndex].name.c_str())) {
    if (ImGui::BeginCombo("Available Models", std::to_string(selectedModelIndex).c_str())) {
        for (int i = 0; i < v->modelList.size(); i++) {
            bool isSelected = (selectedModelIndex == i);
            if (ImGui::Selectable("Place Holder " + i, isSelected)) {
                selectedModelIndex = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::Button("Delete Model")) {
        if (!v->modelList.empty()){
          v->removeModel(selectedModelIndex);
          matrixData.erase(matrixData.begin() + selectedModelIndex);
          selectedModelIndex = 0;
        }
    }
     // Apply button to apply texture to the selected model
    if (ImGui::Button("Apply Texture")) {
        // Assuming vk_render.modelList is your list of models
        if (selectedModelIndex < v->modelList.size()) {
            v->modelList[selectedModelIndex].getMesh(0)->setTexID(v->createTexture("mario_main.png"));
        }
    }











    ImGui::Text("Select Quaternion Option:");
    for (int i = 1; i < 9; ++i) {
        if (ImGui::RadioButton((quaternion_title_options[i]).c_str(), &quaternion_options, i)) {
            // If the radio button is clicked, update the quaternion option
            quaternion_options = i;
        }
    }


    // Slider for X position
    // ImGui::SliderFloat("X Position", &xpos, -10.0f, 10.0f);
    ImGui::SliderFloat("X Position", &models[selectedModelIndex].position.x, -10.0f, 10.0f);

    // Slider for Y position
    // ImGui::SliderFloat("Y Position", &ypos, -10.0f, 10.0f);
    ImGui::SliderFloat("Y Position", &models[selectedModelIndex].position.y, -10.0f, 10.0f);

    // Slider for Z position
    ImGui::SliderFloat("Z Position", &models[selectedModelIndex].position.z, -10.0f, 10.0f);
    
    // Slider for Quaternion rotation
    ImGui::SliderFloat("X Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_x, 0.0f, 360.0f);
    ImGui::SliderFloat("Y Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_y, 0.0f, 360.0f);
    ImGui::SliderFloat("Z Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_z, 0.0f, 360.0f);

    ImGui::End();
    ImGui::Begin("Camera Controls");
     ImGui::Text("Use Arrow Keys or Joystick to Control Camera:");

    // X Position
    ImGui::SliderFloat("X Position", &x_cam, -10.0f, 10.0f);

    // Y Position
    ImGui::SliderFloat("Y Position", &y_cam, -10.0f, 10.0f);

    // Z Position
    ImGui::SliderFloat("Z Position", &z_cam, -10.0f, 10.0f);

    // Rotation
    ImGui::SliderAngle("Rotation", &r_cam, -180.0f, 180.0f);

    // Camera Rotation (in degrees)
    ImGui::SliderAngle("Camera Degree", &cam_degree, 0.0f, 360.0f);

    ImGui::End();
}
