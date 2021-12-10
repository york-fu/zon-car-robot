#include "E18-D80NK.h"
#include "contiki.h"
/*********************************************************************************************
* ���ƣ�E18D80NK_init
* ���ܣ���ʼ�������Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void E18D80NK_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_D80NK , ENABLE);                   //ʹ��GPIOBʱ��
  
  GPIO_InitStructure.GPIO_Pin = pin_left | pin_right;           //ѡ��PB8��PB9����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //�ٶ�50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������
  GPIO_Init(left_port, &GPIO_InitStructure);                    //����������������GPIO
}

u8 get_state(void)
{
  char state=0x00;
  
  if(GPIO_ReadInputDataBit(left_port,pin_left)==1)
    state |=0x01;
  if(GPIO_ReadInputDataBit(right_port,pin_right)==1)
    state |=0x02;
  
  return state;
}