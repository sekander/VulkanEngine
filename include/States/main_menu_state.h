#pragma once

#include "state.h"
#include "../GameData/GameData.h"

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

        ~MainMenuState();
    private :
        //GameDataRef _data;
        GameData* _data;

        

	float x_cam, y_cam, z_cam, r_cam;
    float fade = 1.0f;
};
