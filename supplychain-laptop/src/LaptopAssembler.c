/* This is a managed file. Do not delete this comment. */

#include <supplychain/laptop/laptop.h>

int16_t laptop_LaptopAssembler_construct(
    laptop_LaptopAssembler this)
{
    supplychain_EquipmentComponent
        place_cpu = corto_lookup(this, "place_cpu"),
        place_ram = corto_lookup(this, "place_ram"),
        place_ssd = corto_lookup(this, "place_ssd"),
        place_screen = corto_lookup(this, "place_screen"),
        assemble = corto_lookup(this, "assemble");

    place_cpu->power_consumption = 500;
    place_ram->power_consumption = 250;
    place_ssd->power_consumption = 375;
    place_screen->power_consumption = 250;
    assemble->power_consumption = 500;

    /* Append components to process step list. Transfer ownership of lookup to
     * the list (no need to do corto_release) */
    supplychain_Factory_Equipment eqpt = supplychain_Factory_Equipment(this);

    corto_ll_append(eqpt->process_steps, place_cpu);
    corto_ll_append(eqpt->process_steps, place_ram);
    corto_ll_append(eqpt->process_steps, place_ssd);
    corto_ll_append(eqpt->process_steps, place_screen);
    corto_ll_append(eqpt->process_steps, assemble);

    return 0;
}
