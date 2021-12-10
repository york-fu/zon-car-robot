#include <stdio.h>
#include <contiki.h>
#include <dev/leds.h>
#include "ili93xx.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "rtc.h"
#include "sample.h"
#include "user.h"




void fsmc_init(void);

extern char qrcode_flag;
extern u8 motion_mode;
extern u8 motor_direction;

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
        temp_y = 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("�¶�")*8/2)+temp_x, temp_y+2, "�¶�", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("����")*8/2)+temp_x, temp_y+2, "����", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("���ն�")*8/2)+temp_x, temp_y+2, "���ն�", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("����ģʽ")*8/2)+temp_x, temp_y+2, "����ģʽ", brush_color, backdrop_color);
        
        temp_x = 170;
        temp_y = 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("ʪ��")*8/2)+temp_x, temp_y+2, "ʪ��", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("γ��")*8/2)+temp_x, temp_y+2, "γ��", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("ʱ��")*8/2)+temp_x, temp_y+2, "ʱ��", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("����ģʽ")*8/2)+temp_x, temp_y+2, "����ģʽ", brush_color, backdrop_color);
        
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
    temp_y = 40+3;
    sprintf(s, "%.1f", A1);               
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //����
    temp_y+=40;
    sprintf(s, "%.4f",A3);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //���ն�
    temp_y+=40;
    sprintf(s, "%.1f",A0);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //����ģʽ
    temp_y+=40;
    if(connect_mode==0)
        sprintf(s, "BLE");
    else
        sprintf(s, "GSM");
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //ʪ��
    temp_x = 70+1+170;
    temp_y = 40+3;
    sprintf(s, "%.1f", A2);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //γ��
    temp_y+=40;
    sprintf(s, "%.4f",A4);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //ʱ��
    temp_y+=40;
    sprintf(s, "%02d:%02d:%02d", RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    
    //����ģʽ
    temp_y+=40;
    if(motion_mode==1)
    {
        sprintf(s, "�ֶ�");        
    }
    else if(motion_mode==2)
    {    
        sprintf(s, "Ѱ��");
    }
    else if(motion_mode==3)
    {
        sprintf(s, "����");
    }
    else
    {
        sprintf(s, "ֹͣ");
    }
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    if (strlen(RADIO_MAC) > 0 ) 
    {
        sprintf(s, "MAC: %s", RADIO_MAC);
        LCDDrawFnt16(65, 222, s, 0x0000, 0x001F);
    }
    
    LCD_Clear(10, 190, 310, 210,0X8430);
    for(uint8 i=0;i<255;i++) s[i]=0;
    //��ʾ��Ϣ
    if(V7!=0)
    {
        if(V7==1)
        {           
            sprintf(s, "��е���Լ�");
        }
        else if(V7==2)
        {
            sprintf(s, "��ˮ");
        }
        else if(V7==3)
        {
            sprintf(s, "������");
        }
        else if(V7==4)
        {
            sprintf(s, "�Ӷ���");
        }
        else
        {
            sprintf(s, "�޶���");
        }
    }
    else if(motion_mode==1)
    {
        if(motor_direction==1)
        {       
            sprintf(s, "ǰ��");   
        }
        else if(motor_direction==2)
        {
            sprintf(s, "����");
        }
        else if(motor_direction==3)
        {
            sprintf(s, "��ת");
        }
        else if(motor_direction==4)
        {
            sprintf(s, "��ת");
        }
        else
        {  
            sprintf(s, "ֹͣ");
        }
    }
    LCDDrawFnt16(160-(strlen(s)*8/2), 190+2, s, 0x0000, 0X8430);
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
        
        if(qrcode_flag == 0)
        {
            lcd_page2();  
        }
    }
    PROCESS_END();
}