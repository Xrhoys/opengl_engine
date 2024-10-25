GAME_INIT(game_Init)
{
    engine->gameState = arena_PushStruct(&engine->memory.permanentStorage, game_state);
	
	game_state* game = engine->gameState;
	
	game_entity* entity = &game->entities[0];
	entity->position = V2(0.f, 0.f);
	entity->size = V2(100.f, 100.f);
}

GAME_UPDATE(game_Update)
{
    
}
