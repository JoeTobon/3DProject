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
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
	Entity *ent1, *ent2, *ent3, *world;
    
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
		
	main_menu();

    // main game loop
    slog("gf3d main loop begin");	

	ent1 = entity_load("cube");
	ent1->update = &player_update;
	ent1->position.x = 8;

	ent2 = entity_load("cube");

	ent3 = entity_load("cube");
	ent3->position.x = -10;

	world = entity_load("cube");
	world->position.z = -10;
	world->scale.x = 100;
	world->scale.y = 100;
	world->scale.z = 4;

    while(!done)
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

			enemy_approach(ent1, ent2);

			enemy_ranged(ent1, ent3);

			if (entity_collsion(ent1, ent2))
			{
				entity_delete(ent2);
			}
            
        gf3d_command_rendering_end(commandBuffer);
        gf3d_vgraphics_render_end(bufferFrame);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
