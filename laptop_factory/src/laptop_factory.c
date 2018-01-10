#include <include/laptop_factory.h>

/* Load factory configuration from project etc directory */
int load_configuration(void)
{
    const char *dir = LAPTOP_FACTORY_ETC;

    corto_iter it;
    if (corto_dir_iter(dir, NULL, &it)) {
        corto_throw("failed to open directory '%s'", dir);
        goto error;
    }

    while (corto_iter_hasNext(&it)) {
        const char *file = corto_iter_next(&it);
        if (corto_use(strarg("%s/%s", dir, file), 0, NULL)) {
            corto_throw("failed to load file '%s'", file);
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

/* Application entrypoint */
int cortomain(int argc, char *argv[]) {
    corto_iter it;

    corto_log_verbositySet(CORTO_TRACE);

    if (load_configuration()) {
        goto error;
    }

    /* Find all instances of Factory objects and store them in a list */
    corto_ll factories = corto_ll_new();
    if (corto_select("data/*").type("/supplychain/Factory").iter_objects(&it)) {
        goto error;
    }

    while (corto_iter_hasNext(&it)) {
        corto_object f = corto_iter_next(&it);
        corto_ll_append(factories, f);
    }

    if (!corto_ll_count(factories)) {
        corto_throw("no factories found in configuration!");
        goto error;
    }

    /* Periodically update all factories */
    while (true) {
        it = corto_ll_iter(factories);
        while (corto_iter_hasNext(&it)) {
            corto_object f = corto_iter_next(&it);
            corto_update(f);
        }
        corto_sleep(1, 0);
    }

    return 0;
error:
    return -1;
}
