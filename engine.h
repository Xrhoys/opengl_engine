#ifndef ENGINE_H
#define ENGINE_H

struct platform_engine
{
    b32 running;
    u32 posX, posY;
    u32 windowWidth, windowHeight;
    
    // Timer
    f64 time;
	platform_memory memory;
    
    // GAME LAYER
    game_init*         gameInit;
    game_update*       gameUpdate;
	game_state*        gameState;
	
	// Utility
	platform_log*      log;
	
	char* vsSource;
	char* psSource;
	
	// Audio
	platform_audio audio;
};

#endif //ENGINE_H
