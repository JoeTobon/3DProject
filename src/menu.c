#include "menu.h"

void main_menu()
{
	int menuBool = 1;
	float timer = 0;
	Bool stall = false;
	Bool p = true;
	Bool c = false;
	
	Uint32 bufferFrame = 0;
	VkCommandBuffer commandBuffer;
	const Uint8 * keys;

	//Menu specific graphics and sounds
	Entity *play, *controls;
	Music *gameM;

	//Play Game music
	gameM = music_new("audio/littletown.mid", -1);
	music_play(gameM);

	//Set menu graphics
	play = entity_load("play");
	play->scale = 2.5;
	play->position.x = 7;
	play->position.z = 2.5;

	controls = entity_load("controls");
	controls->scale = 2.5;
	controls->position.x = -7;

	while (menuBool)
	{
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

		if (stall == true)
		{
			timer += 0.1;
		}

		if (timer >= 2.0)
		{
			timer = 0; 
			stall = false;
		}

		bufferFrame = gf3d_vgraphics_render_begin();
		commandBuffer = gf3d_command_rendering_begin(bufferFrame);

			entity_draw_all(bufferFrame, commandBuffer);

			entity_update_all();

		gf3d_command_rendering_end(commandBuffer);
		gf3d_vgraphics_render_end(bufferFrame);

		if (keys[SDL_SCANCODE_LEFT] && stall == false && c == true)
		{
			p = true;
			c = false;

			play->position.z = 2.5;

			controls->position.z = 0;

			stall = true;
		}
		if (keys[SDL_SCANCODE_RIGHT] && stall == false && p == true)
		{
			c = true;
			p = false;

			play->position.z = 0;

			controls->position.z = 2.5;

			stall = true;
		}

		if (keys[SDL_SCANCODE_RETURN])menuBool = 0; // exit condition
	}

	entity_delete_all();
	music_delete_all();

	if (c == true)
	{
		control_screen();
	}

	//else exit loop and start main game loop
}

void control_screen()
{

}

void pause_menu()
{

}