/* This is a managed file. Do not delete this comment. */

#include <supplychain/laptop/laptop.h>

int16_t laptop_SsdAssembler_construct(
    laptop_SsdAssembler this)
{
    supplychain_EquipmentComponent
        nand_flash_assembly = corto_lookup(this, "nand_flash_assembly"),
        wafer_dicing = corto_lookup(this, "wafer_dicing"),
        surface_mount = corto_lookup(this, "surface_mount");

    nand_flash_assembly->power_consumption = 500;
    wafer_dicing->power_consumption = 250;
    surface_mount->power_consumption = 2500;

    /* Append components to process step list. Transfer ownership of lookup to
     * the list (no need to do corto_release) */
    supplychain_Factory_Equipment eqpt = supplychain_Factory_Equipment(this);
    corto_ll_append(eqpt->process_steps, nand_flash_assembly);
    corto_ll_append(eqpt->process_steps, wafer_dicing);
    corto_ll_append(eqpt->process_steps, surface_mount);

    return 0;
}
