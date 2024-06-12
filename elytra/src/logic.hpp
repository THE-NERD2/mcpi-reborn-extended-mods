#include "template.hpp"

static bool flying = false;
static unsigned char* player = NULL;
static unsigned char* level = NULL;

static float t = clock();
static float dx = 0.0f;
static float dy = 0.0f;
static float dz = 0.0f;

static float speed = 0.0f;
static const float old_velocity_decay = 0.8f;
static const float drag = 0.0f;
static const float fall_speed = 0.5f;
static const float pitch_accel = 4.0f;
static const float terminal_velocity = 6.0f;
//static const float turn_accel = WIP;
//static const float rocket_boost = 2.0f;

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
                        speed = 0.0f;
                        dx = 0.0f;
                        dy = 0.0f;
                        dz = 0.0f;
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
        int newT = clock();
        float dt = (newT - t) / 1000000;

        float pitch = *(float*) (player + Entity_pitch_property_offset);
        float yaw = *(float*) (player + Entity_yaw_property_offset);
        float lookX = -sind(yaw) * cosd(pitch);
        float lookY = -sind(pitch);
        float lookZ = cosd(yaw) * cosd(pitch);
        float l = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
        lookX /= l;
        lookY /= l;
        lookZ /= l;

        float ds = 0;
        ds -= drag * speed;
        //fprintf(stderr, "%f\n", ds);
        ds += -(lookY + 0.1) * pitch_accel;
        //fprintf(stderr, "%f\n", ds);
        speed += ds * dt;
        //fprintf(stderr, "%f\n", speed);
        if(speed < 0) {
            speed = 0;
        } else if(speed > terminal_velocity) {
            speed = terminal_velocity;
        }

        dx += lookX * speed * dt;
        dy += lookY * speed * dt;
        dz += lookZ * speed * dt;

        Entity_move_t player_move = *(Entity_move_t*) (*(unsigned char**) player + Entity_move_vtable_offset);
        *(float*) (player + Player_jump_property_offset) = 0.0f;
        (*player_move)(player, dx, dy - fall_speed * pow(0.5, speed), dz);

        dx -= dx * old_velocity_decay * dt;
        dy -= dy * old_velocity_decay * dt;
        dz -= dz * old_velocity_decay * dt;

        int x = round(*(float*) (player + Entity_x_property_offset));
        int y = floor(*(float*) (player + Entity_y_property_offset) - 1.5f);
        int z = round(*(float*) (player + Entity_z_property_offset));
        if((*Level_getTile)(level, x, y, z) != 0) {
            flying = false;
        }

        t = newT;
    }
}