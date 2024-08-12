/*
 * @Author: SongZihui-sudo 1751122876@qq.com
 * @Date: 2024-08-05 15:59:49
 * @LastEditors: SongZihui-sudo 1751122876@qq.com
 * @LastEditTime: 2024-08-05 21:46:04
 * @FilePath: /luckfox-oled/src/0_in_96_oled.h
 * @Description: 0.96 OLED DRIVER
 */
#ifndef OLED_0_IN_96_H
#define OLED_0_IN_96_H

#include <stdint.h>
#include <stdlib.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define IIC_CMD        0X00
#define IIC_RAM        0X40

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

UBYTE DEV_ModuleInit(void);
void  DEV_ModuleExit(void);
void DEV_Delay_ms(UDOUBLE xms);
void I2C_Write_Byte(uint8_t value, uint8_t Cmd);

int OLED_Init();

void OLED_Clear(void);

void OLED_address(uint8_t x, uint8_t y);

void OLED_FULL_ON();

void OLED_FULL_OFF();

void OLED_Display_128x64(uint8_t *dp);

void OLED_Display_64x64(uint8_t x, uint8_t y, uint8_t *dp);

void OLED_Display_16x16(uint8_t x, uint8_t y, uint8_t *dp);

void OLED_Display_8x16(uint8_t x, uint8_t y, uint8_t *dp);

void OLED_Display_16x8(uint8_t x, uint8_t y, uint8_t *dp);

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size);

void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size);

void OLED_ClearGivenChar( uint8_t Line, uint8_t Column );

#endif