/**
 * @file ext_mcu_com.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief 外部マイコン通信
 * @version 0.1
 * @date 2026-01-31
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */

#include "ext_mcu_com.h"

static emc_config_t s_emc_confg;

bool emc_init(emc_config_t *p_config)
{
    bool ret;

    if(p_config->mode > EMC_MODE_SLAVE) {
        ret = false;
        s_emc_confg.mode = EMC_MODE_NONE;
    } else {
        s_emc_confg.mode         = p_config->mode;
        s_emc_confg.p_func_read  = p_config->p_func_read;
        s_emc_confg.p_func_write = p_config->p_func_write;
        ret = true;
    }
}

void emc_cmd_write(emc_cmd_frame_t *p_flame)
{
    if(s_emc_confg.mode != EMC_MODE_NONE) {
        s_emc_confg.p_func_write(p_flame);
    }
}

void emc_cmd_read(emc_cmd_frame_t *p_flame)
{
    if(s_emc_confg.mode != EMC_MODE_NONE) {
        s_emc_confg.p_func_read(p_flame);
    }
}