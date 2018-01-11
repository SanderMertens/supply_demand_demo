/* This is a managed file. Do not delete this comment. */

#include <supplychain/supplychain.h>
#define DEFAULT_TEMPERATURE (293)
#define HEATUP_RATIO (75)
#define COOLDOWN_RATIO (50)

int16_t supplychain_EquipmentComponent_validate(
    supplychain_EquipmentComponent this)
{
    if (this->operational_state == Supplychain_On) {
        this->temperature += this->power_consumption / HEATUP_RATIO;
    } else {
        if (this->temperature) {
            this->temperature += ((DEFAULT_TEMPERATURE - this->temperature) / COOLDOWN_RATIO);
        }
    }

    return 0;
}

int16_t supplychain_EquipmentComponent_init(
    supplychain_EquipmentComponent this)
{
    this->temperature = DEFAULT_TEMPERATURE;
    return 0;
}
