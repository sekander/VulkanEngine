/*********************************************************************************
*                             Author: Nahid Sekander                             *
*       File Name: /home/fnky/C0de/new_multi_component_engine/src/main.cpp       *
*                   Creation Date: September 29, 2022 10:38 PM                   *
*                   Last Updated: September 30, 2022 01:30 AM                    *
*                              Source Language: cpp                              *
*                                                                                *
*                            --- Code Description ---                            *
*                                  main source                                   *
*********************************************************************************/
#include "../include/Window/game.h"
#include <thread>
#include <memory>


#include "../include/GameData/ObjectCounter.h"
int ObjectCounter::ai_component_counter; 
int ObjectCounter::physic_component_counter; 
int ObjectCounter::render_component_counter; 
int ObjectCounter::transform_component_counter; 
int ObjectCounter::npc_actor_counter; 
int ObjectCounter::texture_actor_counter; 
int ObjectCounter::projectile_actor_counter; 
int ObjectCounter::edge_actor_counter; 
int ObjectCounter::animation_actor_counter; 
float ObjectCounter::verticies_counter;


int main(void)
{
	std::shared_ptr<Game> g;
	g = std::make_shared<Game>();
    
    
	//Game *g;
	//g = new Game();

		g->Initialize();
           
		
		std::thread t0([=]() {
			g->NetworkUpdate();
		});
        
		
		std::thread t1([=]() {
			g->SoundUpdate();
		});

		std::thread t2([=]() {
			g->Update();
		});
		         
                            
		g->Render();
		t0.join();
		t1.join();
		t2.join();



//	delete g;
		std::cout << "Total Number of Textures " << ObjectCounter::texture_actor_counter << std::endl;
		std::cout << "Total Number of NPC " << ObjectCounter::npc_actor_counter << std::endl;
		std::cout << "Total Number of Animator " << ObjectCounter::animation_actor_counter << std::endl;
		std::cout << "Total Number of Projectile " << ObjectCounter::projectile_actor_counter << std::endl;
		
		std::cout << "Total Number of RenderComponents " << ObjectCounter::render_component_counter << std::endl;
		std::cout << "Total Number of TransformComponents " << ObjectCounter::transform_component_counter << std::endl;
		std::cout << "Total Number of PhysicsComponents " << ObjectCounter::physic_component_counter << std::endl;
		std::cout << "Total Number of AI Componets " << ObjectCounter::ai_component_counter << std::endl;
		
		std::cout << "APPLICATION CLOSED" <<  std::endl;

		//t0.detach();
		//t1.detach();
		//t2.detach();






		return 0;
}
