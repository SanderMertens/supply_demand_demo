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

    corto_log_verbositySet(CORTO_TRACE);

    if (load_configuration()) {
        goto error;
    }

    /* Keep alive */
    while (true) {
        corto_sleep(1, 0);
    }

    return 0;
error:
    return -1;
}
