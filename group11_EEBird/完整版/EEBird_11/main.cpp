
//Using SDL, SDL_image, standard IO, vectors, and strings
#include "Header.h"
#include "LTexture.hpp"
#include "Barrier.hpp"
#include "Bird.h"
#include "Button.h"
#include "score.h"
#include "Start.h"
#include "highscore.h"
//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 680;
const int barrier_range = 360;
const int BIRD_XLOCATION = 200;
const int StartB_WIDTH = 300;
const int StartB_HEIGHT = 150;
const int StartB_PosX = SCREEN_WIDTH/2-StartB_WIDTH-20;
const int StartB_PosY = 200;
const int InfoBox_WIDTH = 400;
const int InfoBox_HEIGHT = 240;
const int InfoPosX = SCREEN_WIDTH/2 -InfoBox_WIDTH/2;
const int InfoPosY = SCREEN_HEIGHT/2 -InfoBox_HEIGHT/2;

//The background scrolling offset
int movetop10=400;
int highscore[10];
int scrollingOffset =0;
int move_start=700;
int move_EEBird=440;
int move_loseBird=440;
int move_restart=400;
void initBarrier();
Barrier barrier[5];
bool newhighornot=0;
//control the beat sound
bool playonce=true;
void initBarrier()
{
    for(int i=0;i<5;i++)
    {
        barrier[i].reset(i+1);
    }
}


bool infoAppear = false;
int CurrentInfo = 0;
int score = 0;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//temporary escape method
bool escape ( SDL_Event& e );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gStartBTexture;
LTexture gInfoBTexture;
LTexture gInfoTexture[3];
LTexture gCloseInfoTexture;
LTexture gBarrierTexture;
LTexture gBGTexture;
LTexture gBirdTexture[2];
LTexture gBBTexture; //button back
LTexture gButtonTexture[3];
TTF_Font *gFont = NULL;
LTexture gscore;
LTexture ghighscore[10];
LTexture gEEBird;
LTexture ghalftransparent;
LTexture gloseBird;
LTexture gtop10;
//The sound effects that will be used
Mix_Chunk *gbeat = NULL;
Mix_Chunk *gclick = NULL;


bool loadMedia()
{
	//Loading success flag
	bool success = true;

	gFont = TTF_OpenFont( "000/font.ttf", 72 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 0 };
        if( !gscore.loadFromRenderedText( "SCORE : "+scorestring(score), textColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }
        for (int i=0;i<10;i++)
        {
            if( !ghighscore[i].loadFromRenderedText( scorestring(highscore[i]), textColor ) )
            {
                printf( "Failed to render text texture!\n" );
                success = false;
            }
        }
    }

    //Load start scene textures
    if( !gtop10.loadFromFile( "000/top10.png" ) )
    {
        printf( "Failed to load start300_150 texture!\n" );
        success = false;
    }
    if( !gloseBird.loadFromFile( "000/loseBird2.png" ) )
    {
        printf( "Failed to load start300_150 texture!\n" );
        success = false;
    }
    if( !ghalftransparent.loadFromFile( "000/halftransparent.png" ) )
    {
        printf( "Failed to load start300_150 texture!\n" );
        success = false;
    }
    if( !gEEBird.loadFromFile( "000/EEbird.png" ) )
    {
        printf( "Failed to load start300_150 texture!\n" );
        success = false;
    }
    if( !gStartBTexture.loadFromFile( "000/start300_150.png" ) )
	{
		printf( "Failed to load start300_150 texture!\n" );
		success = false;
	}
	if( !gInfoBTexture.loadFromFile( "000/info300_150.png" ) )
	{
		printf( "Failed to load info300_150 texture!\n" );
		success = false;
	}
	if( !gInfoTexture[0].loadFromFile( "000/info1st_new400_240.png" ) )
	{
		printf( "Failed to load info1st400_240 texture!\n" );
		success = false;
	}
	if( !gInfoTexture[1].loadFromFile( "000/info2nd_new400_240.png" ) )
	{
		printf( "Failed to load info2nd400_240 texture!\n" );
		success = false;
	}
	if( !gInfoTexture[2].loadFromFile( "000/info3rd400_240.png" ) )
	{
		printf( "Failed to load info3rd400_240 texture!\n" );
		success = false;
	}
	if( !gCloseInfoTexture.loadFromFile( "000/closeinfo25_25.png" ) )
	{
		printf( "Failed to load closeinfo50_50 texture!\n" );
		success = false;
	}
	//Load barrier texture
	if( !gBarrierTexture.loadFromFile( "000/barrier.png" ) )
	{
		printf( "Failed to load barrier texture!\n" );
		success = false;
	}
	//Load background texture
	if( !gBGTexture.loadFromFile( "000/background new.jpg" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}
	//Load bird texture
    if( !gBirdTexture[0].loadFromFile( "000/birdlifting.png" ) )
    {
        printf( "Failed to load bird texture!\n" );
        success = false;
    }
    //Load bird texture
    if( !gBirdTexture[1].loadFromFile( "000/birdfalling.png" ) )
    {
        printf( "Failed to load bird texture!\n" );
        success = false;
    }
	//Load button texture
	if( !gBBTexture.loadFromFile( "000/buttonback600_240.png" ) )
    {
        printf( "Failed to load buttonback600_240 texture!\n" );
        success = false;
    }
	if( !gButtonTexture[0].loadFromFile( "000/resume150_87.png" ) )
    {
        printf( "Failed to load resume150_87 texture!\n" );
        success = false;
    }
    if( !gButtonTexture[1].loadFromFile( "000/newgame150_87.png" ) )
    {
        printf( "Failed to load newgame150_87 texture!\n" );
        success = false;
    }
    if( !gButtonTexture[2].loadFromFile( "000/quit150_87.png" ) )
    {
        printf( "Failed to load quit150_87 texture!\n" );
        success = false;
    }
    //Load sound effects
	gbeat = Mix_LoadWAV( "000/beat_1.wav" );
	if( gbeat == NULL )
	{
		printf( "Failed to load beat_1 sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gclick = Mix_LoadWAV( "000/click_1.wav" );
	if( gbeat == NULL )
	{
		printf( "Failed to load click_1 sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

	return success;
}

void close()
{
    ghighscore[10].free();
    gEEBird.free();
    ghalftransparent.free();
    gloseBird.free();
	gStartBTexture.free();
	gInfoBTexture.free();
	gInfoTexture[3].free();
	gCloseInfoTexture.free();
	gBarrierTexture.free();
	gBGTexture.free();
    gBirdTexture[2].free();
    gBBTexture.free();
    gButtonTexture[3].free();
    gscore.free();
    TTF_CloseFont( gFont );
    gFont = NULL;

    //Free the sound effects
    Mix_FreeChunk( gbeat );
    Mix_FreeChunk( gclick );
	gbeat = NULL;
	gclick = NULL;

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
bool init()
{
    srand((unsigned int )time(0));
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "EEBird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
                //Initialize SDL_ttf
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
                //Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
            }
        }
    }

    return success;
}
/*
bool escape ( SDL_Event& e )
{
    //escape flag
    bool esc = false;

    //If SPACE key was pressed
    if ( e.key.keysym.sym == SDLK_RSHIFT || e.key.keysym.sym == SDLK_LSHIFT ) esc = true;

    return esc;
}*/


int main( int argc, char* args[] )
{
    // initialize highscore;
    for(int i=0;i<10;i++)
    {highscore[i]=0;};
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//escape buttons
            Button resume(1),newgame(2),quitgame(3);

            //The bird object that we will control
			Bird bird;

			//While application is running
            while( !quit )
            {

                score = 0;
                bool start = false;

                //check if game start
                while(( SDL_PollEvent( &e ) != 0 )&&(!quit))
                {
                    if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else
                    {
                        start = StartScene( e );
                        if(start) playonce=true;
                    }
                    //quit = escape ( e );

                }

                //reset position of bird and barriers
                bird.resetPos();
                for(int i=0;i<5;i++)
                {
                    barrier[i].reset(i+1);
                }

                //mark first round after enter game
                bool first = true;

                //appear flag for buttons
                bool buttonAppear = false;

                //flag to check if game over
                bool gameover = false;
                int i=1;
                //game start from here
                while( start &&(!quit))
                {
                    //Handle events on queue
                    while(( SDL_PollEvent( &e ) != 0 )&&(!quit))
                    {
                       //User request quit
                        if( e.type == SDL_QUIT )
                        {
                            start = false;
                            quit = true;
                        }

                        else if( e.key.keysym.sym == SDLK_ESCAPE )
                        {
                            buttonAppear = true;
                        }

                        //start = !escape( e );
                        //quit = escape ( e );

                        if( buttonAppear == true )
                        {
                            if(!gameover)
                            {
                                resume.handleEvent( e ,1);
                                newgame.handleEvent( e ,1);
                                quitgame.handleEvent( e ,1);
                            }
                            else
                            {
                                resume.handleEvent( e ,2);
                                newgame.handleEvent( e ,2);
                                quitgame.handleEvent( e ,2);
                            }
                        }
                        else
                        {
                            bird.handleEvent( e );
                        }
                    }

                    //if button appear, move the button and freeze the background and barriers
                    if( buttonAppear )
                    {

                        if(!gameover)
                        {
                            resume.move();newgame.move();quitgame.move();
                        }
                        else
                        {
                            newgame.move();quitgame.move();
                        }

                        if( resume.getButtonflag() )
                        {
                            buttonAppear = false;
                        }
                        else if( newgame.getButtonflag() )
                        {
                            buttonAppear = false; start = false;
                        }
                        else if( quitgame.getButtonflag() )
                        {
                            buttonAppear = false; start = false; quit = true;
                        }
                        //Scroll background
                        scrollingOffset-=1;
                        if( scrollingOffset < - gBGTexture.getWidth())
                        {
                            scrollingOffset = 0;
                        }

                    }
                    else if( !gameover )
                    {
                        //Move the barrier
                        for(int i=0;i<5;i++)
                        {
                            barrier[i].move();
                        }

                        //Move the bird
                        bird.move();

                        //Scroll background
                        scrollingOffset-=1;
                        if( scrollingOffset < - gBGTexture.getWidth())
                        {
                            scrollingOffset = 0;
                        }
                    }

                    //check whether collision occur
                    if(first) first=false;
                    else
                    {
                        for(int i=0;i<5;i++)
                        {
                            bool go=false;
                            go = bird.collision(barrier[i].locationX() ,barrier[i].locationY());
                            if(go)
                            {
                                buttonAppear=true;gameover=true;
                                if(playonce) {Mix_PlayChannel( -1, gbeat, 0 ); playonce=false;}
                            }
                        }
                    }

                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render background
                    gBGTexture.render( scrollingOffset, 0 );
                    gBGTexture.render( scrollingOffset + gBGTexture.getWidth()-5, 0 );

                    //Render objects
                    for(int i=0;i<5;i++)
                    {
                        barrier[i].render();
                    }

                    bird.render(gameover);

                    Score();
                    SDL_Color textColor = { 60, 60, 60 };
                    // output score
                    if (score>highscore[0])
                    {
                        newhighornot=1;
                    }
                    if (newhighornot==1)
                    {
                        gscore.loadFromRenderedText( "NEW HIGH : "+scorestring(score), textColor ) ;
                        gscore.render(  SCREEN_WIDTH - 410, 10);
                    }
                    else
                    {
                        gscore.loadFromRenderedText( "SCORE : "+scorestring(score), textColor ) ;
                        gscore.render(  SCREEN_WIDTH - 337, 10);
                    }
                    if (score==0)
                    {
                        newhighornot=0;
                    }

                    if( buttonAppear == true )
                    {
                        //show highscore
                        sort();
                        SDL_Color textColor = { 200, 200, 200 };

                        gtop10.render(0-movetop10,0);
                        for (int i=0;i<10;i++)
                        {
                            ghighscore[i].loadFromRenderedText( scorestring(highscore[i]), textColor );
                            ghighscore[i].render(120-movetop10,60*i+87);
                        }
                        if (movetop10>0)movetop10-=10;
                        if(!gameover)
                        {
                            gBBTexture.render( SCREEN_WIDTH/2-300 , SCREEN_HEIGHT/2-100-50 );
                            resume.render(1);newgame.render(1);quitgame.render(1);
                        }
                        else
                        {
                            gloseBird.render(SCREEN_WIDTH/2-400,SCREEN_HEIGHT-280+move_loseBird);
                            gBBTexture.render( SCREEN_WIDTH/2-300 , SCREEN_HEIGHT/2-100-100-move_restart );
                            newgame.render(2);quitgame.render(2);
                            if(move_loseBird>0)move_loseBird-=6;
                            if(move_loseBird<=0&&move_restart>0)move_restart-=10;
                        }
                        move_start=700;
                        move_EEBird=440;
                    }
                    else
                    {
                        if (score>highscore[9])
                        {
                            highscore[9]=score;
                        }
                        movetop10=400;
                    }

                    //Update screen
                    SDL_RenderPresent( gRenderer );
                    i*=(-1);
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );
                //Scroll background
                scrollingOffset-=1;
                if( scrollingOffset < - gBGTexture.getWidth())
                {
                    scrollingOffset = 0;
                }

                gBGTexture.render( scrollingOffset, 0 );
                gBGTexture.render( scrollingOffset + gBGTexture.getWidth()-5, 0 );

                //Render start scene
                if( !infoAppear )
                {
                        gStartBTexture.render( StartB_PosX-move_start , StartB_PosY );
                        gInfoBTexture.render( StartB_PosX+StartB_WIDTH+40+move_start , StartB_PosY );
                        gEEBird.render(SCREEN_WIDTH/2-400,SCREEN_HEIGHT-280+move_EEBird);

                }
                else
                {
                    gStartBTexture.render( StartB_PosX , StartB_PosY );
                    gInfoBTexture.render( StartB_PosX+StartB_WIDTH+40 , StartB_PosY );
                    gEEBird.render(SCREEN_WIDTH/2-400,SCREEN_HEIGHT-280-4);
                    ghalftransparent.render(0,0);
                    gInfoTexture[ CurrentInfo ].render( InfoPosX , InfoPosY );
                    gCloseInfoTexture.render( InfoPosX+InfoBox_WIDTH , InfoPosY-25 );

                }
                if(move_start>0){move_start-=7;}
                if(move_EEBird>0){move_EEBird-=6;}
                move_loseBird=440;
                move_restart=400;
                //Update screen
                SDL_RenderPresent( gRenderer );
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
            }
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
