#pragma once

#include "state.h"
#include "../GameData/GameData.h"

class LoadingPlayState : public State {

    public :
        LoadingPlayState( GameData& data);


        void Init();
        void Input(float delta);
        void Update(float delta);
        void Render(float delta);
        void SoundUpdate();
        void DeleteData();
        ~LoadingPlayState();

    private :
        //GameDataRef _data;
        GameData* _data;
		

        bool finishedLoading;
};
