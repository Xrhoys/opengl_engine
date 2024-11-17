#ifndef GAME_H
#define GAME_H

struct game_state;
struct game_entity;

#include "platform.h"
#include "c_math.h"
#include "camera.h"
#include "audio.h"

#include "renderer.h"

struct game_entity
{
	v2 position;
	v2 size;
};

struct game_state
{
    game_entity entities[256];
	u32 entityCount;
};

#endif //GAME_H
