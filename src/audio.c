#include <SDL.h>
#include <string.h>
#include "stdio.h"
#include "simple_logger.h"
#include "audio.h"

//Used to manage all the sounds in the game
typedef struct
{
	//Sound list
	Uint32 maxSounds;
	Sound *soundList;

	//Music list
	Uint32 maxMusic;
	Music *musicList;
} AudioManager;

static AudioManager audio_manager = { 0 };

//MANAGER
void audio_system_close()
{
	if (audio_manager.soundList != NULL)
	{
		sound_delete_all();
		free(audio_manager.soundList);
	}

	if (audio_manager.musicList != NULL)
	{
		music_delete_all();
		free(audio_manager.musicList);
	}

	memset(&audio_manager, 0, sizeof(AudioManager));

	slog("Audio system is closed.");
}

void audio_system_init(Uint32 maxSounds, Uint32 channels, Uint32 channelGroups, Uint32 maxMusic, Uint8 enableMP3, Uint8 enableOgg)
{
	int flags = 0;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		slog("Failed to open audio: %s\n", SDL_GetError());
		return;
	}

	if (enableMP3)
	{
		flags |= MIX_INIT_MP3;
	}

	if (enableOgg)
	{
		flags |= MIX_INIT_OGG;
	}

	/*if(!(Mix_Init(flags) & flags))
	{
		slog("Failed to initialize some audio support : %s", SDL_GetError());
	}*/

	//Initialize list for sounds
	if (maxSounds <= 0 || maxMusic <= 0)
	{
		slog("Can't initialize a list of zero sounds or musics");
		return;
	}

	//Initialize soundList and musicList
	memset(&audio_manager, 0, sizeof(AudioManager));
	audio_manager.soundList = (Sound *)malloc(sizeof(Sound)*maxSounds);
	audio_manager.musicList = (Music *)malloc(sizeof(Sound)*maxSounds);

	if (!audio_manager.soundList || !audio_manager.musicList)
	{
		slog("Failed to allocate sound and or music list");
		audio_system_close();
		return;
	}

	memset(audio_manager.soundList, 0, sizeof(Sound)*maxSounds);
	audio_manager.maxSounds = maxSounds;

	memset(audio_manager.musicList, 0, sizeof(Music)*maxMusic);
	audio_manager.maxMusic = maxMusic;

	atexit(audio_system_close);
	atexit(Mix_CloseAudio);
	atexit(Mix_Quit);

	slog("Audio system initialized");
}

//SOUND
Sound *sound_new(char *filename, int loop, int channel)
{
	int i;

	if (!filename)
	{
		return;
	}

	//search through the sound list for an unused address
	for (i = 0; i < audio_manager.maxSounds; i++)
	{
		if (audio_manager.soundList[i].inuse == 0)
		{
			sound_delete(&audio_manager.soundList[i]); //cleans up old data
			audio_manager.soundList[i].inuse = 1; //Set ref count to 1. Address is now in use

			//Initialize various attributes of audio here
			audio_manager.soundList[i].filename = filename;
			audio_manager.soundList[i].loop = loop;
			audio_manager.soundList[i].channel = channel;

			audio_manager.soundList[i].chunk = Mix_LoadWAV(audio_manager.soundList[i].filename);

			return &audio_manager.soundList[i];
		}
	}

	slog("error: out of addresses for sounds");
	exit(0);
	return NULL;
}

void sound_delete(Sound *sound)
{
	if (!sound)
	{
		return;
	}

	sound->inuse = 0;

	Mix_FreeChunk(sound->chunk);
	memset(sound, 0, sizeof(Sound));//clean up all other data
}

void sound_play(Sound *sound)
{
	if (!sound)
	{
		return;
	}

	Mix_PlayChannel(sound->channel, sound->chunk, sound->loop);
}

void sound_delete_all()
{
	int i;

	for (i = 0; i < audio_manager.maxSounds; i++)
	{
		sound_delete(&audio_manager.soundList[i]);
	}
}

//MUSIC
Music *music_new(char *filename, int loops)
{
	int i;

	if (!filename)
	{
		return;
	}

	for (i = 0; i < audio_manager.maxMusic; i++)
	{
		if (audio_manager.musicList[i].inuse == 0)
		{
			music_delete(&audio_manager.musicList[i]); //cleans up old data

			//initialize various attributes for music
			audio_manager.musicList[i].inuse = 1;
			audio_manager.musicList[i].loop = loops;
			audio_manager.musicList[i].filename = filename;

			audio_manager.musicList[i].music = Mix_LoadMUS(audio_manager.musicList[i].filename);

			return &audio_manager.musicList[i];
		}
	}

	slog("Out of addresses for music");
	exit(0);
	return NULL;
}

void music_delete(Music *music)
{
	if (!music)
	{
		return;
	}

	music->inuse = 0;
	Mix_FreeMusic(music->music);

	memset(music, 0, sizeof(Music));//clean up all other data
}

void music_play(Music *music)
{
	if (!music)
	{
		return;
	}

	Mix_PlayMusic(music->music, music->loop);
}

void music_delete_all()
{
	int i;

	for (i = 0; i < audio_manager.maxMusic; i++)
	{
		music_delete(&audio_manager.musicList[i]);
	}
}
