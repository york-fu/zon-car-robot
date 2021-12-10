#include "rgb_timer.h"
extern char V3, V4, V5;
/*********************************************************************************************
* ���ƣ�TIM3_Init
* ���ܣ���ʼ��TIM3
* ������period���Զ���װֵ�� prescaler��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ���ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
*       AHB Prescaler = 1;AHB��ʱ��HCLK=SYSCLK/1 = 168MHz;
*       TIM3������APB1�ϣ� APB1 Prescaler = 4��APB1��ʱ��Ƶ��PCLK1 = HCLK/4 = 42;Ft=2*PCLK1= 84MHz
*       Ft=��ʱ������Ƶ��,��λ:Mhz��
*********************************************************************************************/
void hardware_pwm_init(unsigned short period,unsigned short prescaler)//TIM_PeriodΪ16λ����
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;               //��ʱ������
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��GPIOBʱ��
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);         //PB0��PB1����Ϊ��ʱ��3
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3);         //PB0��PB1����Ϊ��ʱ��3

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;          //PB0��PB1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
  GPIO_Init(GPIOB,&GPIO_InitStructure);                         //��ʼ��PB0��PB1

  TIM_TimeBaseStructure.TIM_Period = period;                    //��������װֵ
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;              //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //����ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //���������ó�ʼ��TIM3
  //��ʼ��TIM3 Channel1 PWMģʽ

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������:TIM����Ƚϼ��Ե�
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //����Tָ���Ĳ�����ʼ������TIM1 4OC1
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);                      //����Tָ���Ĳ�����ʼ������TIM1 4OC1

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ���
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ���

  TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPEʹ��
  TIM_Cmd(TIM3, ENABLE);                                        //ʹ��TIM3
}

/*********************************************************************************************
* ���ƣ�TIM4_Init
* ���ܣ���ʼ��TIM4
* ������period���Զ���װֵ�� prescaler��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ���ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
*       AHB Prescaler = 1;AHB��ʱ��HCLK=SYSCLK/1 = 168MHz;
*       TIM3������APB1�ϣ� APB1 Prescaler = 4��APB1��ʱ��Ƶ��PCLK1 = HCLK/4 = 42;Ft=2*PCLK1= 84MHz
*       Ft=��ʱ������Ƶ��,��λ:Mhz��
*********************************************************************************************/
void software_pwm_init(unsigned short period,unsigned short prescaler)//TIM_PeriodΪ16λ����
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;               //��ʱ������
  NVIC_InitTypeDef  NVIC_InitStructure;                         //�ж�����

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��GPIOBʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                     //PB2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
  GPIO_Init(GPIOB,&GPIO_InitStructure);                         //��ʼ��PB2

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;               //TIM4�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            //�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);			        //�����������ó�ʼ���ж�

  TIM_TimeBaseStructure.TIM_Period = period;                    //��������װֵ
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;              //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //����ģʽ
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);               //���������ó�ʼ��TIM4

  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                      //����ʱ��4�����ж�
  TIM_Cmd(TIM4, ENABLE);                                        //ʹ��TIM4
}

/*********************************************************************************************
* ���ƣ�TIM4_IRQHandler
* ���ܣ�TIM3�жϴ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned int tick = 0;
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update ) != RESET) {         //����жϱ�־������
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);                 //����жϱ�־
    tick++;
    if(tick >= V5)
      GPIO_SetBits(GPIOB, GPIO_Pin_2);
    else
      GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    if(tick >= 256)   tick = 0;
  }
}

void rgb_init(void)
{
  hardware_pwm_init(500-1, 84-1);                                       //��ʼ��TIM3��84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.
  software_pwm_init(2-1, 84-1);                                         //��ʼ��TIM4��84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ1Mhz.
}