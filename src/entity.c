#include "entity.h"
#include "simple_logger.h"


typedef struct
{
	Uint32 maxEnt;
	Entity *entList;
	Uint64 increment;
}EntityManager;

static EntityManager entity_manager = {0};

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

void entity_draw(Entity *entity)
{
	if (!entity)
	{
		slog("Error: Cannot draw an entity that does not exist");
		return;
	}

	//gf3d_model_draw(entity->model, entity->bufferFrame, commandBuffer);
}

void entity_draw_all()
{
	int i;

	for (i = 0; i < entity_manager.maxEnt; i++)
	{
		if (entity_manager.entList[i].inuse)
		{
			entity_draw(&entity_manager.entList[i]);
		}
	}
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
