/******************************************************
*�ļ�: infrared.c
*����: liding 2015.9.22
*˵��: ������⴫����ʵ��
*�޸�:
*ע��:
*******************************************************/
#include "infrared.h"

/*********************************************************************************************
* ���ƣ�GPIO_init
* ���ܣ����ܳ�ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void infrared_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         //ʹ��GPIODʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                     //PD3
  GPIO_InitStructure.GPIO_Speed =  GPIO_High_Speed;             //����
  GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_IN;                //����
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_OD;               //��©ģʽ
  GPIO_InitStructure.GPIO_PuPd  =  GPIO_PuPd_DOWN ;             //����

  GPIO_Init(GPIOD, &GPIO_InitStructure);                        //��ʼ��

}

/*********************************************************************************************
* ���ƣ�Get_State
* ���ܣ���ȡ���ŵĵ�ƽֵ
* ������GPIO_TypeDef* GPIOx GPIO�� uint16_t GPIO_Pin ���ź�
* ���أ���ƽ״̬
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int infrared_get(void)
{

  if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)){  //��ȡPD3�ĵ�ƽ
    return 1;
  }
  else{
    return 0;
  }
}
