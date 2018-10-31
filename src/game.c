#include <SDL.h>            

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

int main(int argc,char *argv[])
{
    int done = 0;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;

	Entity *ent1;
	Entity *ent2;

	//movement
	float move = 0.0f;
    
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
    
	entity_system_init(1024);
	
    // main game loop
    slog("gf3d main loop begin");

    ent1 = entity_load("agumon");
	ent2 = entity_load("cube");

	ent1->position.x -= 1;
	ent1->position.y -= 1;
	ent1->position.z -= 1;

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

            entity_draw(ent1, bufferFrame, commandBuffer);
			//entity_draw(ent2, bufferFrame, commandBuffer);

			if (keys[SDL_SCANCODE_K])
			{
				entity_draw(ent2, bufferFrame, commandBuffer);
			}

			if (keys[SDL_SCANCODE_L])
			{
				entity_delete(ent2, bufferFrame, commandBuffer);
			}

			if (keys[SDL_SCANCODE_W])
			{
				move = 2.0f;
			}
			else if (keys[SDL_SCANCODE_S])
			{
				move = -2.0f;
			}
			else if (keys[SDL_SCANCODE_A])
			{
				move = 1.0f;
			}
			else if (keys[SDL_SCANCODE_D])
			{
				move = -1.0f;
			}
			else
			{
				move = 0;
			}

        gf3d_command_rendering_end(commandBuffer);
        gf3d_vgraphics_render_end(bufferFrame, keys, move);

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
