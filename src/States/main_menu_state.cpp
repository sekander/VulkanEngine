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
#include <algorithm>


#include <chrono>
#include <memory>
#include <mutex>
#include <future>

// Add ImGuiFileDialog include if using ImGuiFileDialog library


#include "../../include/Render/VulkanRenderer.h"


#include "../../include/Camera/Camera.h"

Camera mcamera;


float	mx = 1.0f;
float	my = 1.0f;
float	mz = 1.0f;

	float mangle = 0.0f;
	float mdeltaTime = 0.0f;
	float mlastTime = 0.0f;
  
float cam_degree = 0.0f;  glm::vec3 _eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);


	float mwave0;
	float mwave1;
	float mwave2;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int maxLights = 0;
std::vector<vec3> lightPositions = { vec3(2.0f, 2.0f, 2.0f), vec3(-2.0f, 2.0f, 2.0f), vec3(0.0f, -2.0f, 2.0f) };
std::vector<vec3> lightColors = { vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) };
std::vector<float> lightAmbientStrength = { 0.2f, 0.2f, 0.2f };
std::vector<float> lightDiffuseStrength = { 0.5f, 0.5f, 0.5f };
std::vector<float> lightSpecularStrength = { 1.0f, 1.0f, 1.0f };
std::vector<float> lightShininess = { 32.0f, 32.0f, 32.0f };
std::vector<float> lightRadius = { 1.0f, 1.0f, 1.0f }; // Default radius for each light
glm::vec3 rimLightColour(1.0f, 0.0f, 1.0f); // White color
float rimLightStrength = 0.5f; // Default strength

int windowWidth = 0;
int windowHeight = 0;
float main_menu_volume = 0.25f;

glm::vec3 scaleFactor(0.5f); // Change this as needed, e.g., glm::vec3(1.5f, 1.5f, 1.5f);

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
    // auto v = static_cast<VulkanRenderer*>(_data->vk);
	  //v->cleanup();
    // v->recreateSwapChain();
}


void MainMenuState::DeleteData()
{
//    delete _data.get()->ld;
      printf("Deleting Main Menu DATA\n");

    auto v = static_cast<VulkanRenderer*>(_data->vk);
    if (v->modelList.size() > 0)
    {
        v->cleanModels();
    }

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
	  // v->createMeshModel("Mario.obj", "mario_mime.png");
	  v->createMeshModel("Mario.obj", "mario_mime.png", "mario_main_normal.png");
    // v->createMeshModel("sphere.obj", "default-grey.jpg"); // Use your own mesh/texture
    // v->createMeshModel("brick_wall.obj", "wall.jpeg", "");
	  // v->createMeshModel("floor.obj", "floor.jpg", "floor_normal.jpeg");
	  // v->createMeshModel("Mario.obj", "mario_main_normal.png");
	  // v->createMeshModel("Mario.obj", "", "mario_main_normal.png");
    previousModelListSize = v->modelList.size();

    maxLights = 3;
    // v->pushData.push_constant_maxLights = maxLights;
    // Resize lightPositions vector based on maxLights
    lightPositions.resize(maxLights, glm::vec3(0.0f));
    lightColors.resize(maxLights, glm::vec3(1.0f, 1.0f, 1.0f)); // Default color is white
	  //v->createMeshModel("Mario.obj", "mario_fire.png");
    // Lighting parameters (controlled by ImGui)
    // v->pushData.push_constant_rimLightColour = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    // v->pushData.push_constant_rimLightStrength = 0.5f; // Default strength


    // v->pushData.outlineThickness = 0.2f; // Default outline thickness
    v->pushData.sdfCenter = glm::vec3(0.0f, 0.0f, 0.0f); // Default center
    v->pushData.sdfRadius = 1.0f; // Default radius

    // Rotate the camera 90 degrees around Y-axis
    // mcamera.SetRotation(glm::radians(-180.0f));
    r_cam = -179.0f;
    z_cam = -10.0f;

    // Or use Euler if needed (pitch, yaw, roll)


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
  
    // Use the global eulerAngles variable instead of redeclaring it here
    // mcamera.SetEulerRotation(glm::radians(eulerAngles));
    // Control vector, e.g. updated by user input
  glm::vec3 rotationInput = glm::radians(glm::vec3(_eulerAngles.x, _eulerAngles.y, _eulerAngles.z));

  mcamera.SetEulerRotation(rotationInput);


  // mcamera.RotateAroundPosition(cam_degree, glm::vec3(-1.0f, 0.0f, -2.5f));
  static float lastTime = glfwGetTime();
  static float totalElapsed = 0.0f;

  float now = glfwGetTime();
  float elapsedTime = now - lastTime;
  totalElapsed += elapsedTime;

  lastTime = now;

  // printf("Elapsed Time: %f\n", totalElapsed);
  
  auto v = static_cast<VulkanRenderer*>(_data->vk);
//  models[selectedModelIndex].position;

  v->pushData.time = totalElapsed; // Update the time in push constants


  printf("Time : %f\n", v->pushData.time);


//  mangle += 1.0f;

 if (mangle >= 360.0f)
    mangle = 0.0f;

  //Model Update
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

    if(!models.empty())
      v->pushData.sdfCenter = models[selectedModelIndex].position; // Default center
      

    {
    // Quaternion representing no rotation
    glm::quat totalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::quat rotationY;
    if(!models.empty())
      // rotationY = glm::angleAxis(glm::radians((i == 0) ? models[i].rotation_control_ui_y: 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      rotationY = glm::angleAxis(glm::radians(models[i].rotation_control_ui_y), glm::vec3(0.0f, 1.0f, 0.0f));
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

    glm::vec3 scaleFactor(1.0f); // Change this as needed, e.g., glm::vec3(1.5f, 1.5f, 1.5f);


    if(!models.empty())
      scaleFactor = models[i].scale;
    // Apply the rotation to the model matrix
    // if (i == 0)
    {
      firstModel = rotationMatrix * firstModel;                 // Apply rotation
      matrixDataInfo +=  demonstrateQuaternionProperties(quaternion_options, firstModel);
      firstModel = glm::translate(firstModel, -rotationCenter);   // Translate back to position
      firstModel = glm::scale(firstModel, scaleFactor);
    }
    // else{
    //   // Apply translation and rotation to the model matrix
    //   firstModel = rotationMatrix * firstModel;                 // Apply rotation
    //   firstModel =  glm::rotate(firstModel, glm::radians(-mangle), glm::vec3(0.0f, 1.0f, 0.0f));
    //   firstModel = glm::translate(firstModel, -rotationCenter);  // Translate to position
    // }


    }
    // v->pushData.push_constant_normalMat= glm::mat4(1.0f);

    // Apply scaling
    // firstModel = glm::scale(firstModel, scaleFactor);
    // Only scale the selected model
    // if (i == selectedModelIndex) 
    {
      // firstModel = glm::scale(firstModel, models[i].scale);
    }
    // Calculate the normal matrix (transpose of the inverse of the upper-left 3x3 of the model matrix)
    glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(firstModel)));
    v->pushData.push_constant_normalMat = normMat;
    // Print the normal matrix in a pretty format
    // printf("Normal Matrix:\n");
    // for (int row = 0; row < 3; ++row) {
    //   printf("[ ");
    //   for (int col = 0; col < 3; ++col) {
    //     printf("% .4f ", normMat[row][col]);
    //   }
    //   printf("]\n");
    // }

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
      // models[i].name = "Model: " + std::to_string(i);
      models.push_back(model);
    }
    else if(v->modelList.size() > previousModelListSize)
    {
      matrixData.push_back(matrixDataInfo);
      // models[i].name = "Model: " + std::to_string(i);
      models.push_back(model);
    }
    else{
      //Update Model Data
      matrixData[i] = matrixDataInfo;
      models[i].data = matrixDataInfo;
      models[i].name = "Model: " + std::to_string(i);


        // std::cout << "Name: " << models[i].name << std::endl;
        // std::cout << "Data: " << models[i].data << std::endl;
        // std::cout << "Position: (" << models[i].position.x << ", " << models[i].position.y << ", " << models[i].position.z << ")" << std::endl;
    }


    previousModelListSize = v->modelList.size();
  }



  v->drawUI( [&]() { this->_ui(matrixData); }, _data);

  mwave0 = 1.0f * sin(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave1 = 1.0f * cos(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
  mwave2 = 1.0f * sin(2 * 3.14 * 0.0001f * (int)(glfwGetTime() * 100)); 

  // v->pushData.push_constant_colour = vec4(1.0f, 1.0f, 0.0f, 0.0f);	
  //Move Red light with keyboard
  // v->lightData.position[0] = glm::vec3(mx, my, mz);
  // Update light data as before
  for (int i = 0; i < maxLights && i < lightPositions.size(); ++i) {
    v->lightData.position[i] = glm::vec3(lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
    v->lightData.colour[i] = glm::vec4(lightColors[i].x, lightColors[i].y, lightColors[i].z, 0.0f);
    v->lightData.ambientStrength[i] = lightAmbientStrength[i];
    v->lightData.diffuseStrength[i] = lightDiffuseStrength[i];
    v->lightData.specularStrength[i] = lightSpecularStrength[i];
    v->lightData.shininess[i] = lightShininess[i];
    v->lightData.radius[i] = lightRadius[i]; // Set the radius for each light
  }

  // v->pushData.push_constant_rimLightColour = rimLightColour; // White color
  // v->pushData.push_constant_rimLightStrength = rimLightStrength; // Default strength

  //Red Light
  // v->lightData.colour[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  // //Blue Light
  // v->lightData.colour[1]   = glm::vec4(0.0f, 0.0f, abs(mwave0), 0.0f);
  // //Green Light
  // v->lightData.colour[2]   = glm::vec4(0.0f, abs(mwave1), 0.0f, 0.0f);
  v->draw();

  v->uboViewProjection.projection = glm::perspective(glm::radians(1000.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  v->uboViewProjection.projection[1][1] *= -1;
  v->uboViewProjection.view = mcamera.GetViewMatrix();
  v->uboViewProjection.cameraPos = mcamera.GetPosition();

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
    v->pushData.push_constant_colour = vec4(selectedColor.x, selectedColor.y, selectedColor.z, selectedColor.w);	
    // Button to clear models
    if (ImGui::Button("Clear Models")) {
      v->cleanModels();
    }



    // Pulldown list to select model and texture
    static int selectedItem = 0;
    static const char* items[] = { "Mario Main", 
                                   "Pixel Mario", 
                                   "Dragon", 
                                   "Street",
                                   "Wall",
                                   "Ford",
                                   "Charizard",
                                   "Skull",
                                   "Sphere",
                                  }; // Add model names here
    if (ImGui::Combo("Select Model", &selectedItem, items, IM_ARRAYSIZE(items))) {
        // Load selected model based on selectedItem
        switch (selectedItem) {
            case 0:
              v->createMeshModel("Mario.obj", "mario_main.png");
                break;
            case 1:
              v->createMeshModel("PixelMario.obj", "mario2.jpg", "mario2_norm.png");
                break;
            case 2:
              v->createMeshModel("Dragon.3ds", "Dragon_ground_color.jpg");
                break;
            case 3:
              v->createMeshModel("street.obj", "street-diffuse.jpeg", "street-normal.jpeg");
                break;
            case 4:
              v->createMeshModel("wall.obj", "brick.jpeg", "brick_normal.jpeg");
                break;
            case 5:
              v->createMeshModel("ford.obj", "0000.jpg");
                break;
            case 6:
              v->createMeshModel("charizard.obj", "charizard.jpg", "charizard_specular.png");
                break;
            case 7:
              v->createMeshModel("Skull.obj", "skull_texture.jpg", "skull_norm.png");
                break;
            case 8:
              v->createMeshModel("sphere.obj", "default-grey.jpg", "");
                break;
            // case 8:
            //   v->createMeshModel("gameboyadv.obj", "gameboyadv.jpg", "gameboyadv_norm.png");
            //     break;
            // case 9:
            //   v->createMeshModel("jigglypuff.obj", "jigglypuff.jpg", "jigglypuff_norm.png");
            //     break;
            // case 11:
            //   v->createMeshModel("pineapple-juice.obj", "pineapple.jpg", "pineapple_norm.png");
            //     break;
            // case 12:
            //   v->createMeshModel("watermellon.obj", "watermellon.png", "watermellon_norm.png");
            //     break;
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
    // if (ImGui::BeginCombo("Available Models", std::to_string(selectedModelIndex).c_str())) {
    //     for (int i = 0; i < v->modelList.size(); i++) {
    //         bool isSelected = (selectedModelIndex == i);
    //         if (ImGui::Selectable("Place Holder " + i, isSelected)) {
    //             selectedModelIndex = i;
    //         }
    //         if (isSelected) {
    //             ImGui::SetItemDefaultFocus();
    //         }
    //     }
    //     ImGui::EndCombo();
    // }

        ImGui::Separator();

    ImGui::Text("Manage Models");
    // Combo box to select available models by name
    if (ImGui::BeginCombo("Available Models", (models.empty() || selectedModelIndex >= models.size()) ? "" : models[selectedModelIndex].name.c_str())) {
      for (int i = 0; i < v->modelList.size(); i++) {
        bool isSelected = (selectedModelIndex == i);
        const char* modelName = (i < models.size() && !models[i].name.empty())
                      ? models[i].name.c_str()
                      : ("Model " + std::to_string(i)).c_str();
                      
        if (ImGui::Selectable(modelName, isSelected)) {
          selectedModelIndex = i;
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    

    // Show the world space position of the selected model
    if (selectedModelIndex < models.size()) {
      const glm::vec3& pos = models[selectedModelIndex].position;
      ImGui::Text("World Position: (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
    }

    // Radio buttons to select texture for the selected model
    static int textureOption = 0;
    ImGui::Text("Apply Texture:");
    ImGui::RadioButton("Mario Main", &textureOption, 0);
    ImGui::RadioButton("Mario Fire", &textureOption, 1);
    ImGui::RadioButton("Mario Mime", &textureOption, 2);

    if (ImGui::Button("Apply Texture")) {
      if (selectedModelIndex < v->modelList.size()) {
        const char* texFile = nullptr;
        switch (textureOption) {
          case 0: texFile = "mario_main.png"; break;
          case 1: texFile = "mario_fire.png"; break;
          case 2: texFile = "mario_mime.png"; break;
          default: texFile = "mario_main.png"; break;
        }
        v->modelList[selectedModelIndex].getMesh(0)->setTexID(v->createTexture(texFile));
      }
    }

        // Apply button to open file dialog and apply texture to the selected model
    // if (ImGui::Button("Apply Texture")) {
    //   // Use ImGuiFileDialog or similar library for file browsing
    //   // Example assumes ImGuiFileDialog is integrated and available
    //   ImGuiFileDialog::Instance()->OpenDialog("ChooseTexFileDlgKey", "Choose Texture", ".png,.jpg,.jpeg,.bmp,.tga", ".");
    // }

    // // Show file dialog and handle selection
    // if (ImGuiFileDialog::Instance()->Display("ChooseTexFileDlgKey")) {
    //   if (ImGuiFileDialog::Instance()->IsOk()) {
    //     std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
    //     if (selectedModelIndex < v->modelList.size()) {
    //       v->modelList[selectedModelIndex].getMesh(0)->setTexID(v->createTexture(filePath.c_str()));
    //     }
    //   }
    //   ImGuiFileDialog::Instance()->Close();
    // }



    if (ImGui::Button("Delete Model")) {
        if (!v->modelList.empty()){
          v->removeModel(selectedModelIndex);
          matrixData.erase(matrixData.begin() + selectedModelIndex);
          selectedModelIndex = 0;
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
    ImGui::SliderFloat("X Position", &models[selectedModelIndex].position.x, -100.0f, 100.0f);

    // Slider for Y position
    // ImGui::SliderFloat("Y Position", &ypos, -10.0f, 10.0f);
    ImGui::SliderFloat("Y Position", &models[selectedModelIndex].position.y, -100.0f, 100.0f);

    // Slider for Z position
    ImGui::SliderFloat("Z Position", &models[selectedModelIndex].position.z, -100.0f, 100.0f);

    // Slider for Quaternion rotation
    ImGui::SliderFloat("X Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_x, 0.0f, 360.0f);
    ImGui::SliderFloat("Y Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_y, 0.0f, 360.0f);
    ImGui::SliderFloat("Z Quaternion Theta ", &models[selectedModelIndex].rotation_control_ui_z, 0.0f, 360.0f);

    // Slider for scaling (X, Y, Z)
    // Slider for scaling (X, Y, Z)
    ImGui::SliderFloat3("Scale (X, Y, Z)", glm::value_ptr(models[selectedModelIndex].scale), 0.1f, 3.0f);
    ImGui::End();

    // Light Position Controls
    ImGui::Begin("Light Controls");
    // ImGui::Text("Light Position Controls & Color Controls");
    for (int i = 0; i < maxLights; ++i) {
      std::string lightType = (i == 0) ? "Point" : (i == 1) ? "Directional" : "Spot";
      std::string lightName = "Light Type: " + lightType;

      ImGui::Text("%s", lightName.c_str());
      ImGui::SliderFloat(("Light " + std::to_string(i) + " X").c_str(), &lightPositions[i].x, -100.0f, 100.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Y").c_str(), &lightPositions[i].y, -100.0f, 100.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Z").c_str(), &lightPositions[i].z, -100.0f, 100.0f);
      ImGui::ColorEdit3(("Light " + std::to_string(i) + " Color").c_str(), glm::value_ptr(lightColors[i]));
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Ambient Strength").c_str(), &lightAmbientStrength[i], 0.0f, 1.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Diffuse Strength").c_str(), &lightDiffuseStrength[i], 0.0f, 1.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Specular Strength").c_str(), &lightSpecularStrength[i], 0.0f, 2.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Shininess").c_str(), &lightShininess[i], 1.0f, 128.0f);
      ImGui::SliderFloat(("Light " + std::to_string(i) + " Radius").c_str(), &lightRadius[i], 0.1f, 100.0f);
      // Reset button for each light
      ImGui::SameLine();
      if (ImGui::Button(("Reset##Light" + std::to_string(i)).c_str())) {
        // Restore default values for this light
        if (i == 0) {
          lightPositions[i] = glm::vec3(2.0f, 2.0f, 2.0f);
          lightColors[i] = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (i == 1) {
          lightPositions[i] = glm::vec3(-2.0f, 2.0f, 2.0f);
          lightColors[i] = glm::vec3(0.0f, 1.0f, 0.0f);
        } else if (i == 2) {
          lightPositions[i] = glm::vec3(0.0f, -2.0f, 2.0f);
          lightColors[i] = glm::vec3(0.0f, 0.0f, 1.0f);
        } else {
          lightPositions[i] = glm::vec3(0.0f);
          lightColors[i] = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        lightAmbientStrength[i] = 0.1f;
        lightDiffuseStrength[i] = 0.5f;
        lightSpecularStrength[i] = 1.0f;
        lightShininess[i] = 32.0f;
      }
      ImGui::Separator();
    }
    // Rim Light Controls
    ImGui::Text("Rim Light Controls");
    ImGui::ColorEdit3("Rim Light Color", glm::value_ptr(rimLightColour));
    ImGui::SliderFloat("Rim Light Strength", &rimLightStrength, 0.0f, 1.0f);
    ImGui::End();


    // ImGui::SliderFloat3("Scale (X, Y, Z)", glm::value_ptr(scaleFactor), 0.1f, 3.0f);

    ImGui::Begin("Camera Controls");
     ImGui::Text("Use Arrow Keys or Joystick to Control Camera:");

    // X Position
    ImGui::SliderFloat("X Position", &x_cam, -100.0f, 100.0f);

    // Y Position
    ImGui::SliderFloat("Y Position", &y_cam, -100.0f, 100.0f);

    // Z Position
    ImGui::SliderFloat("Z Position", &z_cam, -100.0f, 100.0f);

    // Rotation
    ImGui::SliderAngle("Rotation", &r_cam, -180.0f, 180.0f);

    // Camera Rotation (in degrees)
    ImGui::SliderAngle("Camera Degree", &cam_degree, 0.0f, 360.0f);

        // Camera Euler Rotation Controls
        // static glm::vec3 eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
        ImGui::Text("Euler Rotation (degrees):");
        ImGui::SliderFloat("Pitch (X)", &_eulerAngles.x, -180.0f, 180.0f);
        ImGui::SliderFloat("Yaw (Y)", &_eulerAngles.y, -180.0f, 180.0f);
        ImGui::SliderFloat("Roll (Z)", &_eulerAngles.z, -180.0f, 180.0f);

        // if (ImGui::Button("Apply Euler Rotation")) {
        //   mcamera.SetEulerRotation(glm::radians(eulerAngles));
        // }
        // mcamera.SetEulerRotation(glm::vec3(glm::radians(-10.0f), glm::radians(45.0f), 0.0f));


    ImGui::End();
	
}
