#include "Ultrasonic.h"
#include "contiki.h"
#include "delay.h"
#include "motor.h"
#include "stdio.h"
#include "led.h"


TIM_ICInitTypeDef  TIM12_ICInitStructure;

/*********************************************************************************************
* ���ƣ�TIM12CH2_Cap_Init
* ���ܣ���ʱ��12ͨ��2���벶������
* ������arr���Զ���װֵ(TIM2,TIM12��32λ��!!)
*       psc��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void TIM12CH2_Cap_Init(u32 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  	//TIM12ʱ��ʹ��    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��GPIOBʱ��	
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //GPIO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
    GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��GPIO
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12); //PB15����λ��ʱ��12
    
    TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);
    
    //��ʼ��TIM12���벶�����
    TIM12_ICInitStructure.TIM_Channel = TIM_Channel_2;                  //CC1S=01 ѡ������� IC2ӳ�䵽TI2��
    TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   //ӳ�䵽TI1��
    TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //�����ز���
    TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //���������Ƶ,����Ƶ 
    TIM12_ICInitStructure.TIM_ICFilter = 0x00;                          //IC1F=0000 ���������˲��� ���˲�
    TIM_ICInit(TIM12, &TIM12_ICInitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����   
    TIM_ITConfig(TIM12,TIM_IT_Update|TIM_IT_CC2,ENABLE);//��������ж� ,����CC2IE�����ж�	
    TIM_Cmd(TIM12,DISABLE ); 	//ʧ�ܶ�ʱ��12    
}

    					
u32 TIM12CH2_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM12��32λ)
u8  TIM12CH2_CAPTURE_STA=0;	//���벶��״̬	

/*********************************************************************************************
* ���ƣ�TIM8_BRK_TIM12_IRQHandler
* ���ܣ���ʱ��12�жϷ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void TIM8_BRK_TIM12_IRQHandler(void)
{ 		    
    if(TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET)      //���
    {	                
        TIM_Cmd(TIM12,DISABLE ); 	                        //�رն�ʱ��   
        TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising);//CC1P=0 ����Ϊ�����ز���
        
        if(TIM12CH2_CAPTURE_STA&0x01)                           //��⵽�źţ�������̫Զ
        {
            ultrasonicValue = 510;
        }
        else                                                    //û���ź�
        {
            ultrasonicValue = 0;
        }
    }
    
    if(TIM_GetITStatus(TIM12, TIM_IT_CC2) != RESET)         //���������¼�2
    {	
        if(TIM12CH2_CAPTURE_STA&0x01)		                //����һ�������� 		
        {	  			
            TIM_Cmd(TIM12,DISABLE ); 	                    //�رն�ʱ��            
            TIM12CH2_CAPTURE_STA = 0x02;		                //��ǳɹ�����һ�θߵ�ƽ����
            TIM12CH2_CAPTURE_VAL=TIM_GetCapture2(TIM12);        //��ȡ��ǰ�Ĳ���ֵ.
            TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising);//CC1P=0 ����Ϊ�����ز���
            
            ultrasonicValue = (TIM12CH2_CAPTURE_VAL *  0.17);//�������
        }
        else  								                //��δ��ʼ,��һ�β���������
        {
            TIM12CH2_CAPTURE_STA = 0x01;		            //��ǲ�����������
            TIM_Cmd(TIM12,DISABLE ); 	                    //�رն�ʱ��               
            TIM_SetCounter(TIM12,0);
            TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Falling);//CC1P=1 ����Ϊ�½��ز���
            TIM_Cmd(TIM12,ENABLE ); 	                    //ʹ�ܶ�ʱ��
        }		    
    }			
    
	TIM_ClearITPendingBit(TIM12, TIM_IT_CC2|TIM_IT_Update); //����жϱ�־λ
}



/*********************************************************************************************
* ���ƣ�Ultrasonic_init
* ���ܣ���������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void Ultrasonic_init(void) 
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_trig, ENABLE);                     //ʹ��GPIOBʱ��
    
    GPIO_InitStructure.GPIO_Pin = trig_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //����ٶ�50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //����
    GPIO_Init(trig_port, &GPIO_InitStructure);                    //����������������GPIO
    GPIO_ResetBits(trig_port, trig_pin);
    
    
    GPIO_InitStructure.GPIO_Pin = echo_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //����ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //����ٶ�50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //����
    GPIO_Init(echo_port, &GPIO_InitStructure);                    //����������������GPIO
    
    /*10us��������ʱ30ms*/
    TIM12CH2_Cap_Init(3000,839);
}


/*********************************************************************************************
* ���ƣ�trigger_ultrasonic
* ���ܣ�����һ�γ��������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void trigger_ultrasonic()
{    
    TIM12CH2_CAPTURE_STA=0;			                    //���
    TIM12CH2_CAPTURE_VAL=0;                
    TIM12->CNT=0;                                        //��TIM�����Ĵ����ļ���ֵ����
    TIM_Cmd(TIM12,ENABLE ); 	                        //ʹ�ܶ�ʱ��
    
    /*����ʱ��*/  
    GPIO_SetBits(trig_port, trig_pin);
    delay_us(15);
    GPIO_ResetBits(trig_port, trig_pin);
}



/*����ȫ�ֱ���ultrasonicValue����λcm*/
float ultrasonicValue=0;

//��������������
PROCESS(ultrasonic,"ultrasonic");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �������
* ����: 
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(ultrasonic, ev, data)
{
    static struct etimer ultrasonic_timer;
    
    PROCESS_BEGIN();
    
    Ultrasonic_init();
    etimer_set(&ultrasonic_timer,50);                           //����etimer��ʱ��
    
    while (1) 
    {       
        PROCESS_WAIT_EVENT(); 
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&ultrasonic_timer);                    //����etimer��ʱ�� 

            trigger_ultrasonic();                               //����һ�β��
        }       
    }
    
    PROCESS_END();
}

