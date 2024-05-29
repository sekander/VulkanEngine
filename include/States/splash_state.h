#pragma once
///------------------------Component System---------------------------------
#include "state.h"
#include <cstdint>
#include <ctime>
#include "../GameData/GameData.h"

class Actor;

class SplashState : public State {

    public :
        //SplashState(GameDataRef& data);
        SplashState(GameData& data);

        void Init();
        void Input(float delta  );
        void Update(float delta );
        void Render(float delta );
        void SoundUpdate();
        void DeleteData();

        ~SplashState();
    private :

	//GameDataRef _data;
	GameData* _data;


	clock_t timer = clock();
	double delay;
	double jump_delay;
	float counter;

	uint16_t platform_delay = 0;

        float x, y;
		float x_cam, y_cam, z_cam, r_cam;



};
