/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����LED��������
*       D1->PA5;D2->PA7;D3->PE2;D4->PE3;R->PB0;G->PB1;B->PB2
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "led.h"
#include "rgb_timer.h"


/*********************************************************************************************
* ���ƣ�led_init
* ���ܣ���ʼ��LED��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE |
                         RCC_AHB1Periph_GPIOA , ENABLE);        //ʹ��GPIOʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;        //ѡ��2��3������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //����ٶ�2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������
  GPIO_Init(GPIOE, &GPIO_InitStructure);                        //����������������GPIOE2��GPIOE3
  GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;        //ѡ��5��7������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                        //����������������GPIOA5��GPIOA7
  GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
}

/*********************************************************************************************
* ���ƣ�turn_on_off
* ���ܣ������ŵ͵�ƽ����LED
* ������led
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void turn_on_off(unsigned char led){
  if(led & LED1)
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);
  else
    GPIO_SetBits(GPIOE, GPIO_Pin_2);
  if(led & LED2)
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  else
    GPIO_SetBits(GPIOE, GPIO_Pin_3);
  if(led & LED3)
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
  else
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
  if(led & LED4)
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
  else
    GPIO_SetBits(GPIOA, GPIO_Pin_7);
}
/*********************************************************************************************
* ���ƣ�turn_on
* ���ܣ������ŵ͵�ƽ����LED
* ������led
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_led_status(void){
  unsigned char led_status = 0;
  if(GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_2))
    led_status |= LED1;
  else
    led_status &= ~LED1;
  
  if(GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_3))
    led_status |= LED2;
  else
    led_status &= ~LED2;
  
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5))
    led_status |= LED3;
  else
    led_status &= ~LED3;
  
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7))
    led_status |= LED4;
  else
    led_status &= ~LED4;
  return led_status;
}


/*********************************************************************************************
��������
*********************************************************************************************/
PROCESS(led,"led");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �������
* ����: 
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(led, ev, data)
{
    static struct etimer led_timer;
    
    PROCESS_BEGIN();
    led_init();
    etimer_set(&led_timer,1000);
    
    while (1) 
    {
        PROCESS_WAIT_EVENT();                                       //�ȴ��¼�����
            
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&led_timer);                             //���ö�ʱ��            
            led1 = !led1;
        }      
    }
    
    PROCESS_END();
}