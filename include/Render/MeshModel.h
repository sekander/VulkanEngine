#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"

#include <assimp/scene.h>

class MeshModel
{
    public:
        MeshModel(){}
        MeshModel(std::vector<Mesh> newMeshList);
        ~MeshModel(){}


        size_t getMeshCount(){return meshList.size();}
        Mesh* getMesh(size_t index){
            if(index >= meshList.size())
                throw std::runtime_error("Attempted to access invalid Mesh index");

            return &meshList[index];
        }


        glm::mat4 getModel(){return model;}

        void setModel(glm::mat4 newModel){
            model = newModel;
            
        }

        //void setNorm(glm::mat4 model)
        //{
            
        //}



        void destroyMeshModel(){
            for(auto& mesh: meshList)
                mesh.destroyBuffers();
        }

        static std::vector<std::string> LoadMaterials(const aiScene* scene);
        static std::vector<Mesh> LoadNode(VkPhysicalDevice newPhysicalDevice, 
                                          VkDevice newDevice, 
                                          VkQueue transferQueue, 
                                          VkCommandPool transferCommandPool, 
                                          aiNode *node, 
                                          const aiScene *scene,
                                          int matToTex);

        static Mesh LoadMesh(VkPhysicalDevice newPhysicalDevice, 
                             VkDevice newDevice, 
                             VkQueue transferQueue, 
                             VkCommandPool transferCommandPool, 
                             aiMesh *mesh, 
                             const aiScene *scene,
                             int matToTex);
    private:
        std::vector<Mesh> meshList;
        glm::mat4 model;
};