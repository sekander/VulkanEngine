#pragma once

#include "state.h"
#include "../GameData/GameData.h"

class GameOverState : public State {

    public :
        GameOverState( GameData& data);


        void Init();
        void Input(float delta);
        void Update(float delta);
        void Render(float delta);
        void SoundUpdate();
        void DeleteData();
        ~GameOverState();

    private :
        //GameDataRef _data;
        GameData* _data;
		
	float x_cam, y_cam, z_cam, r_cam;
    float go_fade_in = 1.0f;
    float go_fade_out = 0.0f;
};
