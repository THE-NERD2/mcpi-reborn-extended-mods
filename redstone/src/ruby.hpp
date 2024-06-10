#include "template.hpp"

static void make_ruby_item() {
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