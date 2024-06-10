#include "template.hpp"
/*extern bool inGame;
extern bool mrStop;
extern unsigned char* mc;
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);*/

static void recallMachines() {
	while(!inGame && !mrStop) {}
	if(inGame) {
		fprintf(stderr, "Loading machines...\nin immediate area...");
		unsigned char* player = *(unsigned char**) (mc + Minecraft_player_property_offset);
		unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
		int px = round(*(float*) (player + Entity_x_property_offset));
		int py = round(*(float*) (player + Entity_y_property_offset));
		int pz = round(*(float*) (player + Entity_z_property_offset));
		for(int x = px - 20; x <= px + 20 && !mrStop; x++) {
			for(int y = py - 20; y <= py + 20 && !mrStop; y++) {
				for(int z = pz - 20; z <= pz + 20 && !mrStop; z++) {
					int tile = (*Level_getTile)(level, x, y, z);
					int data = (*Level_getData)(level, x, y, z);
					Level_setTileAndData_addToEnigma(level, x, y, z, tile, data);
				}
			}
		}
		fprintf(stderr, "done\nin world...");
		/*for(int x = 0; x <= 256 && !mrStop; x++) {
			for(int y = 0; y <= 128 && !mrStop; y++) {
				for(int z = 0; z <= 256 && !mrStop; z++) {
					int tile = (*Level_getTile)(level, x, y, z);
					int data = (*Level_getData)(level, x, y, z);
					Level_setTileAndData_addToEnigma(level, x, y, z, tile, data);
				}
			}
		}*/
		fprintf(stderr, "done.\n");
	}
}