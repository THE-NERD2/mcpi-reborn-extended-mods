#include "template.hpp"
#include "items.hpp"
#include "logic.hpp"

__attribute__((constructor)) static void init() {
    misc_run_on_update(update);
    misc_run_on_items_setup(Item_initItems_injection);
    misc_run_on_creative_inventory_setup(Inventory_setupDefault_FillingContainer_addItem_call_injection);
}