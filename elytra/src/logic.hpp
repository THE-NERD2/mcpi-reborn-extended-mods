#include "template.hpp"

static bool flying = false;
static unsigned char* player = NULL;
static unsigned char* level = NULL;

HOOK(SDL_PollEvent, int, (SDL_Event* event)) {
    ensure_SDL_PollEvent();
    int ret = (*real_SDL_PollEvent)(event);
    if(
        ret == 1 &&
        event != NULL &&
        event->type == SDL_KEYDOWN &&
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
}