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

// --------------------------------------------------------------------------
// [Private]
typedef void(*p_func_cmd_cbk)(uint8_t *p_data, uint32_t data_len);
void emc_cmd_config_cbk(uint8_t *p_data, uint32_t data_len);
typedef struct {
    uint8_t cmd;
    p_func_cmd_cbk p_cbk;
} emc_cmd_tbl_data_t;

const emc_cmd_tbl_data_t g_emc_cmd_tbl[] = {
    {0x10, emc_cmd_config_cbk}
};

static emc_config_t s_emc_confg;
static bool s_is_init_fail = false;
// --------------------------------------------------------------------------
// [Static関数]

void emc_cmd_config_cbk(uint8_t *p_data, uint32_t data_len)
{
}

// --------------------------------------------------------------------------
// [API関数]

bool emc_init(emc_config_t *p_config)
{
    bool ret;

    if((p_config->mode > EMC_MODE_SLAVE) ||
        (p_config->p_func_read == NULL) ||
        (p_config->p_func_write == NULL) ||
        (p_config->p_func_delay == NULL)
    ){
        s_emc_confg.mode = EMC_MODE_NONE;
        ret = false;
    } else {
        s_emc_confg.mode         = p_config->mode;
        s_emc_confg.p_func_read  = p_config->p_func_read;
        s_emc_confg.p_func_write = p_config->p_func_write;
        s_emc_confg.p_func_delay = p_config->p_func_delay;
        ret = true;
    }

    s_is_init_fail = ret;
    return ret;
}

void emc_cmd_write(emc_cmd_frame_t *p_flame)
{
    uint8_t *p_ptr;
    uint32_t i;

    if(s_is_init_fail != false) {
        return;
    }

    s_emc_confg.p_func_write(p_flame->cmd);
    p_ptr = p_flame->p_data;
    for(i = 0; i < p_flame->data_len; i++)
    {
        s_emc_confg.p_func_write(*p_ptr);
        s_emc_confg.p_func_delay(1);
    }
}

void emc_cmd_read(emc_cmd_frame_t *p_flame)
{
    uint8_t *p_ptr;
    uint32_t i, rx_data_len;

    if((s_is_init_fail != false) || (s_is_init_fail == 0)) {
        return;
    }

    s_emc_confg.p_func_read(p_ptr, &rx_data_len);
    if((p_ptr != NULL) || (rx_data_len > 1)) {
        p_flame->cmd = *p_ptr;
        p_ptr++;
        p_flame->p_data = p_ptr;
        p_flame->data_len = (rx_data_len - 1);
    }
}