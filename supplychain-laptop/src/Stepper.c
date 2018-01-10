/* This is a managed file. Do not delete this comment. */

#include <supplychain/laptop/laptop.h>

int16_t laptop_Stepper_construct(
    laptop_Stepper this)
{
    supplychain_EquipmentComponent
        wafer_loader = corto_lookup(this, "wafer_loader"),
        wafer_alignment = corto_lookup(this, "wafer_alignment"),
        illumination = corto_lookup(this, "illumination");

    wafer_loader->power_consumption = 500;
    wafer_alignment->power_consumption = 250;
    illumination->power_consumption = 2500;

    /* Append components to process step list. Transfer ownership of lookup to
     * the list (no need to do corto_release) */
    supplychain_Factory_Equipment eqpt = supplychain_Factory_Equipment(this);
    corto_ll_append(eqpt->process_steps, wafer_loader);
    corto_ll_append(eqpt->process_steps, wafer_alignment);
    corto_ll_append(eqpt->process_steps, illumination);
    corto_ll_append(eqpt->process_steps, wafer_loader);

    return 0;
}
