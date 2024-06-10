#include "template.hpp"
#include "redstone_dust.hpp"
#include "redstone_torch.hpp"
#include "repeater.hpp"
#include "pistons.hpp"
#include "ruby.hpp"

static void Tile_initTiles_injection(unsigned char* p0) {
    make_redstone_dust_tile();
    make_redstone_torch_tile();
    make_redstone_repeater_tile();
    make_piston_tile();
    make_spiston_tile();
}
static void Item_initItems_injection(unsigned char* p0) {
    make_redstone_dust_item();
    make_redstone_torch_item();
    make_redstone_repeater_item();
    make_piston_item();
    make_spiston_item();
    make_ruby_item();
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