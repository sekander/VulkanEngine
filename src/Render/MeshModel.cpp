#include "../../include/Render/MeshModel.h"

#include <iostream>

MeshModel::MeshModel(std::vector<Mesh> newMeshList)
{
    meshList = newMeshList;
    model = glm::mat4(1.0f);
}


std::vector<std::string> MeshModel::LoadMaterials(const aiScene* scene)
{
    //Create 1:1 sized list of textures
    std::vector<std::string> textureList(scene->mNumMaterials);
    /*
    //Go through each material and copy its texture file name (if it exists)
    for(size_t i = 0; i < scene->mNumMaterials; i++)
    {
        //Get the material
        aiMaterial * material = scene->mMaterials[i];
        //Initialize the texture to empty string (will be replaced if texture exists)
        textureList[i] = "";

        std::cout <<"T:" << textureList[i] << std::endl;


        //Check for a Diffuse Texture (standard detail texture)
        if(material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            std::cout << i << std::endl;
            //Get the path of the texture file
            aiString path;
            if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                //Cut off any directory information already present
                //For microsoft only
                //int idx = std::string(path.data).rfind("\\");

                std::string fileName = std::string(path.data);

                textureList[i] = fileName;
                std::cout << "Tex: " << fileName << std::endl;
            }
        }
    }
    */

    return textureList;
}

std::vector<Mesh> MeshModel::LoadNode(VkPhysicalDevice newPhysicalDevice, 
                                          VkDevice newDevice, 
                                          VkQueue transferQueue, 
                                          VkCommandPool transferCommandPool, 
                                          aiNode *node, 
                                          const aiScene *scene,
                                          //std::vector<int> matToTex)
                                          int matToTex)
{
    std::vector<Mesh> meshList;

    //Go through each mesh at this node and create it, then add it to our meshList
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        //Load Mesh Here
        meshList.push_back(LoadMesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, scene->mMeshes[node->mMeshes[i]], scene, matToTex));
    }

    //Go through each node attached to this node and load it, then append their meshes to this node's mesh list
    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        std::vector<Mesh> newList = LoadNode(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, node->mChildren[i], scene, matToTex);
        meshList.insert(meshList.end(), newList.begin(), newList.end());
    }


    return meshList;
}

Mesh MeshModel::LoadMesh(VkPhysicalDevice newPhysicalDevice, 
                             VkDevice newDevice, 
                             VkQueue transferQueue, 
                             VkCommandPool transferCommandPool, 
                             aiMesh *mesh, 
                             const aiScene *scene,
                             int matToTex)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    //Resize vertex list to hold all vertices for mesh
    vertices.resize(mesh->mNumVertices);    

    for(size_t i = 0; i < mesh->mNumVertices; i++)
    {
        //Set Position
        vertices[i].pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        //Set tex coords (if they exist)
        if(mesh->mTextureCoords[0])
        {
            vertices[i].tex = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }
        else
        {
            vertices[i].tex = {0.0f, 0.0f};
        }

        vertices[i].normVert = glm::normalize( vertices[i].pos);
        //CHANGE HERE to find the norm vert
    }


    //Iterate over indices through faces and copy across
    for(size_t i =0; i < mesh->mNumFaces; i++)
    {
        //Get a face
        aiFace face = mesh->mFaces[i];

        //Go through face's indices and add to list
        for(size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    //Create new mesh with details and return it
    Mesh newMesh = Mesh(newPhysicalDevice, newDevice, transferQueue, transferCommandPool, &vertices, &indices, matToTex);

    return newMesh;
}
