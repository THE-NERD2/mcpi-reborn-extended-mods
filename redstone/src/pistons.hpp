#include "template.hpp"
/*extern vector<operation> scheduled;
extern vector<vector<int>> pistons;
extern vector<vector<int>> spistons;
extern vector<carrier> enigma;
extern vector<bool> pistonActivities;
extern vector<bool> spistonActivities;
extern vector<bool> pistonActPending;
extern vector<bool> spistonActPending;
extern vector<int> processHit(int, int, int, int);
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);
extern int getEnigmaStrength(int, int, int);*/

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
static AABB* piston_getAABB(unsigned char* tile, unsigned char* level, int x, int y, int z) {
	AABB* aabb = (AABB*) (tile + Tile_AABB_property_offset);
	aabb->x1 = x;
	aabb->y1 = y;
	aabb->z1 = z;
	aabb->x2 = x + 1;
	aabb->y2 = y + 1;
	aabb->z2 = z + 1;
	return aabb;
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
	*(Tile_getAABB_t*) (vtable + Tile_getAABB_vtable_offset) = &piston_getAABB;
	Tile_setDestroyTime_t piston_setDestroyTime = *(Tile_setDestroyTime_t*) (vtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t piston_setExplodeable = *(Tile_setExplodeable_t*) (vtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t piston_setSoundType = *(Tile_setSoundType_t*) (vtable + Tile_setSoundType_vtable_offset);
	(*piston_setDestroyTime)(tile, 10);
	(*piston_setExplodeable)(tile, 1);
	(*piston_setSoundType)(tile, SOUND_STONE);
}

static void make_piston_tile() {
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
}
static void make_spiston_tile() {
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
}
static void make_piston_item() {
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
}
static void make_spiston_item() {
    unsigned char* stickyPiston = (unsigned char*) ::operator new(0x38);
	(*Item)(stickyPiston, 155);
	unsigned char* spvtable = *(unsigned char**) stickyPiston;
	Item_setIcon_t spiston_setIcon = *(Item_setIcon_t*) (spvtable + Item_setIcon_vtable_offset);
	(*spiston_setIcon)(stickyPiston, 4, 9);
	*(int*) (stickyPiston + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (stickyPiston + Item_category_property_offset) = 1;
	*(int*) (stickyPiston + Item_max_damage_property_offset) = 250;
	*(int*) (stickyPiston + Item_max_stack_size_property_offset) = 64;
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