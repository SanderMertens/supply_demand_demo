/* This is a managed file. Do not delete this comment. */

#include <supplychain/supplychain.h>
#define DEFAULT_TEMPERATURE (293)
#define HEATUP_RATIO (95)
#define COOLDOWN_RATIO (50)

int16_t supplychain_EquipmentComponent_validate(
    supplychain_EquipmentComponent this)
{
    if (this->operational_state == Supplychain_On) {
        this->temperature.value += this->power_consumption / HEATUP_RATIO;
    } else {
        if (this->temperature.value) {
            this->temperature.value += ((DEFAULT_TEMPERATURE - this->temperature.value) / COOLDOWN_RATIO);
        }
    }

    if (this->operational_state == Supplychain_Error) {
        if (this->temperature.value < this->temperature.hi.medium_alert) {
            this->operational_state = Supplychain_Idle;
        }
    }

    if (this->temperature.value > (this->temperature.hi.high_alert + 50)) {
        this->operational_state = Supplychain_Error;
    }

    return 0;
}

int16_t supplychain_EquipmentComponent_init(
    supplychain_EquipmentComponent this)
{
    /* Initialize temperature, add some chaos */
    this->temperature.value = DEFAULT_TEMPERATURE + ((double)rand() / (double)RAND_MAX) * 100;
    return 0;
}
