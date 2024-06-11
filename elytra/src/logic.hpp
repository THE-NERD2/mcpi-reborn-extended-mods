#include "template.hpp"

static bool flying = false;
static unsigned char* player = NULL;
static unsigned char* level = NULL;
static float t = clock();
static float dx = 0.0f;
static float dy = 0.0f;
static float dz = 0.0f;

static float speed = 20.0f;

HOOK(SDL_PollEvent, int, (SDL_Event* event)) {
    ensure_SDL_PollEvent();
    int ret = (*real_SDL_PollEvent)(event);
    if(
        ret == 1 &&
        event != NULL &&
        event->type == SDL_KEYUP &&
        event->key.keysym.sym == SDLK_SPACE &&
        player != NULL
    ) {
        ItemInstance* armor = (*Player_getArmor)(player, 1);
        if(armor != NULL) {
            if(armor->id == 413) {
                int x = round(*(float*) (player + Entity_x_property_offset));
                int y = floor(*(float*) (player + Entity_y_property_offset));
                int z = round(*(float*) (player + Entity_z_property_offset));
                if((*Level_getTile)(level, x, y - 3, z) == 0 || flying) {
                    flying = !flying;
                    if(flying) {
                        t = clock();
                    }
                }
            }
        }
    }
    return ret;
}

static void update(unsigned char* minecraft) {
    player = *(unsigned char**) (minecraft + Minecraft_player_property_offset);
    level = *(unsigned char**) (minecraft + Minecraft_level_property_offset);
    if(flying && player != NULL) {
        float pitch = *(float*) (player + Entity_pitch_property_offset);
        float yaw = *(float*) (player + Entity_yaw_property_offset);
        float lookX = -sind(yaw) * cosd(pitch);
        float lookY = -sind(pitch);
        float lookZ = cosd(yaw) * cosd(pitch);
        float l = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
        lookX /= l;
        lookY /= l;
        lookZ /= l;
        dx = lookX * speed;
        dy = lookY * speed;
        dz = lookZ * speed;
        int newT = clock();
        dx *= (newT - t) / 1000000;
        dy *= (newT - t) / 1000000;
        dz *= (newT - t) / 1000000;
        *(float*) (player + Player_jump_property_offset) = 0.0f;
        Entity_move_t player_move = *(Entity_move_t*) (*(unsigned char**) player + Entity_move_vtable_offset);
        (*player_move)(player, dx, dy, dz);
        t = newT;
    }
}