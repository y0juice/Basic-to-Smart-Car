/*
 * lcd.h
 *
 *  Created on: Oct 23, 2025
 *      Author: 58
 */

#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include <stdint.h>




// LCD dimensions
#define LCD_WIDTH  160
#define LCD_HEIGHT 120

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0


// LCD functions
void LCD_Init(void);
void LCD_Fill(uint16_t color);
void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bg_color);
void LCD_DrawString(uint8_t x, uint8_t y, const char* str, uint16_t color, uint16_t bg_color);
//void LCD_Delete_Area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t bg_color));
//void LCD_DeleteArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

#endif
