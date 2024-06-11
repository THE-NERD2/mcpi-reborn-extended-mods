#include "template.hpp"

static bool flying = false;
static unsigned char* player = NULL;
static unsigned char* level = NULL;
static float t = clock();
static float px = 0.0f;
static float py = 0.0f;
static float pz = 0.0f;
static float dx = 0.0f;
static float dy = 0.0f;
static float dz = 0.0f;

static float speed = 1.0f;

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
                if((*Level_getTile)(level, x, y - 3, z) == 0) {
                    flying = !flying;
                    if(flying) {
                        t = clock();
                        px = *(float*) (player + Entity_x_property_offset);
                        py = *(float*) (player + Entity_y_property_offset);
                        pz = *(float*) (player + Entity_z_property_offset);
                    }
                    fprintf(stderr, "Toggled flying\n");
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
        float lookX = -sind(yaw);
        float lookY = -sind(pitch);
        float lookZ = cosd(yaw);
        float l = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
        lookX /= l;
        lookY /= l;
        lookZ /= l;
        dx = lookX;
        dy = lookY;
        dz = lookZ;
        int newT = clock();
        float newX = px + dx * (float) (newT - t) / 1000000.0;
        float newY = py + dy * (float) (newT - t) / 1000000.0;
        float newZ = pz + dz * (float) (newT - t) / 1000000.0;
        if((*Level_getTile)(level, round(newX), round(newY), round(newZ)) == 0 && (*Level_getTile)(level, round(newX), round(newY) - 1, round(newZ)) == 0) {
            px = newX;
            py = newY;
            pz = newZ;
            Entity_move_t player_move = *(Entity_move_t*) (*(unsigned char**) player + Entity_move_vtable_offset);
            (*player_move)(player, dx, dy, dz);
        }
        t = newT;
    }
}