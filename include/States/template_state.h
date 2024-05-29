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
        ~TemplateState();

    private :
        //GameDataRef _data;
        GameData* _data;
		
	float x_cam, y_cam, z_cam, r_cam;
};
