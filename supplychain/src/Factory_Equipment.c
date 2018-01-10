/* This is a managed file. Do not delete this comment. */

#include <supplychain/supplychain.h>

static
supplychain_EquipmentComponent find_component_leaf(
    supplychain_Factory_Equipment this,
    corto_leaf leaf)
{
    return corto_lookup(this, corto_idof(leaf));
}

int16_t supplychain_Factory_Equipment_validate(
    supplychain_Factory_Equipment this)
{
    /* If current_step is set, turn off component from previous step */
    if (this->current_step) {
        supplychain_EquipmentComponent ec = find_component_leaf(
            this,
            corto_ll_get(this->process_steps, this->current_step - 1));
        ec->operational_state = Supplychain_Off;
        corto_update(ec);
    }

    if (this->operational_state == Supplychain_On) {
        if (this->current_step >= corto_ll_count(this->process_steps)) {
            /* Finished processing */
            this->operational_state = Supplychain_Idle;
        } else {
            /* Process next step */
            this->current_step ++;

            /* Turn on component for current step */
            if (this->current_step) {
                supplychain_EquipmentComponent ec = find_component_leaf(
                    this,
                    corto_ll_get(this->process_steps, this->current_step - 1));
                ec->operational_state = Supplychain_On;
                corto_update(ec);

                /* Power consumption of equipment equal consumption of current
                 * component. */
                this->power_consumption = ec->power_consumption;
            }
        }
    }

    if (this->operational_state != Supplychain_On) {
        this->power_consumption = 0;
        this->current_step = 0;
    }

    return 0;
}
