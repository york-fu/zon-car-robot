#include "SteeringEngine.h"
#include "led.h"
#include "delay.h"
#include "adc_key.h"
#include "stdio.h"
#include "math.h"
#include "adc_key.h"
#include "user.h"


/*1�ź�2�Ŷ��pwm�ȼ�ֵ*/
uint16 steer1Pwm=2500,steer2Pwm=500;

/*********************************************************************************************
* ���ƣ�tim2Pwm_init
* ���ܣ���ʼ����ʱ��2,����PWM��(����2·)
* ������arr:�Զ���װֵ
        psc:�Զ�Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void tim2Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);       //ʹ�ܶ�ʱ��ʱ�� 
	//ʹ��GPIO����ʱ��ʹ��
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
    //���TIM2 PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//TIM_CH3-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM2, ENABLE);                         //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���


	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;                          //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�������
	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);//    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);//     
	
	TIM_Cmd(TIM2, ENABLE);                                      //ʹ��TIM2
	TIM_CtrlPWMOutputs(TIM2,ENABLE);                            //MOE �����ʹ��
}


/*********************************************************************************************
* ���ƣ�tim3Pwm_init
* ���ܣ���ʼ����ʱ��,����PWM��(����4·)
* ������arr:�Զ���װֵ
        psc:�Զ�Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void tim3Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);       //ʹ�ܶ�ʱ��ʱ�� 
	//ʹ��GPIO����ʱ��ʹ��
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
    //���TIM3 PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//TIM_CH1-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);

	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM3, ENABLE);                         //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�������
	TIM_OCInitStructure.TIM_Pulse = 2500;                          //���ô�װ�벶��ȽϼĴ���������ֵ
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);// 
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���   
    
	TIM_OCInitStructure.TIM_Pulse = 500; 
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);//  
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���  
    
	TIM_OCInitStructure.TIM_Pulse = 1500; 
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);//    
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR3�ϵ�Ԥװ�ؼĴ���  
    
	TIM_OCInitStructure.TIM_Pulse = 2270; 
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);//     
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR4�ϵ�Ԥװ�ؼĴ���  
    	
	TIM_CtrlPWMOutputs(TIM3,ENABLE);                            //MOE �����ʹ��
	TIM_Cmd(TIM3, ENABLE);                                      //ʹ��TIM3
}



/*********************************************************************************************
* ����: tim5Int_init
* ����: ��ʱ���жϳ�ʼ��
* ����: 
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
void tim5Int_init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);         //ʹ��TIMʱ��
    
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	            //�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_Prescaler= psc;               //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);          //��ʼ��TIM
    
      TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);                  //����ʱ�������ж�

    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;               //��ʱ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;     //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;            //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM5,ENABLE);   
}

//��ʱ��5�жϷ�����,���ڿ��ƶ��
void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //����ж�
    {
        if(STEER1<steer1Pwm) STEER1++;
        else if(STEER1>steer1Pwm) STEER1--;
        
        if(STEER2<steer2Pwm) STEER2++;
        else if(STEER1>steer2Pwm) STEER2--;
    }
    TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //����жϱ�־λ
}




/*********************************************************************************************
* ���ƣ�steeringEngine_init
* ���ܣ������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void steeringEngine_init()
{
    //Ƶ��200Hz
    tim3Pwm_init(5000,84);
    //2ms�ж�
    tim5Int_init(2000,84);
    
    steer1Pwm = 2000;
    steer2Pwm = 500;
    STEER3 = 1500;
    STEER4 = 2270;
}




/*********************************************************************************************
* ���ƣ�steer1_control
* ���ܣ��������
* ������angle:���ת���Ƕȣ�0-160
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void steer1_control(float angle)
{
    angle = 180-angle;
    if(angle<20) angle = 20;
    if(angle>180) angle = 180;  
    
    steer1Pwm = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* ���ƣ�steer2_control
* ���ܣ��������
* ������angle:���ת���Ƕȣ�0-180,
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void steer2_control(float angle)
{
    if(angle<0) angle = 0;
    if(angle>180) angle = 180;  
    
    steer2Pwm = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* ���ƣ�steer3_control
* ���ܣ��������
* ������angle:���ת���Ƕȣ�0-180
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void steer3_control(float angle)
{
    if(angle<0) angle = 0;
    if(angle>180) angle = 180;
    
    STEER3 = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* ���ƣ�steer4_control
* ���ܣ��������
* ������angle:���ת���Ƕȣ�0-70
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void steer4_control(float angle)
{
    angle = 160-angle;
    if(angle<90) angle = 90;
    if(angle>160) angle = 160;
    
    STEER4 = 500+(u16)(angle*11.11);
}



/*��������*/
PROCESS(robot_arm,"robot_arm");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: ������̣���е�ۿ��ƽ���
* ����: ev:�¼���data:�¼�����������
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(robot_arm, ev, data)
{ 
    static struct etimer robotArm_timer;
    
    PROCESS_BEGIN();

    steeringEngine_init();    
    etimer_set(&robotArm_timer,50); //����etimer��ʱ�� 
    
    while(1) 
    {     
        steer1_control(V1);
        steer2_control(V2);
        steer3_control(V3);
        steer4_control(V4);
        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        etimer_reset(&robotArm_timer);
    }

    PROCESS_END();
}

