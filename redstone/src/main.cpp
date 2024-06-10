#include "template.hpp"

// Must define certain things before including the rest of the code

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

// Everything defined now, include other code here
#include "items.hpp"
#include "recall.hpp"

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