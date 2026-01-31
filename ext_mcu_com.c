/**
 * @file ext_mcu_com.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief 外部マイコン通信
 * @version 0.1
 * @date 2026-01-31
 * 
 * @copyright Copyright (c) 2026 Chimipupu All Rights Reserved.
 * 
 */

#include "ext_mcu_com.h"

// --------------------------------------------------------------------------
// [Private]
#ifdef EMC_MODE_SLAVE
typedef void(*p_func_cmd_cbk)(uint8_t *p_data, uint32_t data_len);
void emc_cmd_config_cbk(uint8_t *p_data, uint32_t data_len);
typedef struct {
    uint8_t cmd;
    p_func_cmd_cbk p_cbk;
} emc_cmd_tbl_data_t;

const emc_cmd_tbl_data_t g_emc_cmd_tbl[] = {
    {0x10, emc_cmd_config_cbk}
};
#endif

static emc_config_t s_emc_confg;
static bool s_is_init_fail = false;
// --------------------------------------------------------------------------
// [Static関数]

#ifdef EMC_MODE_SLAVE
void emc_cmd_config_cbk(uint8_t *p_data, uint32_t data_len)
{
    // TOOD
}
#endif
// --------------------------------------------------------------------------
// [API関数]

bool emc_init(emc_config_t *p_config)
{
    if ((p_config->p_func_read == NULL)   ||
#ifdef EMC_MODE_MASTER
        (p_config->p_func_write == NULL)  ||
#endif
        (p_config->p_func_delay == NULL)
    ) {
        s_is_init_fail = true;
        return false;
    }

#ifdef EMC_MODE_MASTER
    if (p_config->p_func_cs != NULL){
        s_emc_confg.p_func_cs    = p_config->p_func_cs;
    } else {
        s_is_init_fail = true;
        return false;
    }

    s_emc_confg.p_func_write = p_config->p_func_write;
#endif

    s_emc_confg.p_func_read  = p_config->p_func_read;
    s_emc_confg.p_func_delay = p_config->p_func_delay;

    return true;
}

void emc_cmd_proc(emc_cmd_frame_t *p_flame)
{
    uint8_t *p_ptr;
    uint32_t i;

    if(s_is_init_fail != false) {
        return;
    }

#ifdef EMC_MODE_MASTER
    s_emc_confg.p_func_cs(SPI_CS_LOW); // SPI CS = Low
    s_emc_confg.p_func_write(p_flame->cmd);
    s_emc_confg.p_func_write(p_flame->tx_data);
    p_flame->rx_data = s_emc_confg.p_func_read();
    s_emc_confg.p_func_cs(SPI_CS_HIGH); // SPI CS = High
#else
    p_flame->cmd = s_emc_confg.p_func_read();
    p_flame->rx_data = s_emc_confg.p_func_read();
    // TOOD: コマンド解析処理の実装
#endif
}