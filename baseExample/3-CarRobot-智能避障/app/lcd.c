#include <stdio.h>
#include <contiki.h>
#include <dev/leds.h>
#include "ili93xx.h"
#include "adc.h"
#include "usart.h"
#include "string.h"
#include "rtc.h"
#include "sample.h"
#include "motor.h"
#include "Ultrasonic.h"
#include "user.h"


//lcd��ɫ����
#define LCD_COLOR_BLACK 0x0000
#define LCD_COLOR_WHITE 0xFFFF
#define LCD_COLOR_GRAY  0x8430
#define LCD_COLOR_RED   0xF800
#define LCD_COLOR_GREEN 0x07E0
#define LCD_COLOR_BLUE  0x001F

void fsmc_init(void);

void drawing_window(uint16 x,uint16 y,uint16 brush_color,uint16 backdrop_color)
{ 
    LCD_Clear(x, y, x+64, y+20,brush_color);
    LCD_Clear(x+64+5, y, x+140, y+20,brush_color);
    LCD_Clear(x+64+5+1, y+1, x+140-1, y+20-1,backdrop_color);
}


void lcd_page1(int page)
{
    uint16 temp_x=0,temp_y=0;
    uint16 brush_color=0xffff;          //������ɫ
    uint16 backdrop_color=0x001F;       //������ɫ
    
    if(page==1) 
    {
        LCDClear(0x0000); 
        
        LCD_Clear(0, 0, 319, 30, backdrop_color);
        LCDDrawFnt24(160-(strlen("���ͻ�����")*12/2), 3, "���ͻ�����", brush_color, backdrop_color);
        
        temp_x = 10;
        temp_y = 60;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("�¶�")*8/2)+temp_x, temp_y+2, "�¶�", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("���ն�")*8/2)+temp_x, temp_y+2, "���ն�", brush_color, backdrop_color);
        
        temp_x = 170;
        temp_y = 60;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("ʪ��")*8/2)+temp_x, temp_y+2, "ʪ��", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("ʱ��")*8/2)+temp_x, temp_y+2, "ʱ��", brush_color, backdrop_color);
        
        LCD_Clear(10, 160, 310, 180,0X8430);
        LCD_Clear(10, 190, 310, 210,0X8430);
        LCD_Clear(0, 240-20, 319, 240,0x001F);
    }
}




void lcd_page2()
{
    char s[255];
    uint16 temp_x=0,temp_y=0;
    
    //�¶�
    temp_x = 70+1+10;
    temp_y = 60+3;
    sprintf(s, "%04.1f", A1);               
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //���ն�
    temp_y+=40;
    sprintf(s, "%06.1f",A0);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //ʪ��
    temp_x = 70+1+170;
    temp_y = 60+3;
    sprintf(s, "%04.1f", A2);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //ʱ��
    temp_y+=40;
    sprintf(s, "%02d:%02d:%02d", RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //��ʾ��Ϣ
    if(motor_run)
    {
        sprintf(s,"������");
        lcdShowFont16(315-(strlen(s)*8),15-8,s,LCD_COLOR_WHITE,LCD_COLOR_BLUE);
    }
    else
    {
        sprintf(s,"��ֹͣ");
        lcdShowFont16(315-(strlen(s)*8),15-8,s,LCD_COLOR_WHITE,LCD_COLOR_RED);
    }
    
    sprintf(s,"�������������룺%03.1fcm",ultrasonicValue);
    lcdShowFont16(160-(strlen(s)*8/2), 160+2, s, LCD_COLOR_RED, LCD_COLOR_GRAY);
    
    sprintf(s,"����ٶȣ�%04d(K3+ K4-)",motor_speed);
    lcdShowFont16(160-(strlen(s)*8/2), 190+2, s, LCD_COLOR_RED, LCD_COLOR_GRAY);
    
}



PROCESS(lcd, "lcd");

PROCESS_THREAD(lcd, ev, data)
{
    static struct etimer lcd_timer;
    
    PROCESS_BEGIN();
    
    fsmc_init();                                                //fsmc��ʼ��
    BLInit();                                                   //�����ʼ��
    ILI93xxInit();                                              //LCD��ʼ��
    lcd_page1(1);                                               //��ʾpage2    
    etimer_set(&lcd_timer,500);                                 //����lcd_timer��ʱ��                 
    
    while(1)
    {                       
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);   //�ȴ�lcd_timer��ʱ�����   
        etimer_reset(&lcd_timer);                              //����lcd_timer��ʱ��  
        
        lcd_page2();  
    }
    PROCESS_END();
}