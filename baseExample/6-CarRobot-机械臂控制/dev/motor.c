#include "motor.h"
#include "contiki.h"
#include "E18-D80NK.h"
#include "Ultrasonic.h"
#include "led.h"

/*********************************************************************************************
* ���ƣ�tim4Pwm_init
* ���ܣ���ʼ����ʱ��,����PWM��(����4·)
* ������arr:�Զ���װֵ
psc:�Զ�Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void tim4Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);       //ʹ�ܶ�ʱ��ʱ�� 
	//ʹ��GPIO����ʱ��ʹ��
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
    //���TIM3 PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//TIM_CH1-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //�ٶ�100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4);
    
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM4, ENABLE);                         //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
    
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //�������
	TIM_OCInitStructure.TIM_Pulse = 0;                          //���ô�װ�벶��ȽϼĴ���������ֵ
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);//    
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);//   
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//    
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR3�ϵ�Ԥװ�ؼĴ���
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);//     
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);             //ʹ��TIM4��CCR4�ϵ�Ԥװ�ؼĴ���
    
    
	TIM_CtrlPWMOutputs(TIM4,ENABLE);                            //MOE �����ʹ��
	TIM_Cmd(TIM4, ENABLE);                                      //ʹ��TIM3
}

/*********************************************************************************************
* ���ƣ�motor_init
* ���ܣ���ʼ��motor��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void motor_init(void)
{
    //Ƶ��10KHz
    tim4Pwm_init(8399,0);
}


/*********************************************************************************************
* ���ƣ�motor_control
* ���ܣ��������
* ������LeftPwm����ߵ��PWMֵ
RightPwm���ұߵ��PWMֵ
Mode������ģʽ
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void motor_control(u16 LeftPwm,u16 RightPwm,u8 Mode)
{
    if(LeftPwm>8400) LeftPwm=8400;
    if(RightPwm>8400) RightPwm=8400;
    
    //ǰ
    if((Mode) == FRONT)                                     
    {       
        PWM1 = 0;
        PWM2 = LeftPwm;
        PWM3 = RightPwm;
        PWM4 = 0;	
    }
    //��
    else if((Mode) == REAR)                                
    {
        PWM1 = LeftPwm;
        PWM2 = 0;
        PWM3 = 0;
        PWM4 = RightPwm;
    }
    //��
    else if((Mode) == LEFT)                                
    {
        PWM1 = LeftPwm;
        PWM2 = 0;
        PWM3 = RightPwm;
        PWM4 = 0;
    }
    //��
    else if((Mode) == RIGHT)                               
    {
        PWM1 = 0;
        PWM2 = LeftPwm;
        PWM3 = 0;
        PWM4 = RightPwm;
    }
    else
    {
        PWM1 = 0;
        PWM2 = 0;
        PWM3 = 0;
        PWM4 = 0;
    }
}


/*********************************************************************************************
�ƶ�ģʽ����
*********************************************************************************************/
/*
0��ֹͣ
1���ֶ�
2��Ѱ��
3������
*/
u8 motion_mode=0;

/*********************************************************************************************
������Ʋ���
*********************************************************************************************/
u8 motor_direction=0;
u16 motor_speed=0;

//��������
PROCESS(motor,"motor");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �������
* ����:
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(motor, ev, data)
{
    static struct etimer motor1_timer;
    
    PROCESS_BEGIN();
    
    motor_init();                                               //�����ʼ��
    E18D80NK_init();                                            //����Ѱ����ʼ��
    etimer_set(&motor1_timer,20);                               //����etimer��ʱ��
    
    while (1) 
    {        
        PROCESS_WAIT_EVENT();
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&motor1_timer); 
            
            /*�ֶ�����ģʽ*/
            if(motion_mode==1)
            {
                motor_control(motor_speed,motor_speed,motor_direction);
            }
            /*Ѱ��ģʽ*/
            else if(motion_mode==2)
            {
                u8 infraredSta = get_state();
                if((infraredSta&0x03)==0x03)
                {
                    led3 = 0;
                    led4 = 0;
                    motor_control(motor_speed,motor_speed,STOP);//ֹͣ                  
                }
                else
                {
                    if((infraredSta&0x01)==0x01)
                    { 
                        led3 = 1;
                        led4 = 0;
                        motor_control(motor_speed,motor_speed,LEFT);//��ת
                    }
                    else if((infraredSta&0x02)==0x02)
                    {      
                        led3 = 0;
                        led4 = 1;
                        motor_control(motor_speed,motor_speed,RIGHT);//��ת
                    }
                    else
                    {        
                        led3 = 1;
                        led4 = 1;
                        motor_control(motor_speed,motor_speed,FRONT);//ǰ��
                    }
                }
            }
            /*����ģʽ*/
            else if(motion_mode==3)
            {
                if(ultrasonicValue<30)
                {                    
                    motor_control(0,0,0);
                }
                else
                {                    
                    motor_control(motor_speed,motor_speed,FRONT);
                }
            }
            else
            {               
                motor_control(0,0,0);
            }
            
            if(motion_mode!=2)
            {      
                led3 = 1;
                led4 = 1; 
            }
        }
    }
    
    PROCESS_END();
}

