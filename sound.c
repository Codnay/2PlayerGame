#include <sound.h>

#include <SDL.h>
#include <SDL_image.h>
// #include <SDL_mixer.h>
#include<SDL2/SDL_mixer.h>
#include <stdio.h>
// #include <string>


//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gCar = NULL;
// Mix_Chunk *gHigh = NULL;
// Mix_Chunk *gMedium = NULL;
// Mix_Chunk *gLow = NULL;

int init()
{
	//Initialization flag
	int success = 1;

	 //Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		success = 0;
	}

	return success;
}

int loadMedia()
{
	//Loading success flag
	int success = 1;

	//Load music
	gMusic = Mix_LoadMUS( "resources/background.wav" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = 0;
	}
	
	//Load sound effects
	gCar = Mix_LoadWAV( "resources/car2.wav" );
	if( gCar == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = 0;
	}
	

	Mix_PlayMusic( gMusic, -1 );

	return success;
}
int loadMedia2()
{
	//Loading success flag
	int success = 1;

	//Load music
	gMusic = Mix_LoadMUS( "resources/finish.wav" );
	if( gMusic == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = 0;
	}
	
	//Load sound effects
	gCar = Mix_LoadWAV( "resources/car2.wav" );
	if( gCar == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = 0;
	}
	

	Mix_PlayMusic( gMusic, -1 );

	return success;
}

void close_sound()
{
	// //Free loaded images
	// gPromptTexture.free();

	//Free the sound effects
	Mix_FreeChunk( gCar );
	// Mix_FreeChunk( gHigh );
	// Mix_FreeChunk( gMedium );
	// Mix_FreeChunk( gLow );
	gCar = NULL;
	// gHigh = NULL;
	// gMedium = NULL;
	// gLow = NULL;
	
	//Free the music
	Mix_FreeMusic( gMusic );
	gMusic = NULL;

	//Destroy window	
	// SDL_DestroyRenderer( gRenderer );
	// SDL_DestroyWindow( gWindow );
	// gWindow = NULL;
	// gRenderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	// IMG_Quit();
	// SDL_Quit();
}

void start(SDL_Event e)
{
	if( e.type == SDL_KEYDOWN )
	{
		switch( e.key.keysym.sym )
		{
			//Play high sound effect
			case SDLK_LEFT:
			Mix_PlayChannel( -1, gCar, 0 );
			break;
			
			//Play medium sound effect
			case SDLK_RIGHT:
			Mix_PlayChannel( -1, gCar, 0 );
			break;
			
			//Play low sound effect
			case SDLK_UP:
			Mix_PlayChannel( -1, gCar, 0 );
			break;
			
			//Play scratch sound effect
			case SDLK_DOWN:
			Mix_PlayChannel( -1, gCar, 0 );
			break;
			
		}
	}

	return;
}
