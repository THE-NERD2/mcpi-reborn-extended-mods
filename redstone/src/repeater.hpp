#include "template.hpp"
/*extern unsigned char* mc;
extern vector<vector<int>> repeaters;
extern vector<carrier> enigma;
extern vector<operation> scheduled;
extern vector<int> processHit(int, int, int, int);
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);
extern int getEnigmaStrength(int, int, int);*/

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
static AABB* repeater_getAABB(unsigned char* tile, unsigned char* level, int x, int y, int z) {
	AABB* aabb = (AABB*) (tile + Tile_AABB_property_offset);
	aabb->x1 = x;
	aabb->y1 = y;
	aabb->z1 = z;
	aabb->x2 = x + 1;
	aabb->y2 = y + 0.0625;
	aabb->z2 = z + 1;
	return aabb;
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
	*(Tile_getAABB_t*) (vtable + Tile_getAABB_vtable_offset) = &repeater_getAABB;
	Tile_setDestroyTime_t repeater_setDestroyTime = *(Tile_setDestroyTime_t*) (vtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t repeater_setExplodeable = *(Tile_setExplodeable_t*) (vtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t repeater_setSoundType = *(Tile_setSoundType_t*) (vtable + Tile_setSoundType_vtable_offset);
	(*repeater_setDestroyTime)(tile, 0);
	(*repeater_setExplodeable)(tile, 1);
	(*repeater_setSoundType)(tile, SOUND_WOOD);
}

static void make_redstone_repeater_tile() {
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
static void make_redstone_repeater_item() {
    unsigned char* repeater = (unsigned char*) ::operator new(0x38);
	(*Item)(repeater, 156);
	unsigned char* revtable = *(unsigned char**) repeater;
	Item_setIcon_t repeater_setIcon = *(Item_setIcon_t*) (revtable + Item_setIcon_vtable_offset);
	(*repeater_setIcon)(repeater, 6, 5);
	*(int*) (repeater + Item_is_stacked_by_data_property_offset) = 1;
	*(int*) (repeater + Item_category_property_offset) = 1;
	*(int*) (repeater + Item_max_damage_property_offset) = 250;
	*(int*) (repeater + Item_max_stack_size_property_offset) = 64;
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