/*
 * @Author: SongZihui-sudo 1751122876@qq.com
 * @Date: 2024-08-05 15:59:56
 * @LastEditors: SongZihui-sudo 1751122876@qq.com
 * @LastEditTime: 2024-08-05 22:05:53
 * @FilePath: /luckfox-oled/src/oled_0_in_96.c
 * @Description: 0.96 OLED DRIVER
 */
#include "oled_0_in_96.h"
#include "./Fonts/fonts.h"
#include "i2c/dev_hardware_i2c.h"
#include <stdio.h>

#include "i2c/dev_hardware_i2c.h"

UBYTE DEV_ModuleInit(void) {
  printf("USE_IIC\r\n");
  DEV_HARDWARE_I2C_begin("/dev/i2c-3");
  DEV_HARDWARE_I2C_setSlaveAddress(0x3c);
}

void DEV_ModuleExit(void) { DEV_HARDWARE_I2C_end(); }

void DEV_Delay_ms(UDOUBLE xms) {
  UDOUBLE i;
  for (i = 0; i < xms; i++) {
    usleep(1000);
  }
}

void I2C_Write_Byte(uint8_t value, uint8_t Cmd) {
  char wbuf[2] = {Cmd, value};
  DEV_HARDWARE_I2C_write(wbuf, 2);
}

int OLED_Init() {
  I2C_Write_Byte(0xAE, IIC_CMD); //--display off

  I2C_Write_Byte(0x00, IIC_CMD); //--set low column address
  I2C_Write_Byte(0x10, IIC_CMD); //--set high column address
  I2C_Write_Byte(0x40, IIC_CMD); //--set start line address

  I2C_Write_Byte(0xB0, IIC_CMD); //--set page address

  I2C_Write_Byte(0x81, IIC_CMD); // contract control
  I2C_Write_Byte(0xFF, IIC_CMD); //--128
  I2C_Write_Byte(0xA1, IIC_CMD); // set segment re-map 0 to 127
  I2C_Write_Byte(0xA6, IIC_CMD); // set normal display
  I2C_Write_Byte(0xA8, IIC_CMD); // set multiplex ratio(1 to 64)
  I2C_Write_Byte(0x3F, IIC_CMD); //--1/32 duty

  I2C_Write_Byte(0xC8, IIC_CMD); // Com scan direction
  I2C_Write_Byte(0xD3, IIC_CMD); // set display offset
  I2C_Write_Byte(0x00, IIC_CMD); // no offset

  I2C_Write_Byte(
      0xD5, IIC_CMD); // set display clock divide ratio/oscillator frequency
  I2C_Write_Byte(0x80, IIC_CMD); //

  I2C_Write_Byte(0xD8, IIC_CMD); // set area color mode off
  I2C_Write_Byte(0x05, IIC_CMD); //

  I2C_Write_Byte(0xD9, IIC_CMD); // Set Pre-Charge Period
  I2C_Write_Byte(0xF1, IIC_CMD); //

  I2C_Write_Byte(0xDA, IIC_CMD); // set com pin hardware configuartion
  I2C_Write_Byte(0x12, IIC_CMD); //

  I2C_Write_Byte(0xDB, IIC_CMD); // set Vcomh
  I2C_Write_Byte(0x30, IIC_CMD); // 0x20,0.77xVcc

  I2C_Write_Byte(0x8D, IIC_CMD); // set charge pump enable
  I2C_Write_Byte(0x14, IIC_CMD); //

  I2C_Write_Byte(0xAF, IIC_CMD); //--turn on oled panel

  return 0;
}

void OLED_Clear(void) {
  uint8_t i, n;
  for (i = 0; i < 8; i++) {
    I2C_Write_Byte(0xb0 + i, IIC_CMD); //设置页地址
    I2C_Write_Byte(0x00, IIC_CMD);     //设置显示位置—列低地址
    I2C_Write_Byte(0x10, IIC_CMD);     //设置显示位置—列高地址
    //	I2C_Write_Byte(0x10, IIC_CMD);	  //列地址高4位
    //	I2C_Write_Byte(0x00, IIC_CMD);	  //列地址低4位
    for (n = 0; n < 128; n++) {
      I2C_Write_Byte(0x00, IIC_RAM); //清除所有数据
    }
  }
}

//设置起始地址
void OLED_address(uint8_t x, uint8_t y) {
  I2C_Write_Byte(0xb0 + y, IIC_CMD);                 //设置页地址
  I2C_Write_Byte(((x & 0xf0) >> 4) | 0x10, IIC_CMD); //列地址高4位
  I2C_Write_Byte((x & 0x0f), IIC_CMD);               //列地址低4位
}

void OLED_FULL_ON() {
  uint8_t i, j;
  for (i = 0; i < 8; i++) {
    OLED_address(0, i);
    for (j = 0; j < 128; j++) {
      I2C_Write_Byte(0xFF, IIC_RAM);
    }
  }
}
void OLED_FULL_OFF() {
  uint8_t i, j;
  for (i = 0; i < 8; i++) {
    OLED_address(0, i);
    for (j = 0; j < 128; j++) {
      I2C_Write_Byte(0x00, IIC_RAM);
    }
  }
}
//显示64*128的图像
void OLED_Display_128x64(uint8_t *dp) {
  uint8_t i, j;
  for (i = 0; i < 8; i++) {
    OLED_address(0, i);
    for (j = 0; j < 128; j++) {
      I2C_Write_Byte(*dp, IIC_RAM);
      dp++;
    }
  }
}
void OLED_Display_64x64(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i, j;
  for (j = 0; j < 8; j++) {
    OLED_address(x, y);
    for (i = 0; i < 64; i++) {
      I2C_Write_Byte(*dp, IIC_RAM);
      dp++;
    }
    y++;
  }
}

//显示16*16的图像
void OLED_Display_16x16(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i, j;
  for (j = 0; j < 2; j++) {
    OLED_address(x, y);
    for (i = 0; i < 16; i++) {
      I2C_Write_Byte(*dp, IIC_RAM);
      dp++;
    }
    y++;
  }
}

//显示8*16的图像 8宽，16 高
void OLED_Display_8x16(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i, j;
  for (j = 0; j < 2; j++) {
    OLED_address(x, y);
    for (i = 0; i < 8; i++) {
      I2C_Write_Byte(*dp, IIC_RAM);
      dp++;
    }
    y++;
  }
}

void OLED_Display_16x8(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i, j;
  OLED_address(x, y);
  for (i = 0; i < 16; i++) {
    I2C_Write_Byte(*dp, IIC_RAM);
    dp++;
  }
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 8,16,32,48
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size) {
  unsigned char c = 0, i = 0;
  c = chr - ' '; //得到偏移后的值
  if (x > OLED_WIDTH - 1) {
    x = 0;
    y = y + 2;
  }
  if (size == 16) {
    OLED_address(x, y);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font16.table[c * 16 + i], IIC_RAM);
    OLED_address(x, y + 1);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font16.table[c * 16 + i + 8], IIC_RAM);
  } else if (size == 12) {
     OLED_address(x, y);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font12.table[c * 16 + i], IIC_RAM);
    OLED_address(x, y + 1);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font12.table[c * 16 + i + 8], IIC_RAM);
  } else if (size == 20) {
    OLED_address(x, y);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font20.table[c * 24 + i], IIC_RAM);
    OLED_address(x, y + 1);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font20.table[c * 24 + i + 8], IIC_RAM);
    OLED_address(x, y + 2);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font20.table[c * 24 + i + 16], IIC_RAM);
  }
  else if (size == 24) {
    OLED_address(x, y);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font24.table[c * 24 + i], IIC_RAM);
    OLED_address(x, y + 1);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font24.table[c * 24 + i + 8], IIC_RAM);
    OLED_address(x, y + 2);
    for (i = 0; i < 8; i++)
      I2C_Write_Byte(Font24.table[c * 24 + i + 16], IIC_RAM);
  }
}

void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size) {
  unsigned char j = 0;
  printf("show: %s", p);
  while (p[j] != '\0') {
    OLED_ShowChar(x, y, p[j], size);
    x += 8;
    if (x > 120) {
      x = 0;
      y += 2;
    }
    j++;
  }
}