#ifndef REDSTONE_MOD_TEMPLATE
#define REDSTONE_MOD_TEMPLATE

#ifndef REBORN_HAS_PATCH_CODE
#define REBORN_HAS_PATCH_CODE
#endif

#include <libreborn/libreborn.h>
#include <SDL/SDL.h>
#include <mods/misc/misc.h>
#include <symbols/minecraft.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <math.h>
using namespace std;

typedef int (*Tile_getTexture2_t)(unsigned char*, int); // unsigned char* tile, int face
static unsigned int Tile_getTexture2_vtable_offset = 0x2c;

typedef int (*Tile_getTexture3_t)(unsigned char*, unsigned char*, int, int, int, int); // unsigned char* tile, unsigned char* level, int x, int y, int z, int face
static unsigned int Tile_getTexture3_vtable_offset = 0x30;

typedef int (*Tile_isSolidRender_t)(unsigned char*); // unsigned char* tile
static unsigned int Tile_isSolidRender_vtable_offset = 0x40;

typedef int (*Tile_getRenderLayer_t)(unsigned char*); // unsigned char* tile
static unsigned int Tile_getRenderLayer_vtable_offset = 0x94;

typedef void (*Tile_updateDefaultShape_t)(unsigned char*); // unsigned char* tile
static unsigned int Tile_updateDefaultShape_vtable_offset = 0x18;

typedef void (*Tile_updateShape_t)(unsigned char*, unsigned char*, int, int, int); // unsigned char* tile, unsigned char* level, int x, int y, int z
static unsigned int Tile_updateShape_vtable_offset = 0x14;

typedef unsigned char* (*Tile_getDescriptionId_t)(unsigned char*, int); // unsigned char* tile, int <name>
static unsigned int Tile_getDescriptionId_vtable_offset = 0xdc;

typedef AABB* (*Tile_getAABB_t)(unsigned char*, unsigned char*, int, int, int);
static unsigned int Tile_getAABB_vtable_offset = 0x34;

static unsigned int Tile_AABB_property_offset = 0x40;

typedef int (*Level_getData_t)(unsigned char*, int, int, int); // unsigned char* tile, int x, int y, int z
static Level_getData_t Level_getData = (Level_getData_t) 0xa3324;

struct carrier {
	vector<int> point;
	int power;
};
enum operationType {
	PISTON,
	PISTON_RETRACT,
	STICKY_PISTON,
	STICKY_PISTON_RETRACT,
	SET_ENIGMA,
	ACTIVATE_REPEATER,
	DEACTIVATE_REPEATER
};
struct operation {
	operationType type;
	vector<int> block;
	vector<int> direction;
	int strength;
	int initTime;
	int delay;
};

#endif // #ifndef REDSTONE_MOD_TEMPLATE