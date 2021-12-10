#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include "delay.h"
#include "stm32f4xx.h"

#define SCREEN_ORIENTATION_LANDSCAPE 1
#define ZXBEE_PLUSE
#define SWAP_RGB (1<<3)

/* lcd ʹ��PD7�� NE1���ܻ���ַΪ0x60000000, PD13����A18 ��Ϊrsѡ��*/
#ifdef ZXBEE_PLUSE
#define ILI93xx_REG (*((volatile uint16_t *)(0x60000000)))
#define ILI93xx_DAT (*((volatile uint16_t *)(0x60000000 | (1<<(17+1)))))
#else
//NE4���ܻ���ַΪ0x6C000000, rs A6
#define ILI93xx_REG (*((volatile uint16_t *)(0x6C000000)))
#define ILI93xx_DAT (*((volatile uint16_t *)(0x6C000000 | (1<<(6+1)))))
#endif

void LCD_WR_REG(vu16 regval);
void LCD_WR_DATA(vu16 data);
u16 LCD_RD_DATA(void);
void ILI93xx_WriteReg(uint16_t r, uint16_t d);
uint16_t ILI93xx_ReadReg(uint16_t r);
void BLOnOff(int st);
void BLInit(void);
void ILI93xxInit(void);
void LCDSetWindow(int x, int xe, int y, int ye);
void LCDSetCursor(int x, int y);
void LCDWriteData(uint16_t *dat, int len);
void LCDClear(uint16_t color);
void LCDDrawPixel(int x, int y, uint16_t color);
void LCDrawLineH(int x0, int x1, int y0, int color);
void LCDDrawAsciiDot12x24_1(int x, int y, char ch, int color, int bc);
void LCDDrawAsciiDot12x24(int x, int y, char *str, int color, int bc);
void LCDDrawAsciiDot8x16_1(int x, int y, char ch, int color, int bc);
void LCDDrawAsciiDot8x16(int x, int y, char *str, int color, int bc);
void LCDDrawGB_16_1(int x, int y, char *gb2, int color, int bc) ;
void LCDDrawGB_24_1(int x, int y, char *gb2, int color, int bc) ;
void LCDDrawFnt16(int x, int y, char *str, int color, int bc);
void LCDDrawFnt24(int x, int y, char *str, int color, int bc);
void LCD_Clear(int x1,int y1,int x2,int y2,uint16_t color);
void LCD_ShowPic(int x,int y,int length,int height,unsigned char *pic);

/*********************************************************************************************
* ���ƣ�LCD_PutChar16
* ���ܣ�16*16�����ַ���ʾ���ļ��ֿ�棩
* ������x��y����ʾ���� *data���ַ�����Color��������ɫ BColor��������ɫ
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void LCD_PutChar16(u16 x,u16 y,u8 *data,u16 Color,u16 BColor);

/*********************************************************************************************
* ���ƣ�lcdShowFont16
* ���ܣ�lcd��ʾ16x16����
* ������x1,y1:��ʼ����ֵ��color��������ɫ��Bcolor��������ɫ
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void lcdShowFont16(uint16 x,uint16 y,char* p,uint16 color,uint16 Bcolor);

#endif
