#pragma once

#include "state.h"
#include "../GameData/GameData.h"

#include "../../include/IMGUI/imgui.h"

#include <chrono>
#include <unordered_map>
#include <vector>


#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class MainMenuState : public State {

    public :
        //MainMenuState(GameDataRef& data);
        MainMenuState(GameData& data);

        void Init();
        void Input(float delta);
        void Update(float delta);
        void Render(float delta);
        void SoundUpdate();
        void DeleteData();
        

        std::string matrixData;

        // Delay time in milliseconds
        const long long KEY_DELAY = 200;

        std::unordered_map<int, long long> lastKeyPressTime;

        void _ui();

        ~MainMenuState();
    private :
        //GameDataRef _data;
        GameData* _data;

        int quaternion_options = 1;
        std::string quaternion_title_options[9] = {"Unit quaternion", 
                                                   "Identity quaternion", 
                                                   "Conjugate quaternion",
                                                   "Inverse quaternion",
                                                   "Apply quaternion and its inverse to rotate a point",
                                                   "Two possible representations for the same rotation",
                                                   "Combining rotations",
                                                   "Associativity of quaternion multiplication",
                                                   "Non-commutativity of quaternion multiplication"};

        std::vector<glm::mat4> models;

        std::string demonstrateQuaternionProperties(int choice, glm::mat4& modelMatrix);

        std::string printMat4ToString(const glm::mat4& mat) {
            const char* labels[4][4] = {
                {"Rotation/Scale (0,0): ", "Rotation/Scale (0,1): ", "Rotation/Scale (0,2): ", "Translation (0,3): "},
                {"Rotation/Scale (1,0): ", "Rotation/Scale (1,1): ", "Rotation/Scale (1,2): ", "Translation (1,3): "},
                {"Rotation/Scale (2,0): ", "Rotation/Scale (2,1): ", "Rotation/Scale (2,2): ", "Translation (2,3): "},
                {"Perspective (3,0): ", "Perspective (3,1): ", "Perspective (3,2): ", "Perspective (3,3): "}
            };
            std::string result;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result += labels[i][j] + std::to_string(mat[i][j]) + '\n';
                }
            }
            return result;
        }
        // Function to create and return a string representation of a quaternion
        std::string printQuaternion(const glm::quat& q, const std::string& name) {
            std::string result = name + ": (" + std::to_string(q.w) + ", " +
                         std::to_string(q.x) + ", " + std::to_string(q.y) + ", " + 
                         std::to_string(q.z) + ")\n";
            
            std::cout << name << ": (" 
              << q.w << ", " 
              << q.x << ", " 
              << q.y << ", " 
              << q.z << ")" << std::endl;
            
            
            return result;
        }

        // Function to print a matrix
        std::string printMatrix(const glm::mat4& mat, const std::string& name) {
            std::string result = name + ":\n";
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result += std::to_string(mat[i][j]) + " ";
                }
                result += "\n";
            }
            return result;
        }

//         void printMat4(const glm::mat4& mat) {
//             const char* labels[4][4] = {
//                 {"Rotation/Scale (0,0): ", "Rotation/Scale (0,1): ", "Rotation/Scale (0,2): ", "Translation (0,3): "},
//                 {"Rotation/Scale (1,0): ", "Rotation/Scale (1,1): ", "Rotation/Scale (1,2): ", "Translation (1,3): "},
//                 {"Rotation/Scale (2,0): ", "Rotation/Scale (2,1): ", "Rotation/Scale (2,2): ", "Translation (2,3): "},
//                 {"Perspective (3,0): ", "Perspective (3,1): ", "Perspective (3,2): ", "Perspective (3,3): "}
//             };
//             for (int i = 0; i < 4; ++i) {
//                 for (int j = 0; j < 4; ++j) {
//                     std::cout << labels[i][j] << mat[i][j] << '\n';
//                 }
//             }
//         }


        // Function to get the current time in milliseconds
        inline long long currentTimeMillis() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count();
        }

        void ImGui_PrintMat4(const glm::mat4& mat) {
            const char* labels[4][4] = {
                {"Rotation/Scale (0,0): ", "Rotation/Scale (0,1): ", "Rotation/Scale (0,2): ", "Translation (0,3): "},
                {"Rotation/Scale (1,0): ", "Rotation/Scale (1,1): ", "Rotation/Scale (1,2): ", "Translation (1,3): "},
                {"Rotation/Scale (2,0): ", "Rotation/Scale (2,1): ", "Rotation/Scale (2,2): ", "Translation (2,3): "},
                {"Perspective (3,0): ", "Perspective (3,1): ", "Perspective (3,2): ", "Perspective (3,3): "}
            };

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    ImGui::Text("%s %.3f", labels[i][j], mat[i][j]);
                }
            }
        }

        

	float x_cam, y_cam, z_cam, r_cam;
    float xpos, ypos, zpos;
    float ui_rotation_control_x;
    float ui_rotation_control_y;
    float ui_rotation_control_z;
    float fade = 1.0f;
};
