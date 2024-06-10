#include "template.hpp"
/*extern vector<vector<int>> torches;
extern vector<carrier> enigma;
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);
extern bool existsScheduled(int, int, int);
extern bool isc(int, int, int);
extern int getEnigmaStrength(int, int, int);
extern void setEnigmaStrength_delayed(int, int, int, int);
extern vector<int> processHit(int, int, int, int);*/

static void redstoneTorch_useOn(unsigned char* level, int x, int y, int z, int side) {
	vector<int> point = processHit(x, y, z, side);
	Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 409, 0);
}

static void make_redstone_torch_tile() {
    // Copies regular torch and overrides textures
	unsigned char* torch = Tile_tiles[50];
	unsigned char* redstoneTorch = (unsigned char*) ::operator new(0x5c);
	memcpy(redstoneTorch, torch, 0x5c);
	(*Tile)(redstoneTorch, 153, 0x63, Material_wood);
	unsigned char* tvtable = (unsigned char*) ::operator new(0x104);
	memcpy(tvtable, *(unsigned char**) torch, 0x104);
	*(unsigned char**) redstoneTorch = tvtable;
	Tile_setDestroyTime_t redstoneTorch_setDestroyTime = *(Tile_setDestroyTime_t*) (tvtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t redstoneTorch_setExplodeable = *(Tile_setExplodeable_t*) (tvtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t redstoneTorch_setSoundType = *(Tile_setSoundType_t*) (tvtable + Tile_setSoundType_vtable_offset);
	(*redstoneTorch_setDestroyTime)(redstoneTorch, 0);
	(*redstoneTorch_setExplodeable)(redstoneTorch, 1);
	(*redstoneTorch_setSoundType)(redstoneTorch, SOUND_WOOD);
	Tile_tiles[153] = redstoneTorch; // Tile_init
}
static void make_redstone_torch_item() {
    unsigned char* redstoneTorch = (unsigned char*) ::operator new(0x38);
	(*Item)(redstoneTorch, 153);
	unsigned char* tvtable = *(unsigned char**) redstoneTorch;
	Item_setIcon_t redstoneTorch_setIcon = *(Item_setIcon_t*) (tvtable + Item_setIcon_vtable_offset);
	//Item_setDescriptionId_t redstoneTorch_setDescriptionId = *(Item_setDescriptionId_t*) (tvtable + Item_setDescriptionId_vtable_offset);
	(*redstoneTorch_setIcon)(redstoneTorch, 2, 9);
	//const string name2 = "redstonetorch";
	//(*redstoneTorch_setDescriptionId)(redstoneTorch, name2);
	*(int*) (redstoneTorch + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (redstoneTorch + Item_category_property_offset) = 1;
	*(int*) (redstoneTorch + Item_max_damage_property_offset) = 250;
	*(int*) (redstoneTorch + Item_max_stack_size_property_offset) = 64;
}
static void updateTorches(unsigned char* level) {
	for(int i = 0; i < torches.size(); i++) {
		vector<int> torch = torches[i];
		int tile = (*Level_getTile)(level, torch[0], torch[1], torch[2]);
		if(tile == 0) {
			vector<vector<int>>::iterator it1 = torches.begin() + i;
			torches.erase(it1);
			vector<carrier>::iterator it2;
			carrier match;
			match.point = torch;
			match.power = 0;
			for(int j = 0; j < enigma.size(); j++) {
				carrier c = enigma[j];
				if(c.point == match.point) {
					it2 = enigma.begin() + j;
					break;
				}
			}
			enigma.erase(it2);
		} else if(!existsScheduled(torch[0], torch[1], torch[2])) {
			vector<int> powers = {0};
			if((*Level_getTile)(level, torch[0], torch[1] - 1, torch[2]) != 0) {
				if(isc(torch[0] - 1, torch[1] - 1, torch[2])) {
					powers.push_back(getEnigmaStrength(torch[0] - 1, torch[1] - 1, torch[2]));
				} else if(isc(torch[0], torch[1] - 1, torch[2] + 1)) {
					powers.push_back(getEnigmaStrength(torch[0], torch[1] - 1, torch[2] + 1));
				} else if(isc(torch[0] + 1, torch[1] - 1, torch[2])) {
					powers.push_back(getEnigmaStrength(torch[0] + 1, torch[1] - 1, torch[2]));
				} else if(isc(torch[0], torch[1] - 1, torch[2] - 1)) {
					powers.push_back(getEnigmaStrength(torch[0], torch[1] - 1, torch[2] - 1));
				}
			}
			if((*Level_getTile)(level, torch[0] - 1, torch[1], torch[2]) != 0) {
				if(isc(torch[0] - 2, torch[1], torch[2])) {
					powers.push_back(getEnigmaStrength(torch[0] - 2, torch[1], torch[2]));
				} else if(isc(torch[0] - 1, torch[1], torch[2] + 1)) {
					powers.push_back(getEnigmaStrength(torch[0] - 1, torch[1], torch[2] + 1));
				} else if(isc(torch[0] - 1, torch[1], torch[2] - 1)) {
					powers.push_back(getEnigmaStrength(torch[0] - 1, torch[1], torch[2] - 1));
				}
			}
			if((*Level_getTile)(level, torch[0], torch[1], torch[2] + 1) != 0) {
				if(isc(torch[0] - 1, torch[1], torch[2] + 1)) {
					powers.push_back(getEnigmaStrength(torch[0] - 1, torch[1], torch[2] + 1));
				} else if(isc(torch[0], torch[1], torch[2] + 2)) {
					powers.push_back(getEnigmaStrength(torch[0], torch[1], torch[2] + 2));
				} else if(isc(torch[0] + 1, torch[1], torch[2] + 1)) {
					powers.push_back(getEnigmaStrength(torch[0] + 1, torch[1], torch[2] + 1));
				}
			}
			if((*Level_getTile)(level, torch[0] + 1, torch[1], torch[2]) != 0) {
				if(isc(torch[0] + 2, torch[1], torch[2])) {
					powers.push_back(getEnigmaStrength(torch[0] + 2, torch[1], torch[2]));
				} else if(isc(torch[0] + 1, torch[1], torch[2] + 1)) {
					powers.push_back(getEnigmaStrength(torch[0] + 1, torch[1], torch[2] + 1));
				} else if(isc(torch[0] + 1, torch[1], torch[2] - 1)) {
					powers.push_back(getEnigmaStrength(torch[0] + 1, torch[1], torch[2] - 1));
				}
			}
			if((*Level_getTile)(level, torch[0], torch[1], torch[2] - 1) != 0) {
				if(isc(torch[0] - 1, torch[1], torch[2] - 1)) {
					powers.push_back(getEnigmaStrength(torch[0] - 1, torch[1], torch[2] - 1));
				} else if(isc(torch[0], torch[1], torch[2] - 2)) {
					powers.push_back(getEnigmaStrength(torch[0], torch[1], torch[2] - 2));
				} else if(isc(torch[0] + 1, torch[1], torch[2] - 1)) {
					powers.push_back(getEnigmaStrength(torch[0] + 1, torch[1], torch[2] - 1));
				}
			}
			int max = *max_element(powers.begin(), powers.end());
			if(max == 0 ? 0xf : 0x0 != getEnigmaStrength(torch[0], torch[1], torch[2])) {
				setEnigmaStrength_delayed(torch[0], torch[1], torch[2], max == 0 ? 0xf : 0x0);
			}
		}
	}
}