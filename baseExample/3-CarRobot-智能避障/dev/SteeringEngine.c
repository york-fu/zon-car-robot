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
* ������angle:���ת���Ƕȣ�20-180,
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
* ������angle:���ת���Ƕȣ�0-180,
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
* ������angle:���ת���Ƕȣ�0-180,
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


/*********************************************************************************************
��е�ۿ��Ʋ���
*********************************************************************************************/
_robotArm robotArm = {6,10,90,0};




/*********************************************************************************************
��е�۶����¼�
*********************************************************************************************/
process_event_t robotArm_motion_event;



/*��������*/
PROCESS(robotArm_motion,"robotArm_motion");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: ������̣���е���Զ��嶯��
* ����: ev:�¼���data:�¼�����������
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(robotArm_motion, ev, data)
{   
    static struct etimer robotArm_motion_t;
    
    PROCESS_BEGIN();   
    
    while(1)
    {      
        PROCESS_WAIT_EVENT();           
        if(ev == robotArm_motion_event)
        {
            /*�Լ춯��*/
            if((*(uint8*)data)==1)
            {   
                /*3,4��λ*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2����*/            
                V3 = 10;
                V4 = 30;
                etimer_set(&robotArm_motion_t,3000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�ſ�*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3��ʱ��*/
                V5 = 0;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3˳ʱ��*/
                V5 = 180;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3��ʱ��*/
                V5 = 90;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
              
                /*4�պ�*/
                V6 = 0;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
           
                /*��λ*/
                V3 = 6;
                V4 = 10;
            }
            /*��ˮ����*/
            else if(*(uint8*)data==2)
            {               
                /*3,4��λ*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�ſ� 1,2ǰ��*/
                V6 = 70;
                V3 = 17;
                V4 = 5;
                etimer_set(&robotArm_motion_t,2500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�պ�*/
                V6 = 50;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2����*/
                V3 = 17;
                V4 = 25;
                etimer_set(&robotArm_motion_t,2000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3��ʱ��*/
                V5 = 0;
                etimer_set(&robotArm_motion_t,1500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3˳ʱ��*/
                V5 = 90;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2����*/
                V3 = 17;
                V4 = 5;
                etimer_set(&robotArm_motion_t,1500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�ſ�*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*��λ*/
                V3 = 6;
                V4 = 10;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�պ�*/
                V6 = 0;
            }
            /*������*/
            else if(*(uint8*)data==3)
            {               
                /*3,4��λ*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�ſ� 1,2����*/
                V6 = 70;
                V3 = 5;
                V4 = 2;
                etimer_set(&robotArm_motion_t,2000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                
                /*4�պ�*/
                V6 = 20;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*1,2����*/
                V3 = 8;
                for(;V4<27;V4++)
                {
                    etimer_set(&robotArm_motion_t,100); //����etimer��ʱ��
                    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                }                
                
                /*1,2ǰ��*/
                V3 = 17;
                etimer_set(&robotArm_motion_t,2000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                              
                /*4�ſ�*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*��λ*/
                V3 = 6;
                V4 = 10;
                V6 = 0;               
            }
            /*�Ӷ���*/
            else if(*(uint8*)data==4)
            {
                /*3,4��λ*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4�ſ� 1,2����*/
                V6 = 70;
                V3 = 7;
                V4 = 2;
                etimer_set(&robotArm_motion_t,2000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                
                /*4�պ�*/
                V6 = 20;
                etimer_set(&robotArm_motion_t,500); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*�ӳ�*/
                TIM_Cmd(TIM5,DISABLE);                          //�رն�ʱ�� 
                V3 = 17;
                V4 = 28;
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                STEER1 = steer1Pwm;
                STEER2 = steer2Pwm; 
                etimer_set(&robotArm_motion_t,300); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                V6 = 70;   
                etimer_set(&robotArm_motion_t,1000); //����etimer��ʱ��
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                TIM_Cmd(TIM5,ENABLE);                          //�򿪶�ʱ�� 
                
                /*��λ*/
                V3 = 6;
                V4 = 10;
                V6 = 0;
            }
            V7 = 0;
        }
    }
    PROCESS_END();
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
    double a1=0,a2=0;
    double x1=0,y1=0,temp1=0,temp2=0;
    static struct etimer robotArm_timer;
    
    PROCESS_BEGIN();

    steeringEngine_init();     
//    robotArm_motion_event = process_alloc_event();
    V7 = 1;                                                     //�����Լ��¼�
    etimer_set(&robotArm_timer,100); //����etimer��ʱ��
    
    while(1) 
    {                                    
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        etimer_reset(&robotArm_timer);  
    
        /*ȡֵ��Χ*/
        temp1 = L1+L2;
        if(robotArm.x<1) robotArm.x=1;
        if(robotArm.x>temp1) robotArm.x=temp1;
        if(robotArm.y<1) robotArm.y=1;
        if(robotArm.y>temp1+H) robotArm.y=temp1+H;
        
        x1 = L1/sqrt(2);
        y1 = x1+H;
        if( (((robotArm.x+x1)*(robotArm.x+x1)+(robotArm.y-y1)*(robotArm.y-y1))>=(L2*L2))
           && (robotArm.x*robotArm.x+((robotArm.y-H)*(robotArm.y-H)))<=((L1+L2)*(L1+L2)) )
        {        
            /*���㻡��*/
            temp1 = ((L1*L1)+(robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H))-(L2*L2))/(2*L1*sqrt((robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H))));
            temp2 = (H-robotArm.y)/sqrt((robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H)));        
            a1 = pi-acos(temp1)-acos(temp2);
            a2 = acos(((L1*L1)+(L2*L2)-(robotArm.x*robotArm.x)-((robotArm.y-H)*(robotArm.y-H)))/(2*L1*L2));
            
            /*����ת��Ϊ�Ƕ�*/
            a1 = a1*(180.0/pi) + 40;
            a2 = a2*(180.0/pi);
            
            steer1_control(a1);
            steer2_control(a2);
            steer3_control(robotArm.angle_3);
            steer4_control(robotArm.angle_4);
        }
    }

    PROCESS_END();
}
//1 --> 20-180
//2 --> 175-0

