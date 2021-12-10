/*********************************************************************************************
* �ļ���sample.c
* ���ߣ�
˵����
*   ���������Ʋ������ã�  A0��ʾ�������ն�����;
*                        A1��ʾ�����¶�����;
*                        A2��ʾ����ʪ������;
*                        V0��ʾ�����ϱ�ʱ����;
*                        D0(Bit0/Bit1)��ʾ�ϱ�״̬��Ĭ������D0=3;
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "sample.h"
#include "zxbee.h"
#include "pcf8563.h"
#include "w25qxx.h"
#include "bh1750.h"
#include "htu21d.h"
#include "led.h"
#include "infrared.h"
#include "rtc.h"
#include "config.h"
#include "delay.h"
#include "GPS.h"
#include "motor.h"
#include "user.h"
#include "ble.h"

extern int package_data(char *buf);

/*�¶ȴ���������*/
extern float htu21dValue_t;
extern float htu21dValue_h;

/*********************************************************************************************
���������Ʋ���
*********************************************************************************************/
u8 D0 = 0x1f;                                                   //�����ϱ����أ�Ĭ��ȫ��
u16 V0 = 5;                                                     //�����ϱ�ʱ��������λ��,0�������ϱ�
float A0, A1, A2, A3, A4;                                       //���նȡ��¶ȡ�ʪ�ȡ����ȡ�γ��
char lat[16], lng[16];
RTC_TimeTypeDef RTC_TimeStruct;                                 //ʱ��
RTC_DateTypeDef RTC_DateStruct;                                 //����


/*********************************************************************************************
* ���ƣ�sensor_init()
* ���ܣ�
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensor_init(void)
{
    bh1750_init();                                              //���նȴ�����
    gps_init();                                                 //gps��ʼ��
    My_RTC_Init();                                              //��ʼ��RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	        //����WAKE UP�ж�,1���ж�һ��
} 


/*********************************************************************************************
* ���ƣ�sensor_poll()
* ���ܣ���ѯ���������������ϱ�����������
* ��������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sensor_poll(void)
{
    char buf[64];
    static int t = 0;
    
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);  
    gps_get(lat,lng);
    
    A0 = bh1750_get();                                            //��ȡ���ն�ֵ
    A1 = htu21dValue_t;                                           //��ȡ�¶�
    A2 = htu21dValue_h;                                           //��ȡʪ��
    A3 = atoi(lat)/100+(atoi(lat)%100)/60.0+((long)(atof(lat)*100)%100)/3600.0;
    A4 = atoi(lng)/100+(atoi(lng)%100)/60.0+((long)(atof(lng)*100)%100)/3600.0;
    
    if (V0 != 0) 
    {
        if(t >= (V0*10)) 
        {
            t = 0;
            if(connect_mode==1)                                          //GSMģʽ
            {
                zxbeeBegin();
                sprintf(buf, "%.1f", A0);
                zxbeeAdd("A0", buf);
                sprintf(buf, "%.1f", A1);
                zxbeeAdd("A1", buf);
                sprintf(buf, "%.1f", A2);
                zxbeeAdd("A2", buf);
                sprintf(buf, "%.4f", A3);
                zxbeeAdd("A3", buf);
                sprintf(buf, "%.4f", A4);
                zxbeeAdd("A4", buf);
                char *p = zxbeeEnd();
                if (p != NULL) 
                {
                    package_data(p);
                }
            }
            else                                                //����ģʽ
            {
                zxbeeBegin();
                sprintf(buf, "%.1f", A0);
                zxbeeAdd("A0", buf);
                sprintf(buf, "%.1f", A1);
                zxbeeAdd("A1", buf);
                sprintf(buf, "%.1f", A2);
                zxbeeAdd("A2", buf);
                sprintf(buf, "%.4f", A3);
                zxbeeAdd("A3", buf);
                sprintf(buf, "%.4f", A4);
                zxbeeAdd("A4", buf);
                char *p = zxbeeEnd();
                if (p != NULL) 
                {
                    ble_write(p);                               //ͨ��������������������ݷ��Ϳ������
                }
            }
        }
        else 
            t++;
    }
}




/*********************************************************************************************
* ���ƣ�sample_z_process_command_call()
* ���ܣ������ϲ�Ӧ�÷�������ָ��
* ������ptag: ָ���ʶ D0��D1�� A0 ...
*       pval: ָ��ֵ�� ��������ʾ��ȡ��
*       obuf: ָ�������ŵ�ַ
* ���أ�>0ָ������������ݳ��ȣ�0��û�з������ݣ�<0����֧��ָ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int sample_z_process_command_call(char* ptag, char* pval)
{
    char buf[16]; 
    int ret = -1;
    
    if (memcmp(ptag, "MAC", 3) == 0) {                          //��ѯMAC
        strcpy(RADIO_MAC, pval);
    }
    else if (memcmp(ptag, "D0", 2) == 0) {                      //��ѯD0
        if (pval[0] == '?') {
            sprintf(buf, "%d", D0);
            zxbeeAdd("D0", buf);
        }
    }
    else if (memcmp(ptag, "CD0", 3) == 0) {                     //�ر�D0(�����ϱ�ʹ��)
        int value = atoi(pval);
        if (value > 0) {
            D0 &= ~value;
        }
    }
    else if (memcmp(ptag, "OD0", 3) == 0) {                     //��D0(�����ϱ�ʹ��)    
        int value = atoi(pval);
        if (value > 0) {
            D0 |= value;
        }
    }
    else if (memcmp(ptag, "A0", 2) == 0) {                      //��ѯA0��ֵ
        if (pval[0] == '?') {
            sprintf(buf, "%f", A0);
            zxbeeAdd("A0", buf);
        }
    }
    
    else if (memcmp(ptag, "A1", 2) == 0) {                             //��ѯA1��ֵ
        if (pval[0] == '?') {
            sprintf(buf, "%f", A1);
            zxbeeAdd("A1", buf);
        }
    }
    else if (memcmp(ptag, "A2", 2) == 0) {                             //��ѯA2��ֵ
        if (pval[0] == '?') {
            sprintf(buf, "%.1f", A2);
            zxbeeAdd("A2", buf);
        }
    }
    else if (memcmp(ptag, "A3", 2) == 0) {                             //��ѯA3��ֵ
        if (pval[0] == '?') {
            sprintf(buf, "%.1f", A3);
            zxbeeAdd("A3", buf);
        }
    }
    else if (memcmp(ptag, "A4", 2) == 0) {                             //��ѯA4��ֵ
        if (pval[0] == '?') {
            sprintf(buf, "%.1f", A4);
            zxbeeAdd("A4", buf);
        }
    } 
    else if (memcmp(ptag, "V0", 2) == 0) {               
        if (pval[0] == '?') {
            sprintf(buf, "%d", V0);
            zxbeeAdd("V0", buf);                                //��ѯV0��ֵ
        } else {
            int value = atoi(pval);
            if (value >= 0) V0 = value;                         //�޸�V0��ֵ
        }
    }
    else if(memcmp(ptag, "V1", 2) == 0)
    {
        if(pval[0] == '?')                                      //��ѯV1��ֵ
        {           
            sprintf(buf, "%d", V1);
            zxbeeAdd("V1", buf);                                      
        }
        else                                                    //�޸�V1��ֵ
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                V1 = value;
            }
        }
    }
    else if(memcmp(ptag, "V2", 2) == 0)
    {
        if(pval[0] == '?')                                      //��ѯV2��ֵ
        {           
            sprintf(buf, "%d", V2);
            zxbeeAdd("V2", buf);                                      
        }
        else                                                    //�޸�V2��ֵ
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                V2 = value;
            }
        }
    }
    else if(memcmp(ptag, "V3", 2) == 0)
    {
        if(pval[0] == '?')                                      //��ѯV3��ֵ
        {           
            sprintf(buf, "%d", V3);
            zxbeeAdd("V3", buf);                                      
        }
        else                                                    //�޸�V3��ֵ
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                V3 = value;
            }
        }
    }
    else if(memcmp(ptag, "V4", 2) == 0)
    {
        if(pval[0] == '?')                                      //��ѯV4��ֵ
        {           
            sprintf(buf, "%d", V4);
            zxbeeAdd("V4", buf);                                      
        }
        else                                                    //�޸�V4��ֵ
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                V4 = value;
            }
        }
    }
    
    return ret;
}