/* This is a managed file. Do not delete this comment. */

#include <supplychain/supplychain.h>

static
supplychain_Storage find_storage(
    supplychain_Factory this,
    supplychain_Product product)
{
    /* Get full path of factory */
    corto_id factory_id;
    corto_fullpath(factory_id, this);

    /* Find storages in this factory */
    corto_iter it;
    if (corto_select("*")
        .from(factory_id)
        .instanceof("supplychain/Storage")
        .iter_objects(&it))
    {
        corto_throw("failed to find storages");
        goto error;
    }

    while (corto_iter_hasNext(&it)) {
        supplychain_Storage s = corto_iter_next(&it);

        if (s->stores == product) {
            /* Prematurely stop iteration, need to release resources */
            corto_iter_release(&it);
            return s;
        }
    }

error:
    return NULL;
}

static
supplychain_ProductQuantity* find_internal_demand(
    supplychain_Factory this,
    supplychain_Product product)
{
    corto_iter it = corto_ll_iter(this->internal_demand);
    while (corto_iter_hasNext(&it)) {
        supplychain_ProductQuantity *q = corto_iter_next(&it);
        if (q->product == product) {
            return q;
        }
    }

    return NULL;
}

static
int16_t calculate_demand(
    supplychain_Factory this,
    supplychain_Product product,
    uint32_t demand)
{
    supplychain_Storage s = find_storage(this, product);
    if (!s) {
        corto_throw("no storage found for '%s'", corto_fullpath(NULL, product));
        goto error;
    }

    /* Compute demand for all products */
    corto_iter it = corto_ll_iter(product->requires);
    while (corto_iter_hasNext(&it)) {
        supplychain_ProductQuantity
            *q = corto_iter_next(&it),
            *factory_q = find_internal_demand(this, q->product);

        /* If quantity not found, create it */
        if (!factory_q) {
            factory_q = corto_ptr_new(supplychain_ProductQuantity_o);
            factory_q->product = q->product;
            corto_ll_append(this->internal_demand, factory_q);
        }

        /* Update demand for factory */
        uint32_t needed = demand * q->quantity;
        supplychain_Storage s = find_storage(this, factory_q->product);

        if (needed < s->units_stored) {
            factory_q->quantity = 0;
        } else {
            factory_q->quantity = needed - s->units_stored;
        }

        /* Recursively update demand for product */
        if (calculate_demand(this, q->product, factory_q->quantity)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
bool produce(
    supplychain_Factory this,
    supplychain_Product product)
{
    bool can_produce = true;

    /* Step 1: check if resources are available to start production */
    corto_iter it = corto_ll_iter(product->requires);
    while (corto_iter_hasNext(&it)) {
        supplychain_ProductQuantity *q = corto_iter_next(&it);
        supplychain_Storage s = find_storage(this, q->product);

        if (q->quantity > s->units_stored) {
            can_produce = false;
        }
    }

    /* Step 2: enable eqpt if resources are available, and decrease storage */
    if (can_produce) {
        it = corto_ll_iter(product->requires);
        while (corto_iter_hasNext(&it)) {
            supplychain_ProductQuantity *q = corto_iter_next(&it);
            supplychain_Storage s = find_storage(this, q->product);
            s->units_stored -= q->quantity;
        }

    }

    return can_produce;
}

static
void start_equipment(
    supplychain_Factory this,
    supplychain_Factory_Equipment e)
{
    if (produce(this, e->produces)) {
        e->operational_state = Supplychain_On;
    }
}

int16_t supplychain_Factory_validate(
    supplychain_Factory this)
{
    /* Get full path of factory */
    corto_id factory_id;
    corto_fullpath(factory_id, this);

    supplychain_ProductQuantity own_demand = {
        this->produces,
        this->demand
    };

    /* Recalculate demand for different products */
    if (calculate_demand(this, this->produces, this->demand)) {
        goto error;
    }

    /* Find all equipments in this factory */
    corto_iter it;
    if (corto_select("Equipment/*")
        .from(factory_id)
        .instanceof("supplychain/Factory/Equipment")
        .iter_objects(&it))
    {
        corto_throw("failed to find equipments");
        goto error;
    }

    this->power_consumption = 0;

    /* Update all equipments */
    while (corto_iter_hasNext(&it)) {
        supplychain_Factory_Equipment e = corto_iter_next(&it);
        supplychain_ProductQuantity *demand = find_internal_demand(this, e->produces);

        /* No internal demand record is created for the top-level resource, which
         * is what the factory as a whole is producing. If returning NULL for
         * demand, point to own demand */
        if (!demand) {
            demand = &own_demand;
        }

        /* Find storage for product */
        supplychain_Storage s = find_storage(this, e->produces);

        /* If equipement is idle, it finished a task, transfer to storage */
        if (e->operational_state == Supplychain_Idle) {
            s->units_stored ++;
            e->operational_state = Supplychain_Off;

            /* Decrease demand by one */
            if (demand->quantity) {
                demand->quantity --;
            }
        }

        /* Check if there is still demand for the product */
        if (demand->quantity && e->operational_state == Supplychain_Off)
        {
            /* Attempt to turn on equipment. Will fail if resources aren't
             * available to produce the product */
            start_equipment(this, e);

        }

        if (!demand->quantity && e->operational_state != Supplychain_Off) {
            /* If there is no demand, turn off equipment */
            e->operational_state = Supplychain_Off;
        }

        /* This will trigger the equipment to process the next step */
        if (corto_update(e)) {
            goto error;
        }

        /* Update storage so subscribers will be notified of any changes */
        corto_update(s);

        /* If equipment is on, add its power output to total factory output */
        if (e->operational_state == Supplychain_On) {
            this->power_consumption += e->power_consumption;
        }
    }

    return 0;
error:
    return -1;
}
