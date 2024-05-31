#pragma once

#include "state.h"
#include "../GameData/GameData.h"

class TemplateState : public State {

    public :
        TemplateState(GameData& data);


        void Init();
        void Input(float delta);
        void Update(float delta);
        void Render(float delta);
        void SoundUpdate();
        void DeleteData();
        

        void printMat4(const glm::mat4& mat) {
            const char* labels[4][4] = {
                {"Rotation/Scale (0,0): ", "Rotation/Scale (0,1): ", "Rotation/Scale (0,2): ", "Translation (0,3): "},
                {"Rotation/Scale (1,0): ", "Rotation/Scale (1,1): ", "Rotation/Scale (1,2): ", "Translation (1,3): "},
                {"Rotation/Scale (2,0): ", "Rotation/Scale (2,1): ", "Rotation/Scale (2,2): ", "Translation (2,3): "},
                {"Perspective (3,0): ", "Perspective (3,1): ", "Perspective (3,2): ", "Perspective (3,3): "}
            };

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::cout << labels[i][j] << mat[i][j] << '\n';
                }
            }
        }

        
        ~TemplateState();

    private :
        //GameDataRef _data;
        GameData* _data;
		
	float x_cam, y_cam, z_cam, r_cam;
};
