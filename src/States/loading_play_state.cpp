//////////////////////////////////////////////////////////////////////////////
///------------------------Component System---------------------------------


#include "../../include/GameData/GameData.h"
#include "../../include/States/loading_play_state.h"


#include <cstdio>
#include <math.h>
#include <memory>


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;





LoadingPlayState::LoadingPlayState( GameData& data)
{

	_data = std::move(&data);

    printf("LOADING State!\n");


}


LoadingPlayState::~LoadingPlayState()
{
    printf("Deleting LOADING State\n");
//    DeleteData();
}

void LoadingPlayState::DeleteData()
{
//    delete _data.get()->ld;
    printf("Deleting LOADING DELETE DATA State\n");

}

void LoadingPlayState::Init()
{
    printf("Hello LOADING State\n");
    finishedLoading = false;


    _data->pd.A = 0;
    _data->pd.B = 0;
    _data->pd.X = 0;
    _data->pd.Y = 0;
    _data->pd.dpadD = 0;
    _data->pd.dpadU = 0;
    _data->pd.dpadL = 0;
    _data->pd.dpadR = 0;
    _data->pd.select = 0;
    _data->pd.start = 0;
    _data->pd.L = 0;
    _data->pd.R = 0;
    _data->pd.L2 = 0;
    _data->pd.R2 = 0;

    _data->pd.touchPad_1 = 0;
    _data->pd.touchPad_2 = 0;
    _data->pd.touchPad_3 = 0;
    _data->pd.touchPad_4 = 0;
    _data->pd.touchPad_5 = 0;
    _data->pd.touchPad_6 = 0;
    _data->pd.touchPad_7 = 0;
    _data->pd.touchPad_8 = 0;
    _data->pd.touchPad_9 = 0;
    _data->pd.touchPad_10 = 0;
    _data->pd.touchPad_11 = 0;
    _data->pd.touchPad_12 = 0;
    _data->pd.touchPad_13 = 0;

}

void LoadingPlayState::SoundUpdate()
{
    //printf("Hello Game Over Sound UPdate\n");
}

void LoadingPlayState::Input(float delta)
{
    //printf("Main Menu Input \n");
}

void LoadingPlayState::Update(float delta)
{
    if(!finishedLoading){

        finishedLoading = true;
    }
    _data->state_switch = PLAY_STATE;
}

void LoadingPlayState::Render(float delta)
{
   // printf("Main Menu Render\n");
}
