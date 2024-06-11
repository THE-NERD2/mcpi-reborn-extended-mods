#ifndef ELYTRA_MOD_TEMPLATE
#define ELYTRA_MOD_TEMPLATE

#ifndef REBORN_HAS_PATCH_CODE
#define REBORN_HAS_PATCH_CODE
#endif

#include <libreborn/libreborn.h>
#include <SDL/SDL.h>
#include <mods/misc/misc.h>
#include <symbols/minecraft.h>
#include <math.h>

typedef unsigned char* (*ArmorItem_t)(unsigned char*, int, unsigned char**, int, int);
static ArmorItem_t ArmorItem = (ArmorItem_t) 0x9362c;

typedef ItemInstance* (*Player_getArmor_t)(unsigned char*, int);
static Player_getArmor_t Player_getArmor = (Player_getArmor_t) 0x8fda4;

typedef void (*Entity_move_t)(unsigned char*, float, float, float);
static unsigned int Entity_move_vtable_offset = 0x18;

static unsigned int Player_jump_property_offset = 0x38;

unsigned char** Material_iron = (unsigned char**) 0x17a7a8;

static float sind(float deg) {
    return sinf(deg * M_PI / 180.0f);
}
static float cosd(float deg) {
    return cosf(deg * M_PI / 180.0f);
}

#endif // #ifndef ELYTRA_MOD_TEMPLATE