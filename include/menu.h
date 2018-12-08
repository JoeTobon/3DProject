#ifndef __menu_h__
#define __menu_h__

#include <SDL.h>
#include "entity.h"
#include "audio.h"
#include "gf3d_types.h"

/**
 * @brief Will handle the processes and drawing of the main menu
 */
void main_menu();

/**
 * @brief  Will handle all the processes and drawing of the control screen
 */
void control_screen();

/**
* @brief  Will handle all the processes and drawing of the pause menu
*/
void pause_menu();

#endif