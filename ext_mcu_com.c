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
#ifdef EMC_MODE_MASTER
#endif // vEMC_MODE_MASTER

#ifdef EMC_MODE_SLAVE
typedef void(*p_func_cmd_cbk)(uint8_t rw, uint8_t addr, uint8_t *p_data);
static void emc_cmd_config_cbk(uint8_t rw, uint8_t addr, uint8_t *p_data);
static void emc_cmd_reg_cbk(uint8_t rw, uint8_t addr, uint8_t *p_data);
typedef struct {
    uint8_t cmd;
    p_func_cmd_cbk p_cbk;
} emc_cmd_tbl_data_t;
const emc_cmd_tbl_data_t g_emc_cmd_tbl[] = {
    {0x10, emc_cmd_config_cbk},
    {0x30, emc_cmd_reg_cbk}
};
#define EMC_CMD_TBL_CNT     0x02

typedef struct {
    const uint8_t addr;
    uint8_t data;
} emc_reg_t;

static emc_reg_t s_emc_reg_tbl[] = {
    {0xF8, 0xAA}, // Who Am Iレジスタ
};
#define EMC_REG_CNT     1
#endif // EMC_MODE_SLAVE

static emc_config_t s_emc_confg;
static bool s_is_init_fail = false;
// --------------------------------------------------------------------------
// [Static関数]

#ifdef EMC_MODE_MASTER
static void emc_master_proc(emc_cmd_frame_t *p_flame)
{
    uint8_t *p_ptr;

    p_ptr = p_flame->p_tx_data;
    s_emc_confg.p_func_cs(SPI_CS_LOW); // SPI CS = Low
    s_emc_confg.p_func_write(p_flame->cmd.byte);
    s_emc_confg.p_func_write(p_ptr); // Reg Addr
    if(p_flame->cmd.bit.rw == EMC_WRITE_CMD) {
        p_ptr++;
        s_emc_confg.p_func_write(p_ptr); // Reg Data
    } else {
        // NOTE: SPIでReadするためにダミーデータを送ってクロック供給
        s_emc_confg.p_func_write(0xFF);
        p_ptr = p_flame->p_rx_data;
        *p_ptr = s_emc_confg.p_func_read();
    }
    s_emc_confg.p_func_cs(SPI_CS_HIGH); // SPI CS = High
}
#endif // EMC_MODE_MASTER

#ifdef EMC_MODE_SLAVE
static void emc_cmd_config_cbk(uint8_t rw, uint8_t addr, uint8_t *p_data)
{
    // TOOD
}

static void emc_cmd_reg_cbk(uint8_t rw, uint8_t addr, uint8_t *p_data)
{
    uint8_t i;

    if(p_data == NULL) {
        return;
    }

    for(i = 0; i < EMC_REG_CNT; i++)
    {
        if(s_emc_reg_tbl[i].addr == addr) {
            if(rw == EMC_READ_CMD) {
                *p_data = s_emc_reg_tbl[i].data;
            } else {
                s_emc_reg_tbl[i].data = *p_data;
            }
            break;
        }
    }
}

static void emc_slave_proc(emc_cmd_frame_t *p_flame)
{
    uint8_t i;
    uint8_t *p_ptr;
    uint8_t rw, addr, data;

    p_flame->cmd.byte = s_emc_confg.p_func_read(); // RX: CMD
    rw = p_flame->cmd.bit.rw;
    p_ptr = p_flame->p_rx_data;
    *p_ptr = s_emc_confg.p_func_read(); // RX: Addr
    addr = *p_ptr;
    if(rw == EMC_WRITE_CMD) {
        p_ptr++;
        *p_ptr = s_emc_confg.p_func_read();  // RX: Addr Data
        data = *p_ptr;
    }

    // コマンド解析処理
    for(i = 0; i < EMC_CMD_TBL_CNT; i++)
    {
        // コマンド IDに一致するコールバック関数実行
        if(p_flame->cmd.bit.cmd == g_emc_cmd_tbl[i].cmd) {
            g_emc_cmd_tbl[i].p_cbk(rw, addr, &data);
            break;
        }
    }
}
#endif // EMC_MODE_SLAVE
// --------------------------------------------------------------------------
// [API関数]

bool emc_init(emc_config_t *p_config)
{
    if ((p_config->p_func_read == NULL)   ||
        (p_config->p_func_write == NULL)  ||
#ifdef EMC_MODE_MASTER
        (p_config->p_func_cs == NULL)  ||
#endif // EMC_MODE_MASTER
        (p_config->p_func_delay == NULL)
    ) {
        s_is_init_fail = true;
        return false;
    }

#ifdef EMC_MODE_MASTER
    s_emc_confg.p_func_cs    = p_config->p_func_cs;
#endif // EMC_MODE_MASTER
    s_emc_confg.p_func_write = p_config->p_func_write;
    s_emc_confg.p_func_read  = p_config->p_func_read;
    s_emc_confg.p_func_delay = p_config->p_func_delay;

    return true;
}

void emc_cmd_proc(emc_cmd_frame_t *p_flame)
{
    uint8_t *p_ptr;

    if(s_is_init_fail != false) {
        return;
    }

#ifdef EMC_MODE_MASTER
    emc_master_proc(p_flame);
#else
    emc_slave_proc(p_flame);
#endif
}