/* Notes:
	- Do not use pistons to push a redstone block into a redstone torch
	- Do not touch a redstone block
*/

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

/* Texture Ids:
    - 0: bottom
    - 1: top
    - 2: side texture z-
    - 3: side texture z+
    - 4: side texture x-
    - 5: side texture x+
*/

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

static unsigned char* mc;
static vector<carrier> enigma;
static vector<operation> scheduled;
static vector<vector<int>> redstoneDusts;
static vector<vector<int>> torches;
static vector<vector<int>> pistons;
static vector<bool> pistonActivities;
static vector<bool> pistonActPending;
static vector<vector<int>> spistons;
static vector<bool> spistonActivities;
static vector<bool> spistonActPending;
static vector<vector<int>> repeaters;
static thread machineRecaller;
static bool mrStop = false;
static bool inGame = false;

// Is Signal Carrier
static bool isc(int x, int y, int z) {
	bool exists = false;
	for(carrier c : enigma) {
		vector<int> point = c.point;
		exists = exists || (x == point[0] && y == point[1] && z == point[2]);
	}
	return exists;
}
// Convert raw x, y, z, and side into block place position
static vector<int> processHit(int x, int y, int z, int side) {
	int dx = 0;
	int dy = 0;
	int dz = 0;
	switch(side) {
		case 0:
			dy = -1;
			break;
		case 1:
			dy = 1;
			break;
		case 2:
			dz = -1;
			break;
		case 3:
			dz = 1;
			break;
		case 4:
			dx = -1;
			break;
		case 5:
			dx = 1;
			break;
	}
	vector<int> point = {x + dx, y + dy, z + dz};
	return point;
}
static int getEnigmaStrength(int x, int y, int z) {
	vector<int> point = {x, y, z};
	int strength = 0;
	for(carrier c : enigma) {
		if(c.point == point) {
			strength = c.power;
		}
	}
	return strength;
}
static bool existsScheduled(int x, int y, int z) {
	vector<int> point = {x, y, z};
	bool exists = false;
	for(operation op : scheduled) {
		if(op.block == point) {
			exists = true;
		}
	}
	return exists;
}
static void setEnigmaStrength(int x, int y, int z, int strength) {
	vector<int> point = {x, y, z};
	for(int i = 0; i < enigma.size(); i++) {
		carrier c1 = enigma[i];
		if(c1.point == point) {
			carrier c2;
			c2.point = point;
			c2.power = strength;
			enigma[i] = c2;
			return;
		}
	}
	fprintf(stderr, "Not part of the enigma.\n");
}
static void setEnigmaStrength_delayed(int x, int y, int z, int strength) {
	int initTime = int(clock());
	vector<int> point = {x, y, z};
	operation op;
	op.type = operationType::SET_ENIGMA;
	op.block = point;
	op.strength = strength;
	op.initTime = initTime;
	op.delay = 100;
	scheduled.push_back(op);
}
static void Level_setTileAndData_injection(unsigned char* level, int x, int y, int z, int id, int aux) {
	int t_xmz = (*Level_getTile)(level, x - 1, y, z);
	int t_xzp = (*Level_getTile)(level, x, y, z + 1);
	int t_xpz = (*Level_getTile)(level, x + 1, y, z);
	int t_xzm = (*Level_getTile)(level, x, y, z - 1);
	if(t_xmz == 409 || t_xmz == 410) {
		(*Level_setTileAndData)(level, x - 1, y, z, 0, 0);
		(*Level_setTileAndData)(level, x, y, z, id, aux);
		(*Level_setTileAndData)(level, x - 1, y, z, t_xzm, 0);
	} else if(t_xzp == 409 || t_xzp == 410) {
		(*Level_setTileAndData)(level, x, y, z + 1, 0, 0);
		(*Level_setTileAndData)(level, x, y, z, id, aux);
		(*Level_setTileAndData)(level, x, y, z + 1, t_xzp, 0);
	} else if(t_xpz == 409 || t_xpz == 410) {
		(*Level_setTileAndData)(level, x + 1, y, z, 0, 0);
		(*Level_setTileAndData)(level, x, y, z, id, aux);
		(*Level_setTileAndData)(level, x + 1, y, z, t_xpz, 0);
	} else if(t_xzm == 409 || t_xzm == 410) {
		(*Level_setTileAndData)(level, x, y, z - 1, 0, 0);
		(*Level_setTileAndData)(level, x, y, z, id, aux);
		(*Level_setTileAndData)(level, x, y, z - 1, t_xzm, 0);
	} else {
		(*Level_setTileAndData)(level, x, y, z, id, aux);
	}
}
static void Level_setTileAndData_addToEnigma(unsigned char* level, int x, int y, int z, int id, int data) {
	Level_setTileAndData_injection(level, x, y, z, id, data);
	if(id == 407 || id == 151) {
		vector<int> point = {x, y, z};
		carrier c;
		c.point = point;
		c.power = 0;
		enigma.push_back(c);
		redstoneDusts.push_back(point);
	} else if(id == 409 || id == 153) {
		vector<int> point = {x, y, z};
		carrier c;
		c.point = point;
		c.power = 0xf;
		enigma.push_back(c);
		torches.push_back(point);
	} else if(id == 410 || id == 413 || id == 414 || id == 415 || id == 416 || id == 417 || id == 154 || id == 157 || id == 158 || id == 159 || id == 160 || id == 161) {
		vector<int> point = {x, y, z};
		carrier c;
		c.point = point;
		c.power = 0;
		enigma.push_back(c);
		pistons.push_back(point);
		pistonActivities.push_back(false);
		pistonActPending.push_back(false);
	} else if(id == 418 || id == 419 || id == 420 || id == 421 || id == 422 || id == 423 || id == 162 || id == 163 || id == 164 || id == 165 || id == 166 || id == 167) {
		vector<int> point = {x, y, z};
		carrier c;
		c.point = point;
		c.power = 0;
		enigma.push_back(c);
		spistons.push_back(point);
		spistonActivities.push_back(false);
		spistonActPending.push_back(false);
	} else if(id == 168 || id == 169 || id == 170 || id == 171) {
		vector<int> point = {x, y, z};
		carrier c;
		c.point = point;
		c.power = 0;
		enigma.push_back(c);
		repeaters.push_back(point);
	}
}
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
static int redstone_getTexture2(unsigned char* tile, int texId) {
	if(texId == 0) {
		return 201;
	} else {
		return 180;
	}
}
static int redstone_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	//int aux = getEnigmaStrength(x, y, z); // This doesn't update very fast...
	int aux = (*Level_getData)(*(unsigned char**) (mc + Minecraft_level_property_offset), x, y, z);
	bool t_xmymz = isc(x - 1, y - 1, z);
	bool t_xmyz = isc(x - 1, y, z);
	bool t_xmypz = isc(x - 1, y + 1, z);
	bool t_xymzp = isc(x, y - 1, z + 1);
	bool t_xyzp = isc(x, y, z + 1);
	bool t_xypzp = isc(x, y + 1, z + 1);
	bool t_xpymz = isc(x + 1, y - 1, z);
	bool t_xpyz = isc(x + 1, y, z);
	bool t_xpypz = isc(x + 1, y + 1, z);
	bool t_xymzm = isc(x, y - 1, z - 1);
	bool t_xyzm = isc(x, y, z - 1);
	bool t_xypzm = isc(x, y + 1, z - 1);
	bool xmz_isc = t_xmymz || t_xmyz || t_xmypz;
	bool xzp_isc = t_xymzp || t_xyzp || t_xypzp;
	bool xpz_isc = t_xpymz || t_xpyz || t_xpypz;
	bool xzm_isc = t_xymzm || t_xyzm || t_xypzm;
	if(texId == 0 || texId == 1) {
		if(aux == 0) {
			if(xmz_isc || xpz_isc) {
				if(xzp_isc || xzm_isc) {
					return 201;
				} else {
					return 203;
				}
			} else if(xzp_isc || xzm_isc) {
				if(xmz_isc || xpz_isc) {
					return 201;
				} else {
					return 202;
				}
			} else {
				return 201;
			}
		} else {
			if(xmz_isc || xpz_isc) {
				if(xzp_isc || xzm_isc) {
					return 184;
				} else {
					return 200;
				}
			} else if(xzp_isc || xzm_isc) {
				if(xmz_isc || xpz_isc) {
					return 184;
				} else {
					return 185;
				}
			} else {
				return 184;
			}
		}
	} else {
		return 180;
	}
}
static int redstone_isSolidRender(unsigned char* tile) {
	return 0;
}
static int redstone_getRenderLayer(unsigned char* tile) {
	return 1;
}
static void redstone_updateDefaultShape(unsigned char* tile) {
	*(int*) (tile + 0xc) = 0;
	*(int*) (tile + 0x10) = 0;
	*(int*) (tile + 0x14) = 0;
	*(int*) (tile + 0x18) = (int) (unsigned int) 0x3f800000;
	*(int*) (tile + 0x1c) = 0;
	*(int*) (tile + 0x20) = (int) (unsigned int) 0x3f800000;
}
static void redstone_updateShape(unsigned char* tile, unsigned char* param_1, int x, int y, int z) {
	redstone_updateDefaultShape(tile);
}
static unsigned char* redstone_getDescriptionId(unsigned char* tile, int param_1) {
	// Is this what I'm supposed to do?
	const char* descriptionId = "reddust";
	char* cdi = const_cast<char*>(descriptionId);
	unsigned char* ucdi = (unsigned char*) cdi;
	return ucdi;
}
static void redstone_useOn(unsigned char* level, int x, int y, int z, int side) {
	vector<int> point = processHit(x, y, z, side);
	Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 407, 0);
}
static void redstoneTorch_useOn(unsigned char* level, int x, int y, int z, int side) {
	vector<int> point = processHit(x, y, z, side);
	Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 409, 0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

static int piston_up_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
			return 0x6d;
		case 1:
			return 0x6b;
		case 2:
		case 3:
		case 4:
		case 5:
			return 0x6c;
	}
}
static int piston_up_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_up_getTexture2(tile, texId);
}
static int piston_down_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
			return 0x6b;
		case 1:
			return 0x6d;
		case 2:
		case 3:
		case 4:
		case 5:
			return 0xab;
	}
}
static int piston_down_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_down_getTexture2(tile, texId);
}
static int piston_left_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
		case 1:
			return 0xac;
		case 2:
			return 0xaa;
		case 3:
			return 0xac;
		case 4:
			return 0x6b;
		case 5:
			return 0x6d;
	}
}
static int piston_left_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_left_getTexture2(tile, texId);
}
static int piston_right_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
		case 1:
			return 0xaa;
		case 2:
			return 0xac;
		case 3:
			return 0xaa;
		case 4:
			return 0x6d;
		case 5:
			return 0x6b;
	}
}
static int piston_right_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_right_getTexture2(tile, texId);
}
static int piston_front_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
		case 1:
			return 0xab;
		case 4:
			return 0xaa;
		case 5:
			return 0xac;
		case 2:
			return 0x6d;
		case 3:
			return 0x6b;
	}
}
static int piston_front_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_front_getTexture2(tile, texId);
}
static int piston_back_getTexture2(unsigned char* tile, int texId) {
	switch(texId) {
		case 0:
		case 1:
			return 0x6c;
		case 4:
			return 0xac;
		case 5:
			return 0xaa;
		case 2:
			return 0x6b;
		case 3:
			return 0x6d;
	}
}
static int piston_back_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return piston_back_getTexture2(tile, texId);
}

#pragma GCC diagnostic pop

static int spiston_up_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_up_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_up_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_up_getTexture2(tile, texId);
}
static int spiston_down_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_down_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_down_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_down_getTexture2(tile, texId);
}
static int spiston_left_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_left_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_left_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_left_getTexture2(tile, texId);
}
static int spiston_right_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_right_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_right_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_right_getTexture2(tile, texId);
}
static int spiston_front_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_front_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_front_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_front_getTexture2(tile, texId);
}
static int spiston_back_getTexture2(unsigned char* tile, int texId) {
	int ptex = piston_back_getTexture2(tile, texId);
	if(ptex == 0x6b) {
		return 0x6a;
	}
	return ptex;
}
static int spiston_back_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	return spiston_back_getTexture2(tile, texId);
}

static int piston_isSolidRender(unsigned char* tile) {
	return 1;
}
static int piston_getRenderLayer(unsigned char* tile) {
	return 1;
}
static void piston_updateDefaultShape(unsigned char* tile) {
	*(int*) (tile + 0xc) = 0;
	*(int*) (tile + 0x10) = 0;
	*(int*) (tile + 0x14) = 0;
	*(int*) (tile + 0x18) = (int) (unsigned int) 0x3f800000;
	*(int*) (tile + 0x1c) = (int) (unsigned int) 0x3f800000;
	*(int*) (tile + 0x20) = (int) (unsigned int) 0x3f800000;
}
static void piston_updateShape(unsigned char* tile, unsigned char* level, int x, int y, int z) {
	piston_updateDefaultShape(tile);
}
static unsigned char* piston_getDescriptionId(unsigned char* tile, int param_1) {
	// ?
	const char* descriptionId = "piston";
	char* cdi = const_cast<char*>(descriptionId);
	unsigned char* ucdi = (unsigned char*) cdi;
	return ucdi;
}
static void piston_useOn(unsigned char* level, int x, int y, int z, int side, unsigned char* player) {
	vector<int> point = processHit(x, y, z, side);
	float pitch = *(float*) (player + Entity_pitch_property_offset); // y= is 0, downward
	float rawYaw = *(float*) (player + Entity_yaw_property_offset); // z+ is 0, cw, INCREASES CONTINUOUSLY; i.e. not always 0-359
	int iYaw = round(rawYaw);
	int yaw = iYaw % 360;
	if(pitch >= 45) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 410, 0); // Facing up
	} else if(pitch <= -45) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 413, 0); // Facing down
	} else {
		if(yaw >= 225 && yaw <= 315) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 414, 0); // Facing left
		} else if(yaw >= 135 && yaw < 225) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 416, 0); // Facing front
		} else if(yaw >= 45 && yaw < 135) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 415, 0); // Facing right
		} else if(yaw < 45 || yaw > 315) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 417, 0); // Facing back
		}
	}
}
static void spiston_useOn(unsigned char* level, int x, int y, int z, int side, unsigned char* player) {
	vector<int> point = processHit(x, y, z, side);
	float pitch = *(float*) (player + Entity_pitch_property_offset); // y= is 0, downward
	float rawYaw = *(float*) (player + Entity_yaw_property_offset); // z+ is 0, cw, INCREASES CONTINUOUSLY; i.e. not always 0-359
	int iYaw = round(rawYaw);
	int yaw = iYaw % 360;
	if(pitch >= 45) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 418, 0); // Facing up
	} else if(pitch <= -45) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 419, 0); // Facing down
	} else {
		if(yaw >= 225 && yaw <= 315) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 420, 0); // Facing left
		} else if(yaw >= 135 && yaw < 225) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 422, 0); // Facing front
		} else if(yaw >= 45 && yaw < 135) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 421, 0); // Facing right
		} else if(yaw < 45 || yaw > 315) {
			Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 423, 0); // Facing back
		}
	}
}
static void setPistonProperties(unsigned char* tile, unsigned char* vtable) {
	*(Tile_isSolidRender_t*) (vtable + Tile_isSolidRender_vtable_offset) = &piston_isSolidRender;
	*(Tile_getRenderLayer_t*) (vtable + Tile_getRenderLayer_vtable_offset) = &piston_getRenderLayer;
	*(Tile_updateDefaultShape_t*) (vtable + Tile_updateDefaultShape_vtable_offset) = &piston_updateDefaultShape;
	*(Tile_updateShape_t*) (vtable + Tile_updateShape_vtable_offset) = &piston_updateShape;
	*(Tile_getDescriptionId_t*) (vtable + Tile_getDescriptionId_vtable_offset) = &piston_getDescriptionId;
	Tile_setDestroyTime_t piston_setDestroyTime = *(Tile_setDestroyTime_t*) (vtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t piston_setExplodeable = *(Tile_setExplodeable_t*) (vtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t piston_setSoundType = *(Tile_setSoundType_t*) (vtable + Tile_setSoundType_vtable_offset);
	(*piston_setDestroyTime)(tile, 10);
	(*piston_setExplodeable)(tile, 1);
	(*piston_setSoundType)(tile, SOUND_STONE);
}
static int repeater_getTexture2(unsigned char* tile, int texId) {
	return 0x83;
}
static int repeater_left_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	int aux = (*Level_getData)(*(unsigned char**) (mc + Minecraft_level_property_offset), x, y, z);
	if(aux == 0) {
		return 0xc7;
	} else {
		return 0xd9;
	}
}
static int repeater_right_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	int aux = (*Level_getData)(*(unsigned char**) (mc + Minecraft_level_property_offset), x, y, z);
	if(aux == 0) {
		return 0x99;
	} else {
		return 0xd7;
	}
}
static int repeater_front_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	int aux = (*Level_getData)(*(unsigned char**) (mc + Minecraft_level_property_offset), x, y, z);
	if(aux == 0) {
		return 0xad;
	} else {
		return 0xd8;
	}
}
static int repeater_back_getTexture3(unsigned char* tile, unsigned char* param_1, int x, int y, int z, int texId) {
	int aux = (*Level_getData)(*(unsigned char**) (mc + Minecraft_level_property_offset), x, y, z);
	if(aux == 0) {
		return 0x83;
	} else {
		return 0x93;
	}
}
static int repeater_isSolidRender(unsigned char* tile) {
	return 0;
}
static int repeater_getRenderLayer(unsigned char* tile) {
	return 1;
}
static void repeater_updateDefaultShape(unsigned char* tile) {
	*(int*) (tile + 0xc) = 0;
	*(int*) (tile + 0x10) = 0;
	*(int*) (tile + 0x14) = 0;
	*(int*) (tile + 0x18) = (int) (unsigned int) 0x3f800000;
	*(int*) (tile + 0x1c) = (int) (unsigned int) 0x3e000000;
	*(int*) (tile + 0x20) = (int) (unsigned int) 0x3f800000;
}
static void repeater_updateShape(unsigned char* tile, unsigned char* param_1, int x, int y, int z) {
	repeater_updateDefaultShape(tile);
}
static unsigned char* repeater_getDescriptionId(unsigned char* tile, int param_1) {
	const char* did = "diode";
	char* cid = const_cast<char*>(did);
	unsigned char* uid = (unsigned char*) cid;
	return uid;
}
static void repeater_useOn(unsigned char* level, int x, int y, int z, int side, unsigned char* player) {
	vector<int> point = processHit(x, y, z, side);
	float rawYaw = *(float*) (player + Entity_yaw_property_offset);
	int iYaw = round(rawYaw);
	int yaw = iYaw % 360;
	if(yaw >= 225 && yaw <= 315) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 170, 0); // Facing left
	} else if(yaw >= 135 && yaw < 225) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 168, 0); // Facing front
	} else if(yaw >= 45 && yaw < 135) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 171, 0); // Facing right
	} else if(yaw < 45 || yaw > 315) {
		Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 169, 0); // Facing back
	}
}
static void setRepeaterProperties(unsigned char* tile, unsigned char* vtable) {
	*(Tile_getTexture2_t*) (vtable + Tile_getTexture2_vtable_offset) = &repeater_getTexture2;
	*(Tile_isSolidRender_t*) (vtable + Tile_isSolidRender_vtable_offset) = &repeater_isSolidRender;
	*(Tile_getRenderLayer_t*) (vtable + Tile_getRenderLayer_vtable_offset) = &repeater_getRenderLayer;
	*(Tile_updateDefaultShape_t*) (vtable + Tile_updateDefaultShape_vtable_offset) = &repeater_updateDefaultShape;
	*(Tile_updateShape_t*) (vtable + Tile_updateShape_vtable_offset) = &repeater_updateShape;
	*(Tile_getDescriptionId_t*) (vtable + Tile_getDescriptionId_vtable_offset) = &repeater_getDescriptionId;
	Tile_setDestroyTime_t repeater_setDestroyTime = *(Tile_setDestroyTime_t*) (vtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t repeater_setExplodeable = *(Tile_setExplodeable_t*) (vtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t repeater_setSoundType = *(Tile_setSoundType_t*) (vtable + Tile_setSoundType_vtable_offset);
	(*repeater_setDestroyTime)(tile, 0);
	(*repeater_setExplodeable)(tile, 1);
	(*repeater_setSoundType)(tile, SOUND_WOOD);
}
static void updateDusts(unsigned char* level) {
	for(int i = 0; i < redstoneDusts.size(); i++) {
		vector<int> dust = redstoneDusts[i];
		int tile = (*Level_getTile)(level, dust[0], dust[1], dust[2]);
		if(tile == 0) {
			vector<vector<int>>::iterator it1 = redstoneDusts.begin() + i;
			redstoneDusts.erase(it1);
			vector<carrier>::iterator it2;
			carrier match;
			match.point = dust;
			match.power = 0;
			for(int j = 0; j < enigma.size(); j++) {
				carrier c = enigma[j];
				if(c.point == match.point) {
					it2 = enigma.begin() + j;
					break;
				}
			}
			enigma.erase(it2);
		} else {
			vector<int> powers = {0};
			powers.push_back(getEnigmaStrength(dust[0] - 1, dust[1] - 1, dust[2]));
			powers.push_back(getEnigmaStrength(dust[0] - 1, dust[1], dust[2]));
			powers.push_back(getEnigmaStrength(dust[0] - 1, dust[1] + 1, dust[2]));
			powers.push_back(getEnigmaStrength(dust[0], dust[1] - 1, dust[2] + 1));
			powers.push_back(getEnigmaStrength(dust[0], dust[1], dust[2] + 1));
			powers.push_back(getEnigmaStrength(dust[0], dust[1] + 1, dust[2] + 1));
			powers.push_back(getEnigmaStrength(dust[0] + 1, dust[1] - 1, dust[2]));
			powers.push_back(getEnigmaStrength(dust[0] + 1, dust[1], dust[2]));
			powers.push_back(getEnigmaStrength(dust[0] + 1, dust[1] + 1, dust[2]));
			powers.push_back(getEnigmaStrength(dust[0], dust[1] - 1, dust[2] - 1));
			powers.push_back(getEnigmaStrength(dust[0], dust[1], dust[2] - 1));
			powers.push_back(getEnigmaStrength(dust[0], dust[1] + 1, dust[2] - 1));
			if((*Level_getTile)(level, dust[0], dust[1] + 1, dust[2]) != 407) {
				powers.push_back(getEnigmaStrength(dust[0], dust[1] + 1, dust[2]));
			}
			if((*Level_getTile)(level, dust[0] - 1, dust[1], dust[2]) == 170 && (*Level_getData)(level, dust[0] - 1, dust[1], dust[2]) == 1) {
				powers.push_back(0x10);
			}
			if((*Level_getTile)(level, dust[0], dust[1], dust[2] + 1) == 168 && (*Level_getData)(level, dust[0], dust[1], dust[2] + 1) == 1) {
				powers.push_back(0x10);
			}
			if((*Level_getTile)(level, dust[0] + 1, dust[1], dust[2]) == 171 && (*Level_getData)(level, dust[0] + 1, dust[1], dust[2]) == 1) {
				powers.push_back(0x10);
			}
			if((*Level_getTile)(level, dust[0], dust[1], dust[2] - 1) == 169 && (*Level_getData)(level, dust[0], dust[1], dust[2] - 1) == 1) {
				powers.push_back(0x10);
			}
			int max = *max_element(powers.begin(), powers.end());
			if(getEnigmaStrength(dust[0], dust[1], dust[2]) != max) {
				setEnigmaStrength(dust[0], dust[1], dust[2], max == 0 ? 0 : max - 1);
				Level_setTileAndData_injection(level, dust[0], dust[1], dust[2], 407, max == 0 ? 0x0 : 0xf);
			}
		}
	}
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
static void updatePistons(unsigned char* level) {
	for(int i = 0; i < pistons.size(); i++) {
		vector<int> piston = pistons[i];
		int tile = (*Level_getTile)(level, piston[0], piston[1], piston[2]);
		if(tile == 0) {
			vector<vector<int>>::iterator it1 = pistons.begin() + i;
			pistons.erase(it1);
			vector<carrier>::iterator it2;
			carrier match;
			match.point = piston;
			match.power = 0;
			for(int j = 0; j < enigma.size(); j++) {
				carrier c = enigma[j];
				if(c.point == match.point) {
					it2 = enigma.begin() + j;
					break;
				}
			}
			enigma.erase(it2);
			vector<bool>::iterator it3 = pistonActivities.begin() + i;
			pistonActivities.erase(it3);
			vector<bool>::iterator it4 = pistonActPending.begin() + i;
			pistonActPending.erase(it4);
		} else {
			vector<int> powers = {0};
			powers.push_back(getEnigmaStrength(piston[0] - 1, piston[1], piston[2]));
			powers.push_back(getEnigmaStrength(piston[0], piston[1], piston[2] + 1));
			powers.push_back(getEnigmaStrength(piston[0] + 1, piston[1], piston[2]));
			powers.push_back(getEnigmaStrength(piston[0], piston[1], piston[2] - 1));
			powers.push_back(getEnigmaStrength(piston[0], piston[1] + 1, piston[2]));
			int max = *max_element(powers.begin(), powers.end());
			if(max == 0 && pistonActivities[i] == true && pistonActPending[i] == true) {
				operation op;
				op.initTime = int(clock());
				pistonActPending[i] = false;
				op.type = operationType::PISTON_RETRACT;
				op.block = piston;
				op.delay = 200;
				scheduled.push_back(op);
			} else if(max > 0 && pistonActivities[i] == false && pistonActPending[i] == false) {
				operation op;
				op.initTime = int(clock());
				pistonActPending[i] = true;
				op.type = operationType::PISTON;
				op.block = piston;
				vector<int> dir = {0, 0, 0};
				if(tile == 154) {
					dir = {0, 1, 0};
				} else if(tile == 157) {
					dir = {0, -1, 0};
				} else if(tile == 158) {
					dir = {-1, 0, 0};
				} else if(tile == 159) {
					dir = {1, 0, 0};
				} else if(tile == 160) {
					dir = {0, 0, 1};
				} else if(tile == 161) {
					dir = {0, 0, -1};
				}
				op.direction = dir;
				op.delay = 200;
				scheduled.push_back(op);
			}
		}
	}
}
static void updateStickyPistons(unsigned char* level) {
	for(int i = 0; i < spistons.size(); i++) {
		vector<int> piston = spistons[i];
		int tile = (*Level_getTile)(level, piston[0], piston[1], piston[2]);
		if(tile == 0) {
			vector<vector<int>>::iterator it1 = spistons.begin() + i;
			spistons.erase(it1);
			vector<carrier>::iterator it2;
			carrier match;
			match.point = piston;
			match.power = 0;
			for(int j = 0; j < enigma.size(); j++) {
				carrier c = enigma[j];
				if(c.point == match.point) {
					it2 = enigma.begin() + j;
					break;
				}
			}
			enigma.erase(it2);
			vector<bool>::iterator it3 = spistonActivities.begin() + i;
			spistonActivities.erase(it3);
			vector<bool>::iterator it4 = spistonActPending.begin() + i;
			spistonActPending.erase(it4);
		} else {
			vector<int> powers = {0};
			powers.push_back(getEnigmaStrength(piston[0] - 1, piston[1], piston[2]));
			powers.push_back(getEnigmaStrength(piston[0], piston[1], piston[2] + 1));
			powers.push_back(getEnigmaStrength(piston[0] + 1, piston[1], piston[2]));
			powers.push_back(getEnigmaStrength(piston[0], piston[1], piston[2] - 1));
			powers.push_back(getEnigmaStrength(piston[0], piston[1] + 1, piston[2]));
			int max = *max_element(powers.begin(), powers.end());
			if(max == 0 && spistonActivities[i] == true && spistonActPending[i] == true) {
				operation op;
				op.initTime = int(clock());
				spistonActPending[i] = false;
				op.type = operationType::STICKY_PISTON_RETRACT;
				op.block = piston;
				vector<int> dir = {0, 0, 0};
				if(tile == 162) {
					dir = {0, 1, 0};
				} else if(tile == 163) {
					dir = {0, -1, 0};
				} else if(tile == 164) {
					dir = {-1, 0, 0};
				} else if(tile == 165) {
					dir = {1, 0, 0};
				} else if(tile == 166) {
					dir = {0, 0, 1};
				} else if(tile == 167) {
					dir = {0, 0, -1};
				}
				op.direction = dir;
				op.delay = 200;
				scheduled.push_back(op);
			} else if(max > 0 && spistonActivities[i] == false && spistonActPending[i] == false) {
				operation op;
				op.initTime = int(clock());
				spistonActPending[i] = true;
				op.type = operationType::STICKY_PISTON;
				op.block = piston;
				vector<int> dir = {0, 0, 0};
				if(tile == 162) {
					dir = {0, 1, 0};
				} else if(tile == 163) {
					dir = {0, -1, 0};
				} else if(tile == 164) {
					dir = {-1, 0, 0};
				} else if(tile == 165) {
					dir = {1, 0, 0};
				} else if(tile == 166) {
					dir = {0, 0, 1};
				} else if(tile == 167) {
					dir = {0, 0, -1};
				}
				op.direction = dir;
				op.delay = 200;
				scheduled.push_back(op);
			}
		}
	}
}
static void updateRepeaters(unsigned char* level) {
	for(int i = 0; i < repeaters.size(); i++) {
		vector<int> repeater = repeaters[i];
		int tile = (*Level_getTile)(level, repeater[0], repeater[1], repeater[2]);
		if(tile == 0) {
			vector<vector<int>>::iterator it1 = repeaters.begin() + i;
			repeaters.erase(it1);
			vector<carrier>::iterator it2;
			carrier match;
			match.point = repeater;
			match.power = 0;
			for(int j = 0; j < enigma.size(); j++) {
				carrier c = enigma[j];
				if(c.point == match.point) {
					it2 = enigma.begin() + j;
					break;
				}
			}
			enigma.erase(it2);
		} else {
			vector<int> dir;
			if(tile == 168) {
				dir = {0, 0, 1};
			} else if(tile == 169) {
				dir = {0, 0, -1};
			} else if(tile == 170) {
				dir = {-1, 0, 0};
			} else if(tile == 171) {
				dir = {1, 0, 0};
			}
			int power = getEnigmaStrength(repeater[0] + dir[0], repeater[1] + dir[1], repeater[2] + dir[2]);
			int aux = (*Level_getData)(level, repeater[0], repeater[1], repeater[2]);
			if(power > 0 && aux == 0) {
				operation op;
				op.initTime = int(clock());
				op.type = operationType::ACTIVATE_REPEATER;
				op.block = repeater;
				op.delay = 100;
				scheduled.push_back(op);
			} else if(power == 0 && aux == 1) {
				operation op;
				op.initTime = int(clock());
				op.type = operationType::DEACTIVATE_REPEATER;
				op.block = repeater;
				op.delay = 100;
				scheduled.push_back(op);
			}
		}
	}
}
static void resolveSchedules(unsigned char* level) {
	int time = int(clock());
	for(int i = 0; i < scheduled.size(); i++) {
		operation op = scheduled[i];
		if(time - op.initTime >= op.delay) {
			if(op.type == operationType::SET_ENIGMA) {
				setEnigmaStrength(op.block[0], op.block[1], op.block[2], op.strength);
			} else if(op.type == operationType::PISTON) {
				int tile = (*Level_getTile)(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2]);
				int data = (*Level_getData)(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2]);
				Level_setTileAndData_addToEnigma(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2], 0, 0);
				Level_setTileAndData_addToEnigma(level, op.block[0] + 2 * op.direction[0], op.block[1] + 2 * op.direction[1], op.block[2] + 2 * op.direction[2], tile, data);
				int j = 0;
				while(pistons[j] != op.block) {
					j++;
				}
				pistonActivities[j] = true;
			} else if(op.type == operationType::PISTON_RETRACT) {
				int j = 0;
				while(pistons[j] != op.block) {
					j++;
				}
				pistonActivities[j] = false;
			} else if(op.type == operationType::STICKY_PISTON) {
				int tile = (*Level_getTile)(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2]);
				int data = (*Level_getData)(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2]);
				Level_setTileAndData_addToEnigma(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2], 0, 0);
				Level_setTileAndData_addToEnigma(level, op.block[0] + 2 * op.direction[0], op.block[1] + 2 * op.direction[1], op.block[2] + 2 * op.direction[2], tile, data);
				int j = 0;
				while(spistons[j] != op.block) {
					j++;
				}
				spistonActivities[j] = true;
			} else if(op.type == operationType::STICKY_PISTON_RETRACT) {
				int tile = (*Level_getTile)(level, op.block[0] + 2 * op.direction[0], op.block[1] + 2 * op.direction[1], op.block[2] + 2 * op.direction[2]);
				int data = (*Level_getData)(level, op.block[0] + 2 * op.direction[0], op.block[1] + 2 * op.direction[1], op.block[2] + 2 * op.direction[2]);
				Level_setTileAndData_addToEnigma(level, op.block[0] + 2 * op.direction[0], op.block[1] + 2 * op.direction[1], op.block[2] + 2 * op.direction[2], 0, 0);
				Level_setTileAndData_addToEnigma(level, op.block[0] + op.direction[0], op.block[1] + op.direction[1], op.block[2] + op.direction[2], tile, data);
				int j = 0;
				while(spistons[j] != op.block) {
					j++;
				}
				spistonActivities[j] = false;
			} else if(op.type == operationType::ACTIVATE_REPEATER) {
				int tile = (*Level_getTile)(level, op.block[0], op.block[1], op.block[2]);
				Level_setTileAndData_injection(level, op.block[0], op.block[1], op.block[2], tile, 1);
			} else if(op.type == operationType::DEACTIVATE_REPEATER) {
				int tile = (*Level_getTile)(level, op.block[0], op.block[1], op.block[2]);
				Level_setTileAndData_injection(level, op.block[0], op.block[1], op.block[2], tile, 0);
			}
			vector<operation>::iterator it = scheduled.begin() + i;
			scheduled.erase(it);
			i--;
		}
	}
}
static void update(unsigned char* minecraft) {
	mc = minecraft;
	unsigned char* level = *(unsigned char**) (minecraft + Minecraft_level_property_offset);
	unsigned char* player = *(unsigned char**) (minecraft + Minecraft_player_property_offset);
	if(player != NULL) {
		inGame = true;
		using namespace this_thread;
		thread ud(updateDusts, level);
		thread ut(updateTorches, level);
		thread up(updatePistons, level);
		thread us(updateStickyPistons, level);
		thread ur(updateRepeaters, level);
		thread rs(resolveSchedules, level);
		ud.join();
		ut.join();
		up.join();
		us.join();
		ur.join();
		rs.join();
	} else {
		if(inGame) {
			if(machineRecaller.joinable()) {
				mrStop = true;
				machineRecaller.join();
				mrStop = false;
			}
			inGame = false;
			machineRecaller = thread(recallMachines);
		}
	}
}
HOOK(SDL_PollEvent, int, (SDL_Event* event)) {
	ensure_SDL_PollEvent();
	int ret = (*real_SDL_PollEvent)(event);
	if(
		ret == 1 &&
		event != NULL &&
		event->type == SDL_MOUSEBUTTONDOWN &&
		event->button.button == SDL_BUTTON_RIGHT &&
		inGame
	) {
		unsigned char* player = *(unsigned char**) (mc + Minecraft_player_property_offset);
		unsigned char* inventory = *(unsigned char**) (player + Player_inventory_property_offset);
		ItemInstance* selected = (*Inventory_getSelected)(inventory);
		if(selected->id == 407) {
			unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
			HitResult hit = *(HitResult*) (mc + Minecraft_hit_result_property_offset);
			redstone_useOn(level, hit.x, hit.y, hit.z, hit.side);
		} else if(selected->id == 409) {
			unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
			HitResult hit = *(HitResult*) (mc + Minecraft_hit_result_property_offset);
			redstoneTorch_useOn(level, hit.x, hit.y, hit.z, hit.side);
		} else if(selected->id == 410) {
			unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
			HitResult hit = *(HitResult*) (mc + Minecraft_hit_result_property_offset);
			piston_useOn(level, hit.x, hit.y, hit.z, hit.side, *(unsigned char**) (mc + Minecraft_player_property_offset));
		} else if(selected->id == 411) {
			unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
			HitResult hit = *(HitResult*) (mc + Minecraft_hit_result_property_offset);
			spiston_useOn(level, hit.x, hit.y, hit.z, hit.side, *(unsigned char**) (mc + Minecraft_player_property_offset));
		} else if(selected->id == 412) {
			unsigned char* level = *(unsigned char**) (mc + Minecraft_level_property_offset);
			HitResult hit = *(HitResult*) (mc + Minecraft_hit_result_property_offset);
			repeater_useOn(level, hit.x, hit.y, hit.z, hit.side, *(unsigned char**) (mc + Minecraft_player_property_offset));
		}
	}
	return ret;
} 
static void Tile_initTiles_injection(unsigned char* null) {
	unsigned char* redstone = (unsigned char*) ::operator new(0x5c);
	(*Tile)(redstone, 151, 201, Material_wood);
	unsigned char* rvtable = (unsigned char*) ::operator new(0x104);
	memcpy(rvtable, Tile_vtable, 0x104);
	*(unsigned char**) redstone = rvtable;
	*(Tile_getTexture2_t*) (rvtable + Tile_getTexture2_vtable_offset) = &redstone_getTexture2;
	*(Tile_getTexture3_t*) (rvtable + Tile_getTexture3_vtable_offset) = &redstone_getTexture3;
	*(Tile_isSolidRender_t*) (rvtable + Tile_isSolidRender_vtable_offset) = &redstone_isSolidRender;
	*(Tile_getRenderLayer_t*) (rvtable + Tile_getRenderLayer_vtable_offset) = &redstone_getRenderLayer;
	*(Tile_updateDefaultShape_t*) (rvtable + Tile_updateDefaultShape_vtable_offset) = &redstone_updateDefaultShape;
	*(Tile_updateShape_t*) (rvtable + Tile_updateShape_vtable_offset) = &redstone_updateShape;
	*(Tile_getDescriptionId_t*) (rvtable + Tile_getDescriptionId_vtable_offset) = &redstone_getDescriptionId;
	Tile_setDestroyTime_t redstone_setDestroyTime = *(Tile_setDestroyTime_t*) (rvtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t redstone_setExplodeable = *(Tile_setExplodeable_t*) (rvtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t redstone_setSoundType = *(Tile_setSoundType_t*) (rvtable + Tile_setSoundType_vtable_offset);
	(*redstone_setDestroyTime)(redstone, 0);
	(*redstone_setExplodeable)(redstone, 1);
	(*redstone_setSoundType)(redstone, SOUND_STONE);
	Tile_tiles[151] = redstone; // Tile_init

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

	unsigned char* piston_up = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_up, 154, 0x6a, Material_wood);
	unsigned char* puvtable = (unsigned char*) ::operator new(0x104);
	memcpy(puvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_up = puvtable;
	*(Tile_getTexture2_t*) (puvtable + Tile_getTexture2_vtable_offset) = &piston_up_getTexture2;
	*(Tile_getTexture3_t*) (puvtable + Tile_getTexture3_vtable_offset) = &piston_up_getTexture3;
	setPistonProperties(piston_up, puvtable);
	Tile_tiles[154] = piston_up;

	// 155, 156 taken

	unsigned char* piston_down = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_down, 157, 0x6a, Material_wood);
	unsigned char* pdvtable = (unsigned char*) ::operator new(0x104);
	memcpy(pdvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_down = pdvtable;
	*(Tile_getTexture2_t*) (pdvtable + Tile_getTexture2_vtable_offset) = &piston_down_getTexture2;
	*(Tile_getTexture3_t*) (pdvtable + Tile_getTexture3_vtable_offset) = &piston_down_getTexture3;
	setPistonProperties(piston_down, pdvtable);
	Tile_tiles[157] = piston_down;

	unsigned char* piston_left = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_left, 158, 0x6a, Material_wood);
	unsigned char* plvtable = (unsigned char*) ::operator new(0x104);
	memcpy(plvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_left = plvtable;
	*(Tile_getTexture2_t*) (plvtable + Tile_getTexture2_vtable_offset) = &piston_left_getTexture2;
	*(Tile_getTexture3_t*) (plvtable + Tile_getTexture3_vtable_offset) = &piston_left_getTexture3;
	setPistonProperties(piston_left, plvtable);
	Tile_tiles[158] = piston_left;

	unsigned char* piston_right = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_right, 159, 0x6a, Material_wood);
	unsigned char* prvtable = (unsigned char*) ::operator new(0x104);
	memcpy(prvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_right = prvtable;
	*(Tile_getTexture2_t*) (prvtable + Tile_getTexture2_vtable_offset) = &piston_right_getTexture2;
	*(Tile_getTexture3_t*) (prvtable + Tile_getTexture3_vtable_offset) = &piston_right_getTexture3;
	setPistonProperties(piston_right, prvtable);
	Tile_tiles[159] = piston_right;

	unsigned char* piston_front = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_front, 160, 0x6a, Material_wood);
	unsigned char* pfvtable = (unsigned char*) ::operator new(0x104);
	memcpy(pfvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_front = pfvtable;
	*(Tile_getTexture2_t*) (pfvtable + Tile_getTexture2_vtable_offset) = &piston_front_getTexture2;
	*(Tile_getTexture3_t*) (pfvtable + Tile_getTexture3_vtable_offset) = &piston_front_getTexture3;
	setPistonProperties(piston_front, pfvtable);
	Tile_tiles[160] = piston_front;

	unsigned char* piston_back = (unsigned char*) ::operator new(0x5c);
	(*Tile)(piston_back, 161, 0x6a, Material_wood);
	unsigned char* pbvtable = (unsigned char*) ::operator new(0x104);
	memcpy(pbvtable, Tile_vtable, 0x104);
	*(unsigned char**) piston_back = pbvtable;
	*(Tile_getTexture2_t*) (pbvtable + Tile_getTexture2_vtable_offset) = &piston_back_getTexture2;
	*(Tile_getTexture3_t*) (pbvtable + Tile_getTexture3_vtable_offset) = &piston_back_getTexture3;
	setPistonProperties(piston_back, pbvtable);
	Tile_tiles[161] = piston_back;

	unsigned char* spiston_up = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_up, 162, 0x6a, Material_wood);
	unsigned char* spuvtable = (unsigned char*) ::operator new(0x104);
	memcpy(spuvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_up = spuvtable;
	*(Tile_getTexture2_t*) (spuvtable + Tile_getTexture2_vtable_offset) = &spiston_up_getTexture2;
	*(Tile_getTexture3_t*) (spuvtable + Tile_getTexture3_vtable_offset) = &spiston_up_getTexture3;
	setPistonProperties(spiston_up, spuvtable);
	Tile_tiles[162] = spiston_up;

	unsigned char* spiston_down = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_down, 163, 0x6a, Material_wood);
	unsigned char* spdvtable = (unsigned char*) ::operator new(0x104);
	memcpy(spdvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_down = spdvtable;
	*(Tile_getTexture2_t*) (spdvtable + Tile_getTexture2_vtable_offset) = &spiston_down_getTexture2;
	*(Tile_getTexture3_t*) (spdvtable + Tile_getTexture3_vtable_offset) = &spiston_down_getTexture3;
	setPistonProperties(spiston_down, spdvtable);
	Tile_tiles[163] = spiston_down;

	unsigned char* spiston_left = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_left, 164, 0x6a, Material_wood);
	unsigned char* splvtable = (unsigned char*) ::operator new(0x104);
	memcpy(splvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_left = splvtable;
	*(Tile_getTexture2_t*) (splvtable + Tile_getTexture2_vtable_offset) = &spiston_left_getTexture2;
	*(Tile_getTexture3_t*) (splvtable + Tile_getTexture3_vtable_offset) = &spiston_left_getTexture3;
	setPistonProperties(spiston_left, splvtable);
	Tile_tiles[164] = spiston_left;

	unsigned char* spiston_right = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_right, 165, 0x6a, Material_wood);
	unsigned char* sprvtable = (unsigned char*) ::operator new(0x104);
	memcpy(sprvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_right = sprvtable;
	*(Tile_getTexture2_t*) (sprvtable + Tile_getTexture2_vtable_offset) = &spiston_right_getTexture2;
	*(Tile_getTexture3_t*) (sprvtable + Tile_getTexture3_vtable_offset) = &spiston_right_getTexture3;
	setPistonProperties(spiston_right, sprvtable);
	Tile_tiles[165] = spiston_right;

	unsigned char* spiston_front = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_front, 166, 0x6a, Material_wood);
	unsigned char* spfvtable = (unsigned char*) ::operator new(0x104);
	memcpy(spfvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_front = spfvtable;
	*(Tile_getTexture2_t*) (spfvtable + Tile_getTexture2_vtable_offset) = &spiston_front_getTexture2;
	*(Tile_getTexture3_t*) (spfvtable + Tile_getTexture3_vtable_offset) = &spiston_front_getTexture3;
	setPistonProperties(spiston_front, spfvtable);
	Tile_tiles[166] = spiston_front;

	unsigned char* spiston_back = (unsigned char*) ::operator new(0x5c);
	(*Tile)(spiston_back, 167, 0x6a, Material_wood);
	unsigned char* spbvtable = (unsigned char*) ::operator new(0x104);
	memcpy(spbvtable, Tile_vtable, 0x104);
	*(unsigned char**) spiston_back = spbvtable;
	*(Tile_getTexture2_t*) (spbvtable + Tile_getTexture2_vtable_offset) = &spiston_back_getTexture2;
	*(Tile_getTexture3_t*) (spbvtable + Tile_getTexture3_vtable_offset) = &spiston_back_getTexture3;
	setPistonProperties(spiston_back, spbvtable);
	Tile_tiles[167] = spiston_back;

	unsigned char* repeater_front = (unsigned char*) ::operator new(0x5c);
	(*Tile)(repeater_front, 168, 0x83, Material_wood);
	unsigned char* rfvtable = (unsigned char*) ::operator new(0x104);
	memcpy(rfvtable, Tile_vtable, 0x104);
	*(unsigned char**) repeater_front = rfvtable;
	*(Tile_getTexture3_t*) (rfvtable + Tile_getTexture3_vtable_offset) = &repeater_front_getTexture3;
	setRepeaterProperties(repeater_front, rfvtable);
	Tile_tiles[168] = repeater_front;

	unsigned char* repeater_back = (unsigned char*) ::operator new(0x5c);
	(*Tile)(repeater_back, 169, 0x83, Material_wood);
	unsigned char* rbvtable = (unsigned char*) ::operator new(0x104);
	memcpy(rbvtable, Tile_vtable, 0x104);
	*(unsigned char**) repeater_back = rbvtable;
	*(Tile_getTexture3_t*) (rbvtable + Tile_getTexture3_vtable_offset) = &repeater_back_getTexture3;
	setRepeaterProperties(repeater_back, rbvtable);
	Tile_tiles[169] = repeater_back;

	unsigned char* repeater_left = (unsigned char*) ::operator new(0x5c);
	(*Tile)(repeater_left, 170, 0x83, Material_wood);
	unsigned char* rlvtable = (unsigned char*) ::operator new(0x104);
	memcpy(rlvtable, Tile_vtable, 0x104);
	*(unsigned char**) repeater_left = rlvtable;
	*(Tile_getTexture3_t*) (rlvtable + Tile_getTexture3_vtable_offset) = &repeater_left_getTexture3;
	setRepeaterProperties(repeater_left, rlvtable);
	Tile_tiles[170] = repeater_left;

	unsigned char* repeater_right = (unsigned char*) ::operator new(0x5c);
	(*Tile)(repeater_right, 171, 0x83, Material_wood);
	unsigned char* rrvtable = (unsigned char*) ::operator new(0x104);
	memcpy(rrvtable, Tile_vtable, 0x104);
	*(unsigned char**) repeater_right = rrvtable;
	*(Tile_getTexture3_t*) (rrvtable + Tile_getTexture3_vtable_offset) = &repeater_right_getTexture3;
	setRepeaterProperties(repeater_right, rrvtable);
	Tile_tiles[171] = repeater_right;
}
static void Item_initItems_injection(unsigned char* null) {
	// setDescriptionId is a broken function (never works and often crashes); NEVER USE!!!
	unsigned char* redstone = (unsigned char*) ::operator new(0x38);
	(*Item)(redstone, 151);
	unsigned char* rvtable = *(unsigned char**) redstone;
	Item_setIcon_t redstone_setIcon = *(Item_setIcon_t*) (rvtable + Item_setIcon_vtable_offset);
	//Item_setDescriptionId_t redstone_setDescriptionId = *(Item_setDescriptionId_t*) (rvtable + Item_setDescriptionId_vtable_offset);
	(*redstone_setIcon)(redstone, 8, 3);
	//const string name1 = "reddust";
	//(*redstone_setDescriptionId)(redstone, name1);
	*(int*) (redstone + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (redstone + Item_category_property_offset) = 1;
	*(int*) (redstone + Item_max_damage_property_offset) = 250;
	*(int*) (redstone + Item_max_stack_size_property_offset) = 64;

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

	unsigned char* piston = (unsigned char*) ::operator new(0x38);
	(*Item)(piston, 154);
	unsigned char* pvtable = *(unsigned char**) piston;
	Item_setIcon_t piston_setIcon = *(Item_setIcon_t*) (pvtable + Item_setIcon_vtable_offset);
	//Item_setDescriptionId_t piston_setDescriptionId = *(Item_setDescriptionId_t*) (pvtable + Item_setDescriptionId_vtable_offset);
	(*piston_setIcon)(piston, 3, 9);
	//const string name3 = "piston";
	//(*piston_setDescriptionId)(piston, name3);
	*(int*) (piston + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (piston + Item_category_property_offset) = 1;
	*(int*) (piston + Item_max_damage_property_offset) = 250;
	*(int*) (piston + Item_max_stack_size_property_offset) = 64;

	unsigned char* stickyPiston = (unsigned char*) ::operator new(0x38);
	(*Item)(stickyPiston, 155);
	unsigned char* spvtable = *(unsigned char**) stickyPiston;
	Item_setIcon_t spiston_setIcon = *(Item_setIcon_t*) (spvtable + Item_setIcon_vtable_offset);
	(*spiston_setIcon)(stickyPiston, 4, 9);
	*(int*) (stickyPiston + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (stickyPiston + Item_category_property_offset) = 1;
	*(int*) (stickyPiston + Item_max_damage_property_offset) = 250;
	*(int*) (stickyPiston + Item_max_stack_size_property_offset) = 64;

	unsigned char* repeater = (unsigned char*) ::operator new(0x38);
	(*Item)(repeater, 156);
	unsigned char* revtable = *(unsigned char**) repeater;
	Item_setIcon_t repeater_setIcon = *(Item_setIcon_t*) (revtable + Item_setIcon_vtable_offset);
	(*repeater_setIcon)(repeater, 6, 5);
	*(int*) (repeater + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (repeater + Item_category_property_offset) = 1;
	*(int*) (repeater + Item_max_damage_property_offset) = 250;
	*(int*) (repeater + Item_max_stack_size_property_offset) = 64;

	unsigned char* ruby = (unsigned char*) ::operator new(0x38);
	(*Item)(ruby, 157);
	unsigned char* ruvtable = *(unsigned char**) ruby;
	Item_setIcon_t ruby_setIcon = *(Item_setIcon_t*) (ruvtable + Item_setIcon_vtable_offset);
	(*ruby_setIcon)(ruby, 6, 6);
	*(int*) (ruby + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (ruby + Item_category_property_offset) = 2;
	*(int*) (ruby + Item_max_damage_property_offset) = 250;
	*(int*) (ruby + Item_max_stack_size_property_offset) = 64;
}
static void Inventory_setupDefault_FillingContainer_addItem_call_injection(unsigned char* fc) {
	ItemInstance* ruby = new ItemInstance;
	ruby->count = 255;
	ruby->auxiliary = 0;
	ruby->id = 413;
	(*FillingContainer_addItem)(fc, ruby);

	ItemInstance* redstone = new ItemInstance;
	redstone->count = 255;
	redstone->auxiliary = 0;
	redstone->id = 407;
	(*FillingContainer_addItem)(fc, redstone);

	ItemInstance* redstoneTorch = new ItemInstance;
	redstoneTorch->count = 255;
	redstoneTorch->auxiliary = 0;
	redstoneTorch->id = 409;
	(*FillingContainer_addItem)(fc, redstoneTorch);

	ItemInstance* piston = new ItemInstance;
	piston->count = 255;
	piston->auxiliary = 0;
	piston->id = 410;
	(*FillingContainer_addItem)(fc, piston);

	ItemInstance* stickyPiston = new ItemInstance;
	stickyPiston->count = 255;
	stickyPiston->auxiliary = 0;
	stickyPiston->id = 411;
	(*FillingContainer_addItem)(fc, stickyPiston);

	ItemInstance* repeater = new ItemInstance;
	repeater->count = 255;
	repeater->auxiliary = 0;
	repeater->id = 412;
	(*FillingContainer_addItem)(fc, repeater);
}
static void Recipes_injection(unsigned char* recipes) {
	Recipes_Type redstone_dust1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 407,
			.auxiliary = 0
		},
		.letter = 'a'
	};
	Recipes_Type stick1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 280,
			.auxiliary = 0
		},
		.letter = 'b'
	};
	Recipes_Type redstone_torch2 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 2,
			.id = 409,
			.auxiliary = 0
		},
		.letter = 'c'
	};
	Recipes_Type stone3 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 3,
			.id = 1,
			.auxiliary = 0
		},
		.letter = 'd'
	};
	Recipes_Type cobblestone4 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 4,
			.id = 4,
			.auxiliary = 0
		},
		.letter = 'e'
	};
	Recipes_Type wooden_planks3 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 3,
			.id = 5,
			.auxiliary = 0
		},
		.letter = 'f'
	};
	Recipes_Type iron_ingot1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 265,
			.auxiliary = 0
		},
		.letter = 'g'
	};
	Recipes_Type piston1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 410,
			.auxiliary = 0
		},
		.letter = 'h'
	};
	Recipes_Type slime_ball1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 341,
			.auxiliary = 0
		},
		.letter = 'i'
	};
	Recipes_Type ruby1 = {
		.item = 0,
		.tile = 0,
		.instance = {
			.count = 1,
			.id = 413,
			.auxiliary = 0
		},
		.letter = 'j'
	};

	// Redstone torch
	ItemInstance const& result1 = {
		.count = 1,
		.id = 409,
		.auxiliary = 0
	};
	/*string const& r1r1 = " a ";
	string const& r1r2 = " b ";*/
	vector<Recipes_Type> const& r1v = {redstone_dust1, stick1};
	//(*Recipes_addShapedRecipe_2)(recipes, result1, r1r1, r1r2, r1v);
	(*Recipes_addShapelessRecipe)(recipes, result1, r1v);

	// Redstone repeater
	ItemInstance const& result2 = {
		.count = 1,
		.id = 412,
		.auxiliary = 0
	};
	/*string const& r2r1 = "cac";
	string const& r2r2 = "ddd";*/
	vector<Recipes_Type> const& r2v = {redstone_dust1, redstone_torch2, stone3};
	//(*Recipes_addShapedRecipe_2)(recipes, result2, r2r1, r2r2, r2v);
	(*Recipes_addShapelessRecipe)(recipes, result2, r2v);

	// Piston
	ItemInstance const& result3 = {
		.count = 1,
		.id = 410,
		.auxiliary = 0
	};
	/*string const& r3r1 = "fff";
	string const& r3r2 = "ege";
	string const& r3r3 = "eae";*/
	vector<Recipes_Type> const& r3v = {redstone_dust1, cobblestone4, iron_ingot1, wooden_planks3};
	//(*Recipes_addShapedRecipe_3)(recipes, result3, r3r1, r3r2, r3r3, r3v);
	(*Recipes_addShapelessRecipe)(recipes, result3, r3v);

	// Sticky piston
	ItemInstance const& result4 = {
		.count = 1,
		.id = 411,
		.auxiliary = 0
	};
	/*string const& r4r1 = " i ";
	string const& r4r2 = " h ";*/
	vector<Recipes_Type> const& r4v = {slime_ball1, piston1};
	//(*Recipes_addShapedRecipe_2)(recipes, result4, r4r1, r4r2, r4v);
	(*Recipes_addShapelessRecipe)(recipes, result4, r4v);

	// Redstone dust
	ItemInstance const& result5 = {
		.count = 5,
		.id = 407,
		.auxiliary = 0
	};
	//string const& r5r1 = " j ";
	vector<Recipes_Type> const& r5v = {ruby1};
	//(*Recipes_addShapedRecipe_1)(recipes, result5, r5r1, r5v);
	(*Recipes_addShapelessRecipe)(recipes, result5, r5v);
}
static void FurnaceRecipes_injection(unsigned char* recipes) {
	// Ruby
	ItemInstance result = {
		.count = 1,
		.id = 413,
		.auxiliary = 0
	};
	(*FurnaceRecipes_addFurnaceRecipe)(recipes, 73, result);
	(*FurnaceRecipes_addFurnaceRecipe)(recipes, 74, result); // There seem to be two consecutive different types of redstone ore
}
__attribute__((constructor)) static void init() {
	overwrite_calls((void*) Level_setTileAndData, (void*) Level_setTileAndData_injection);
	misc_run_on_tiles_setup(Tile_initTiles_injection);
	misc_run_on_items_setup(Item_initItems_injection);
	misc_run_on_creative_inventory_setup(Inventory_setupDefault_FillingContainer_addItem_call_injection);
	misc_run_on_recipes_setup(Recipes_injection);
	misc_run_on_furnace_recipes_setup(FurnaceRecipes_injection);
	misc_run_on_update(update);
	machineRecaller = thread(recallMachines);
}
__attribute__((destructor)) static void fini() {
	mrStop = true;
	if(machineRecaller.joinable()) {
		machineRecaller.join();
	}
}