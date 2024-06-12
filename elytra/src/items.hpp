#include "template.hpp"

static void Item_initItems_injection(unsigned char* p0) {
    unsigned char* elytra = (unsigned char*) ::operator new(0x34);
    (*ArmorItem)(elytra, 157, Material_iron, 2, 1);
    unsigned char* evtable = *(unsigned char**) elytra;
    Item_setIcon_t elytra_setIcon = *(Item_setIcon_t*) (evtable + Item_setIcon_vtable_offset);
    (*elytra_setIcon)(elytra, 0, 9);
    *(int*) (elytra + Item_is_stacked_by_data_property_offset) = 1;
    *(int*) (elytra + Item_category_property_offset) = 2;
    *(int*) (elytra + Item_max_damage_property_offset) = 250;
    *(int*) (elytra + Item_max_stack_size_property_offset) = 1;

    unsigned char* rocket = (unsigned char*) ::operator new(0x34);
    (*Item)(rocket, 158);
    unsigned char* rvtable = *(unsigned char**) rocket;
    Item_setIcon_t rocket_setIcon = *(Item_setIcon_t*) (rvtable + Item_setIcon_vtable_offset);
    (*rocket_setIcon)(rocket, 6, 7);
    *(int*) (elytra + Item_is_stacked_by_data_property_offset) = 1;
    *(int*) (elytra + Item_category_property_offset) = 2;
    *(int*) (elytra + Item_max_damage_property_offset) = 250;
    *(int*) (elytra + Item_max_stack_size_property_offset) = 1;
}
static void Inventory_setupDefault_FillingContainer_addItem_call_injection(unsigned char* fc) {
    ItemInstance* elytra = new ItemInstance;
	elytra->count = 255;
	elytra->auxiliary = 0;
	elytra->id = 413;
	(*FillingContainer_addItem)(fc, elytra);

    ItemInstance* rocket = new ItemInstance;
    rocket->count = 255;
    rocket->auxiliary = 0;
    rocket->id = 414;
    (*FillingContainer_addItem)(fc, rocket);
}