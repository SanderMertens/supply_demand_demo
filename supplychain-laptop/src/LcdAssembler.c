/* This is a managed file. Do not delete this comment. */

#include <supplychain/laptop/laptop.h>

int16_t laptop_LcdAssembler_construct(
    laptop_LcdAssembler this)
{
    supplychain_EquipmentComponent
        coating = corto_lookup(this, "coating"),
        lithography = corto_lookup(this, "lithography"),
        backlight = corto_lookup(this, "backlight");

    coating->power_consumption = 500;
    lithography->power_consumption = 250;
    backlight->power_consumption = 2500;

    /* Append components to process step list. Transfer ownership of lookup to
     * the list (no need to do corto_release) */
    supplychain_Factory_Equipment eqpt = supplychain_Factory_Equipment(this);
    corto_ll_append(eqpt->process_steps, coating);
    corto_ll_append(eqpt->process_steps, lithography);
    corto_ll_append(eqpt->process_steps, backlight);

    return 0;
}
