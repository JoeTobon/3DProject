#include "entity.h"
#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include <SDL.h>


typedef struct
{
	Uint32 maxEnt;
	Entity *entList;
	Uint64 increment;
}EntityManager;

static EntityManager entity_manager = {0};

const Vector3D AXIS_X = { 1, 0, 0 };
const Vector3D AXIS_Y = { 0, 1, 0 };
const Vector3D AXIS_Z = { 0, 0, 1 };

void entity_close()
{
	if (entity_manager.entList != NULL)
	{
		entity_delete_all();
		free(entity_manager.entList);
	}

	memset(&entity_manager, 0, sizeof(EntityManager));

	slog("Entity system closed");
}

void entity_system_init(Uint32 maxEnt)
{
	if (maxEnt <= 0)
	{
		slog("Cannot initialize an entity manager for zero entities!");
		return;
	}

	memset(&entity_manager, 0, sizeof(EntityManager));
	entity_manager.entList = (Entity *)malloc(sizeof(Entity)*maxEnt);

	if (!entity_manager.entList)
	{
		slog("Failed to allocate entity list");
		entity_close();
		return;
	}

	memset(entity_manager.entList, 0, sizeof(Entity)*maxEnt);
	entity_manager.maxEnt = maxEnt;

	atexit(entity_close);
	slog("Entity system intialized");
}

Entity *entity_new()
{
	int i;

	for (i = 0; i < entity_manager.maxEnt; i++)
	{
		if (entity_manager.entList[i].inuse == 0)
		{
			memset(&entity_manager.entList[i], 0, sizeof(Entity));
			entity_manager.entList[i].id = entity_manager.increment++;
			entity_manager.entList[i].inuse = 1;
			entity_manager.entList[i].scale = 1;

			return &entity_manager.entList[i];
		}
	}

	slog("Error: out of entity addresses");
	return NULL;
}

void entity_free(Entity *entity)
{
	if (!entity)
	{
		slog("Error: Cannot free an entity that does not exist");

		return;
	}

	entity->inuse = 0;
}

void entity_draw(Entity *entity, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	if (!entity)
	{
		slog("Error: Cannot draw an entity that does not exist");
		return;
	}

	entity_set_draw_ubo(entity);
	gf3d_vgraphics_update_ubo(&entity->ubo, gf3d_vgraphics_uniform_buffer_get_index(entity->model->ubo));
	gf3d_model_draw(entity->model, bufferFrame, commandBuffer);
}

void entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	int i;

	for (i = 0; i < entity_manager.maxEnt; i++)
	{
		if (entity_manager.entList[i].inuse)
		{
			entity_draw(&entity_manager.entList[i], bufferFrame, commandBuffer);
		}
	}
}

Entity *entity_load(char *filename)
{
	Entity *entity;

	entity = entity_new();

	if (!entity)
	{
		slog("Error: Unable to load entity that does not exist");
		return;
	}

	entity->model = gf3d_model_load(filename);
	
	gf3d_matrix_identity(entity->ubo.model);
	gf3d_matrix_identity(entity->ubo.proj);
	gf3d_matrix_identity(entity->ubo.view);
	
	gf3d_matrix_view(
		entity->ubo.view,
		vector3d(2, 20, 2),
		vector3d(0, 0, 0),
		vector3d(0, 0, 1)
		);
	
	gf3d_matrix_perspective(
		entity->ubo.proj,
		45 * GF3D_DEGTORAD,
		1200 / (float)700,
		0.1f,
		100
		);

	entity->ubo.proj[1][1] *= -1;

	return entity;
}

void entity_delete(Entity *entity)
{
	if (!entity)
	{
		slog("Error: Cannot delete an entity that does not exist");
		return;
	}

	memset(entity, 0, sizeof(Entity));//clean up all other data
}

void entity_delete_all()
{
	int i;

	for (i = 0; i < entity_manager.maxEnt; i++)
	{
		entity_delete(&entity_manager.entList[i]);
	}
}

void entity_set_draw_ubo(Entity *entity)
{
	if (!entity)
	{
		return;
	}

	//Position
	entity->ubo.model[3][0] = entity->position.x;
	entity->ubo.model[3][1] = entity->position.y;
	entity->ubo.model[3][2] = entity->position.z;

	//Rotation
	gf3d_matrix_rotate(entity->ubo.model, entity->ubo.model, entity->relative_rotation.x, AXIS_X);
	gf3d_matrix_rotate(entity->ubo.model, entity->ubo.model, entity->relative_rotation.y, AXIS_Y);
	gf3d_matrix_rotate(entity->ubo.model, entity->ubo.model, entity->relative_rotation.z, AXIS_Z);

	memset(&entity->relative_rotation, 0, sizeof(Vector3D));

	//Scaling
	entity->ubo.model[0][0] *= entity->scale;
	entity->ubo.model[1][1] *= entity->scale;
	entity->ubo.model[2][2] *= entity->scale;
	entity->scale = 1;
}

void player_update(Entity *entity)
{
	const Uint8 *keys;
	SDL_Event e;

	keys = SDL_GetKeyboardState(NULL);

	if (!entity)
	{
		return;
	}

	if (!entity->inuse)
	{
		return;
	}

	//movement
	if (keys[SDL_SCANCODE_W])
	{
		entity->position.y -= 0.5;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		entity->position.y += 0.5;
	}
	else if (keys[SDL_SCANCODE_A])
	{
		entity->position.x += 0.5;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		entity->position.x -= 0.5;
	}

	//rotation
	if (keys[SDL_SCANCODE_X])
	{
		entity->relative_rotation.x += 0.05;
	}
	else if (keys[SDL_SCANCODE_Y])
	{
		entity->relative_rotation.y += 0.05;
	}
	else if (keys[SDL_SCANCODE_Z])
	{
		entity->relative_rotation.z += 0.05;
	}

	//Keeping track of overall rotation
	vector3d_add(entity->rotation, entity->relative_rotation, entity->rotation);  

	//scaling
	if (keys[SDL_SCANCODE_J])
	{
		entity->scale += 0.05;
	}
	else if (keys[SDL_SCANCODE_K])
	{
		entity->scale -= 0.05;
	}
}

void entity_update_all()
{
	int i;

	for (i = 0; i < entity_manager.maxEnt; i++)
	{
		if (entity_manager.entList[i].inuse == 0)
		{
			continue;
		}

		//if entity has no update function go to next entity
		if (!entity_manager.entList[i].update)
		{
			continue;
		}

		entity_manager.entList[i].update(&entity_manager.entList[i]);
	}
}
