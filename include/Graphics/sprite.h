#include <cstdint>
#include <stdio.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include "texture.h"
#include <memory>
#include <string>
#include <unordered_map>

/*
 * Handles texture drawing space and animation per object
 *
 * */
using glm::vec3;
using glm::vec2;
class sprite{

	public:

		sprite(){};
		sprite(const char* fileName,
		       vec2 position, 
		       vec2 size, 
		       //unsigned int x_frame = 0, 
		       uint8_t x_frame = 0, 
		       //unsigned int y_frame = 0, 
		       uint8_t y_frame = 0, 
		       float rotation = 0.0f, 
		       vec3 colour = vec3(1.0f), 
		       vec2 frames = vec2(1.0f),
			   bool isAlpha = true);
		
		~sprite(){
//			printf("Deleting Sprite\n");
		};

	std::shared_ptr<Texture2D> loadSmartTexture(const char* fileName, bool alapha);
	std::shared_ptr<Texture2D> get_smart_texture(){return smart_texture;}	

	void set_texture(std::shared_ptr<Texture2D> texture){smart_texture = texture;}

	
	std::string get_key(){return m_key;}
	bool get_sprite_sheet(){return m_sprite_sheet;}
	bool get_change_frames(){return m_change_frames;}
	uint8_t get_num_x_frames(){return m_x_num_frame;}
	//unsigned int get_num_x_frames(){return m_x_num_frame;}
	uint8_t get_num_y_frames(){return m_y_num_frame;}
	//unsigned int get_num_y_frames(){return m_y_num_frame;}
	float get_incrementer(){return m_incrementer;}
	vec2 get_position(){return m_position;}
	vec2 get_size(){return m_size;}
	float get_rotation(){return m_rotate;}
	vec3 get_color(){return m_color;}
	vec2 get_frame(){return m_frames;}
	
	void set_sprite_sheet(bool sprite_sheet){m_sprite_sheet = sprite_sheet;}
	void set_change_frames(bool change_frames){m_change_frames = change_frames;}
	//void set_num_x_frames(unsigned int num_x_frames){m_x_num_frame = num_x_frames;}
	void set_num_x_frames(uint8_t num_x_frames){m_x_num_frame = num_x_frames;}
	//void set_num_y_frames(unsigned int num_y_frames){m_y_num_frame = num_y_frames;}
	void set_num_y_frames(uint8_t num_y_frames){m_y_num_frame = num_y_frames;}
	void set_incrementer(float incrementer){m_incrementer = incrementer;}
	void set_position(vec2 position){m_position = position;}
	void set_size(vec2 size){m_size = size;}
	void set_rotation(float rotation){m_rotate = rotation;}
	void set_color(vec3 color){m_color = color;}
	void set_frames(vec2 frame){m_frames = frame;}




	private:

		std::shared_ptr<Texture2D> smart_texture;	
		std::unordered_map<std::shared_ptr<Texture2D>, std::string> texture_list;
		std::string m_key;
		bool m_sprite_sheet;
		bool m_change_frames;
		//unsigned int m_x_num_frame;
		uint8_t m_x_num_frame;
		//unsigned int m_y_num_frame;
		uint8_t  m_y_num_frame;
		float m_incrementer;
		vec2 m_position;
		vec2 m_size;
		float m_rotate;
		vec3 m_color;
		vec2 m_frames;




};
