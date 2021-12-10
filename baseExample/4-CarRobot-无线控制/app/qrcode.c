#include <contiki.h>
#include <contiki-net.h>
#include "adc_key.h"
#include <stdio.h>
#include "ili93xx.h"
#include "qr_encode.h"
#include "gsm.h"
#include "user.h"

#define QRCODE_Y 	0		//TFT��ά����ʾ����y

//��ά����ʾ��־��0-->δ��ʾ��1-->��ʾ��
char qrcodeShow_flag = 0;

extern void lcd_page1(int page);
extern void lcd_page2();

//0 0 5 5
//6 6 11 11
u8 codetest[]={//΢����Ƭ������ȥ�ٶ���������ά����Ƭ �鿴
    //"BEGIN:VCARD\r\n"
    //"VERSION:3.0\r\n"
    //"N:������\r\n"//����
    //"TEL:13888888888\r\n"//�ֻ���
    //"TEL;TYPE=WORK,VOICE:0571-88888888\r\n"//��������
    //"TEL;TYPE=HOME,VOICE:0571-66666666\r\n"//��ͥ����
    //"NOTE:QQ:12345\r\n"//QQ����
    //"EMAIL:12345@qq.com\r\n"//�����ַ
    //"ORG:��˾����\r\n"//��˾
    //"TITLE:ְλ����\r\n"
    //"URL:http://������ҳ\r\n"
    //"ADR;TYPE=WORK:������ַ\r\n"//��ַ
    //"ADR;TYPE=HOME:��ͥסַ\r\n"
    //"END:VCARD"
    "0123456789"
};

void DISPLAY_RENCODE_TO_TFT(u8 *qrcode_data)
{
    u8 i,j;
    u16 x,y,p;
    char s[64];
    EncodeData((char *)qrcode_data);
    LCDClear(0);
    if(m_nSymbleSize*2>240)	
    {
        return;
    }
    if(m_nSymbleSize < 1)	
    {
        sprintf(s, "MAC��ַ��ȡʧ��");
        LCDDrawFnt16((320-7*16-8)/2, (240-16)/2, s, 0x0000, 0xffff);
    }
    for(i=0;i<10;i++)
    {
        if((m_nSymbleSize*i*2)>240)	break;
    }
    p=i;//���С
    x=(320-m_nSymbleSize*p)/2;
    y=(240-m_nSymbleSize*p)/2;
    LCD_Clear(x-2,y-2,x+m_nSymbleSize*p+2,y+m_nSymbleSize*p+2,0xffff); 
    for(i=0;i<m_nSymbleSize;i++)
    {
        for(j=0;j<m_nSymbleSize;j++)
        {
            if(m_byModuleData[i][j]==1)
                LCD_Clear(x+p*i,y+p*j,x+p*(i+1)-1,y+p*(j+1),0); 
        }
        
    }
}



PROCESS(qrcode, "qrcode");


PROCESS_THREAD(qrcode, ev, data)
{
    PROCESS_BEGIN();
    while (1) 
    {
        PROCESS_WAIT_EVENT();                                   //�ȴ��¼�����
        if(ev == process_event_key)                             //�ж��Ƿ��������¼�
        {                                
            if((*(int*)data)==KEY1_LONG_PRESSED)                //�ж��¼������ֵ
            {                     
                qrcodeShow_flag = 1;
                if(connect_mode==1)
                    DISPLAY_RENCODE_TO_TFT((unsigned char *)gsm_info.imei);
                else
                    DISPLAY_RENCODE_TO_TFT((unsigned char *)RADIO_MAC);
            }
            if((*(int*)data)==KEY2_PRESSED)                     //�ж��¼������ֵ
            {                          
                qrcodeShow_flag = 0;
                lcd_page1(1);
                lcd_page2();
            }
        } 
    }
    PROCESS_END();
}