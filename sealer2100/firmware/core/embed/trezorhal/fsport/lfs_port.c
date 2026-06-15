#include "lfs_port.h"

static lfs_t lfs;
static lfs_port_t port;

lfs_port_t* lfs_port_init(const struct lfs_config* cfg) {
    port.lfs = &lfs;
    port.cfg = cfg;
    return &port;
}
