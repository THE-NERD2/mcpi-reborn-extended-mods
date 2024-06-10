#include "template.hpp"
/*extern unsigned char* mc;
extern vector<vector<int>> redstoneDusts;
extern vector<carrier> enigma;
extern bool isc(int, int, int);
extern vector<int> processHit(int, int, int, int);
extern void Level_setTileAndData_addToEnigma(unsigned char*, int, int, int, int, int);
extern int getEnigmaStrength(int, int, int);
extern void setEnigmaStrength(int, int, int, int);
extern void Level_setTileAndData_injection(unsigned char*, int, int, int, int, int);*/

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
static AABB* redstone_getAABB(unsigned char* tile, unsigned char* level, int x, int y, int z) {
	AABB* aabb = (AABB*) (tile + Tile_AABB_property_offset);
	aabb->x1 = x;
	aabb->y1 = y;
	aabb->z1 = z;
	aabb->x2 = x + 1;
	aabb->y2 = y;
	aabb->z2 = z + 1;
	return aabb;
}
static void redstone_useOn(unsigned char* level, int x, int y, int z, int side) {
	vector<int> point = processHit(x, y, z, side);
	Level_setTileAndData_addToEnigma(level, point[0], point[1], point[2], 407, 0);
}

static void make_redstone_dust_tile() {
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
	*(Tile_getAABB_t*) (rvtable + Tile_getAABB_vtable_offset) = &redstone_getAABB;
	Tile_setDestroyTime_t redstone_setDestroyTime = *(Tile_setDestroyTime_t*) (rvtable + Tile_setDestroyTime_vtable_offset);
	Tile_setExplodeable_t redstone_setExplodeable = *(Tile_setExplodeable_t*) (rvtable + Tile_setExplodeable_vtable_offset);
	Tile_setSoundType_t redstone_setSoundType = *(Tile_setSoundType_t*) (rvtable + Tile_setSoundType_vtable_offset);
	(*redstone_setDestroyTime)(redstone, 0);
	(*redstone_setExplodeable)(redstone, 1);
	(*redstone_setSoundType)(redstone, SOUND_STONE);
	Tile_tiles[151] = redstone; // Tile_init
}
static void make_redstone_dust_item() {
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