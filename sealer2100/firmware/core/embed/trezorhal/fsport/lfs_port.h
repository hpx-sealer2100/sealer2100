#ifndef __LFS_PORT_H__
#define __LFS_PORT_H__
#include "lfs.h"

#define LFS_USE_NAND 1
#define LFS_USE_EMMC 1

typedef struct {
    lfs_t* lfs;
    const struct lfs_config* cfg;
}lfs_port_t;

lfs_port_t* lfs_port_init(const struct lfs_config* cfg);

#if LFS_USE_NAND == 1
const struct lfs_config* lfs_port_nand_config(void);
#endif

#if LFS_USE_EMMC == 1
const struct lfs_config* lfs_port_emmc_config(void);
#endif

#endif
