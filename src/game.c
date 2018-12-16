#include <SDL.h>  
#include <SDL_ttf.h>

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_camera.h"
#include "gf3d_vector.h"
#include "gf3d_texture.h"
#include "entity.h"
#include "audio.h"

int main(int argc,char *argv[])
{
    int done = 0;
	int cont = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
	Entity *player, *en1, *en2, *world;
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1200,                   //screen width
        700,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        1                       //validation
    );
    
	entity_system_init(1000);
	audio_system_init(256, 16, 4, 1, 1, 1);
		
	cont = main_menu();

    // main game loop
    slog("gf3d main loop begin");	

	//player
	player = entity_load("cube");
	player->update = &player_update;
	player->position.x = 8;

	//Enemies
	en1 = entity_load("cube");

	en2 = entity_load("cube");
	en2->position.x = -10;

	//items


	//world
	world = entity_load("floor");
	world->position.z = -10;
	world->scale.x = 100;
	world->scale.y = 100;
	world->scale.z = 4;

	if (cont == 0)
	{
		while (!done)
		{
			SDL_PumpEvents();   // update SDL's internal event structures
			keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
			//update game things here

			gf3d_vgraphics_rotate_camera(0.01);

			// configure render command for graphics command pool
			// for each mesh, get a command and configure it from the pool
			bufferFrame = gf3d_vgraphics_render_begin();
			commandBuffer = gf3d_command_rendering_begin(bufferFrame);

			entity_draw_all(bufferFrame, commandBuffer);

			entity_update_all();

			enemy_approach(player, en1);

			enemy_ranged(player, en2);

			if (entity_collsion(player, en1))
			{
				entity_delete(en1);
			}

			gf3d_command_rendering_end(commandBuffer);
			gf3d_vgraphics_render_end(bufferFrame);

			if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
		}
	}
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
