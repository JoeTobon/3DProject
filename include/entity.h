#ifndef __entity_h__
#define __entity_h__

#include <SDL.h>
#include "gf3d_model.h"
#include "gf3d_types.h"


typedef struct Entity_S
{
	Bool inuse;

	Uint64 id;

	//Graphics
	Model *model;

	//physics 
	Vector3D position;
	Vector3D velocity;

	Uint32 bufferFrame;


	//Entity specific functions
	void(*update)(struct Entity_S *self);

}Entity;

/**
 * @brief initializes the entity manager/system
 * @param maxNum the maximum number of entities the system can handle
 */
void entity_system_init(Uint32 maxNum);

/**
 * @brief initialize a new entity
 * @return pointer to entity or null on error
 */
Entity *entity_new();

/**
 * @brief free an entity back to the entity manager. Stays in memory until the space is needed
 * @param entity the entity to free
 */
void entity_free(Entity *entity);

/**
 * @brief draws entity to screen
 * @param entity the entity to draw
 */
void entity_draw(Entity *entity);

/**
 * @brief draws all entities in use to the screen
 */
void entity_draw_all();

/**
 * @brief deletes/removes an enity from the entity system
 * @param entity a pointer to the entity that is to be deleted
 */
 void entity_delete(Entity *entity);

 /**
  * @brief deletes all loaded entities from memory does not close entity system
  */
 void entity_delete_all();

 /**
  * @brief update all entities with update functions
  */
 void entity_update_all();

#endif