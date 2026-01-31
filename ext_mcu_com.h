/**
 * @file ext_mcu_com.h
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief 外部マイコン通信
 * @version 0.1
 * @date 2026-01-31
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */
#ifndef EXT_MCU_COM_H
#define EXT_MCU_COM_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define EMC_MODE_MASTER     0x00
#define EMC_MODE_SLAVE      0x01
#define EMC_MODE_NONE       0xFF

typedef struct {
    uint8_t cmd;
    uint8_t *p_data;
    uint32_t data_len;
} emc_cmd_frame_t;

typedef void(*p_func_drv_spi_write)(uint8_t tx_data);
typedef void(*p_func_drv_spi_read)(uint8_t *p_rx_data, uint32_t *p_rx_data_len);
typedef void(*p_func_delay_ms)(uint32_t ms);

typedef struct {
    uint8_t mode;
    p_func_drv_spi_write p_func_write; // 呼び元のSPIドライバ Write関数ポインタ
    p_func_drv_spi_read p_func_read;   // 呼び元のSPIドライバ Read関数ポインタ
    p_func_delay_ms p_func_delay;      // 呼び元のmsec遅延するdelay関数ポインタ
} emc_config_t;

bool emc_init(emc_config_t *p_config);
void emc_cmd_write(emc_cmd_frame_t *p_flame);
void emc_cmd_read(emc_cmd_frame_t *p_flame);

#endif // EXT_MCU_COM_H