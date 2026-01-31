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

#define SPI_CS_LOW          0x00
#define SPI_CS_HIGH         0x01

typedef void(*p_func_drv_spi_cs)(uint8_t hi_low);
typedef void(*p_func_drv_spi_write)(uint8_t tx_data);
typedef uint8_t(*p_func_drv_spi_read)(void);
typedef void(*p_func_delay_ms)(uint32_t ms);

typedef struct {
    uint8_t mode;
    p_func_drv_spi_cs p_func_cs;       // 呼び元のSPIドライバ CS関数ポインタ
    p_func_drv_spi_write p_func_write; // 呼び元のSPIドライバ Write関数ポインタ
    p_func_drv_spi_read p_func_read;   // 呼び元のSPIドライバ Read関数ポインタ
    p_func_delay_ms p_func_delay;      // 呼び元のmsec遅延するdelay関数ポインタ
} emc_config_t;

typedef struct {
    uint8_t cmd;
    uint8_t tx_data;
    uint8_t rx_data;
} emc_cmd_frame_t;

bool emc_init(emc_config_t *p_config);
void emc_cmd_proc(emc_cmd_frame_t *p_flame);

#endif // EXT_MCU_COM_H