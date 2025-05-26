//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------




#include "../../include/GameData/GameData.h"
#include "../../include/States/template_state.h"
#include "../../include/GameData/ObjectCounter.h"

#include <cstdio>
#include <math.h>
#include <memory>
#include <string>


#include "../../include/Render/VulkanRenderer.h"


#include "../../include/Camera/Camera.h"

Camera camera;

float	rx = 1.0f;
float	ry = 1.0f;
float	rz = 1.0f;

	float m_angle = 0.0f;
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	float wave0;
	float wave1;
	float wave2;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



int current_frame = 0;



TemplateState::TemplateState( GameData& data)
{
    printf("Template State!\n");
    _data = std::move(&data);

}


TemplateState::~TemplateState()
{
    printf("Deleting Template State\n");
//    DeleteData();
    // auto v = static_cast<VulkanRenderer*>(_data->vk);
	//v->cleanup();
    // v->recreateSwapChain();
}

void TemplateState::DeleteData()
{
//    delete _data.get()->ld;
    printf("Deleting Template State\n");
    //ss_actor->GetComponent<RenderComponent>()->onDestroy();

    auto v = static_cast<VulkanRenderer*>(_data->vk);
    if (v->modelList.size() > 0)
    {
        v->cleanModels();
    }
    
}

void TemplateState::Init()
{
    
	x_cam = 250.0f;
	y_cam = 250.0f;
	z_cam = 2.5f;
	r_cam = 0.0f;

    printf("Hello Template State\n");

    //_data->vk.createMeshModel("Mario.obj", "mario_main.png");

    auto v = static_cast<VulkanRenderer*>(_data->vk);
        v->recreateSwapChain();
    if (v->modelList.size() > 0)
    {
        // v->cleanModels();
        // v->recreateSwapChain();

    }
    
	v->createMeshModel("Mario.obj", "mario_main.png", "");
    
}

void TemplateState::SoundUpdate()
{
    //printf("Hello Game Over Sound UPdate\n");
}

void TemplateState::Input(float delta)
{
    if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
	    x_cam -=0.05;
    //   mx -= 5;
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
      printf("Z_cam: %f", z_cam);
      z_cam += 0.05;
    }

    if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
    {
      printf("Z_cam: %f", z_cam);
      z_cam -= 0.05;
    }
    
    if (glfwGetKey(_data->window, GLFW_KEY_A) == GLFW_PRESS)
    {/* 
      cam_degree += 1.0f;
      if (cam_degree >= 360.0f)
        cam_degree = 0.0f;*/
    } 
    
    if (glfwGetKey(_data->window, GLFW_KEY_D) == GLFW_PRESS)
    {/* 
      cam_degree -= 1.0f;
      if (cam_degree <= 0.0f)
        cam_degree = 360.0f */;
    }
    //printf("Main Menu Input \n");
    if (glfwGetKey(_data->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(_data->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(_data->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(_data->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
    }
    if (glfwGetKey(_data->window, GLFW_KEY_E) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(_data->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(_data->window, GLFW_KEY_Z) == GLFW_PRESS)
    {
    }

    if (glfwGetKey(_data->window, GLFW_KEY_X) == GLFW_PRESS)
    {
    }
    
    
    
    if (glfwGetKey(_data->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        current_frame = 2;
    }
    else if (glfwGetKey(_data->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        current_frame = 1;
    }
    else{
        current_frame = 0;
    }
        

    if (glfwGetKey(_data->window, GLFW_KEY_W) == GLFW_PRESS)
    {
    }


}

// int tmp_frames = 0;
void TemplateState::Update(float delta)
{
    // tmp_frames++;
    //std::cout << "UPDATE THREAD : Frames per sec " << tmp_frames/glfwGetTime() << std::endl;

}

void TemplateState::Render(float delta)
{

    //_data->vk.draw();    

			camera.UpdateViewMatrix();
			camera.SetRotation(r_cam);
            // camera.SetPosition(glm::vec3(x_cam, y_cam, z_cam));
            camera.SetPosition(glm::vec3(0.0f, 0.0f, z_cam));
			float now = glfwGetTime();
			deltaTime = now - lastTime;
			lastTime = now;

			// per-frame time logic
			//
			float current_frame = glfwGetTime();


			m_angle += 10.0f * deltaTime;
			if(m_angle > 360.0f){m_angle -= 360.0f;}
            
            auto v = static_cast<VulkanRenderer*>(_data->vk);
            //v->drawUI(nullptr);
            v->drawUI(nullptr, _data);
            for (int i = 0; i < v->modelList.size(); i++)
            {
                // printf("Model Size: %d\n", v->modelList.size());
                glm::mat4 firstModel(1.0f);

                // Model's initial position
                glm::vec3 modelPosition = glm::vec3(0.0f + i, 0.0f, 0.0f);
                // std::cout << "Model position for model " << i << ": " 
                // << glm::to_string(modelPosition) << std::endl;


                // firstModel = glm::translate(firstModel, glm::vec3(0.0f + i, 0.0f, -1.5f));

                // Quaternion representing no rotation
                glm::quat totalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

                // Rotate 45 degrees around the Y-axis
                glm::quat rotationY = glm::angleAxis(glm::radians(m_angle), glm::vec3(0.0f, 1.0f, 0.0f));
                // glm::quat rotationY = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                // glm::quat rotationY = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                totalRotation = rotationY * totalRotation;

                // Rotate 30 degrees around the X-axis
                glm::quat rotationX = glm::angleAxis(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                totalRotation = rotationX * totalRotation;

                // Rotate 60 degrees around the Z-axis
                glm::quat rotationZ = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                totalRotation = rotationZ * totalRotation;

                // Convert the total quaternion rotation to a rotation matrix
                glm::mat4 rotationMatrix = glm::mat4(totalRotation);

                // Apply the rotation to the model matrix
                // modelMatrix = rotationMatrix * modelMatrix;
                // // Apply translation to origin, rotation, and then translation to position
                firstModel = glm::translate(firstModel, modelPosition);   // Translate to position
                firstModel = glm::translate(firstModel, -modelPosition);  // Translate to origin (inverse)
                firstModel = rotationMatrix * firstModel;                 // Apply rotation
                firstModel = glm::translate(firstModel, modelPosition);   // Translate back to position

                // firstModel = rotationMatrix * firstModel;
                // firstModel = glm::translate(firstModel, glm::vec3(0.0f + i, 0.0f, 1.5f));
        
                v->updateModel(i, firstModel);
                // std::cout << "\nModel Matrix for model " << i << std::endl;
                // printMat4(firstModel);
            }

			wave0 = 1.0f * sin(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
			wave1 = 1.0f * cos(2 * 3.14 * 0.001f * (int)(glfwGetTime() * 100)); 
			wave2 = 1.0f * sin(2 * 3.14 * 0.0001f * (int)(glfwGetTime() * 100)); 

			//vk_render.pushData.push_constant_colour = vec4(1.0f, 1.0f, 0.0f, 0.0f);	
			//Move Red light with keyboard
			v->lightData.position[0] = glm::vec3(rx, ry, rz);
			

			//Red Light
			v->lightData.colour[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);	
			//Blue Light
			v->lightData.colour[1]   = glm::vec4(0.0f, 0.0f, abs(wave0), 0.0f);
			//Green Light
			v->lightData.colour[2]   = glm::vec4(0.0f, abs(wave1), 0.0f, 0.0f);
			v->draw();

			v->uboViewProjection.projection = glm::perspective(glm::radians(1000.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			v->uboViewProjection.projection[1][1] *= -1;
			v->uboViewProjection.view = camera.GetViewMatrix();

}
