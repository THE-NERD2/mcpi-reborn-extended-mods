#include "template.hpp"
/*extern bool inGame;
extern bool mrStop;
extern unsigned char* mc;
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);*/

static void recallMachines() {
	while(!inGame && !mrStop) {}
	if(inGame) {
		fprintf(stderr, "Loading machines...\n");
		unsigned char* player = *(unsigned char**) (mc + Minecraft_player_property_offset);
		unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
		int px = round(*(float*) (player + Entity_x_property_offset));
		int py = round(*(float*) (player + Entity_y_property_offset));
		int pz = round(*(float*) (player + Entity_z_property_offset));
		for(int x = px - 20; x <= px + 20 && !mrStop; x++) {
			for(int y = py - 20; y <= py + 20 && !mrStop; y++) {
				for(int z = pz - 20; z <= pz + 20 && !mrStop; z++) {
					int tile = (*Level_getTile)(level, x, y, z);
					if(tile == 407 || tile == 151) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 409 || tile == 153) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 410 || tile == 413 || tile == 414 || tile == 415 || tile == 416 || tile == 417 || tile == 154 || tile == 157 || tile == 158 || tile == 159 || tile == 160 || tile == 161) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 418 || tile == 419 || tile == 420 || tile == 421 || tile == 422 || tile == 423 || tile == 162 || tile == 163 || tile == 164 || tile == 165 || tile == 166 || tile == 167) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 168 || tile == 169 || tile == 170 || tile == 171) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
				}
			}
		}
		for(int x = 0; x <= 256 && !mrStop; x++) {
			for(int y = 0; y <= 128 && !mrStop; y++) {
				for(int z = 0; z <= 256 && !mrStop; z++) {
					int tile = (*Level_getTile)(level, x, y, z);
					if(tile == 407 || tile == 151) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 409 || tile == 153) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 410 || tile == 413 || tile == 414 || tile == 415 || tile == 416 || tile == 417 || tile == 154 || tile == 157 || tile == 158 || tile == 159 || tile == 160 || tile == 161) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 418 || tile == 419 || tile == 420 || tile == 421 || tile == 422 || tile == 423 || tile == 162 || tile == 163 || tile == 164 || tile == 165 || tile == 166 || tile == 167) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
					if(tile == 168 || tile == 169 || tile == 170 || tile == 171) {
						Level_setTileAndData_addToEnigma(level, x, y, z, tile, 0);
					}
				}
			}
		}
	}
}