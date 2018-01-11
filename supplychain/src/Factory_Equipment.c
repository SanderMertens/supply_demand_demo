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

                /* Power consumption of equipment equal consumption of current
                 * component. */
                this->power_consumption = ec->power_consumption;
            }
        }
    }

    /* Update all components */
    corto_id eqpt_id;
    corto_iter it;
    corto_fullpath(eqpt_id, this);
    if (corto_select("*").from(eqpt_id).iter_objects(&it)) {
        corto_throw(NULL);
        goto error;
    }

    while(corto_iter_hasNext(&it)) {
        corto_object component = corto_iter_next(&it);
        corto_update(component);
    }

    /* Reset power level and step if not on */
    if (this->operational_state != Supplychain_On) {
        this->power_consumption = 0;
        this->current_step = 0;
    }

    return 0;
error:
    return -1;
}
