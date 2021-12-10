#include "user.h"
#include "rtc.h"
#include "led.h"
#include "sample.h"


//��������
PROCESS(user,"user");

/*********************************************************************************************
* ����: PROCESS_THREAD()
* ����: �������
* ����:
* ����: ��
* �޸�:
* ע��:
*********************************************************************************************/
PROCESS_THREAD(user, ev, data)
{    
    static struct etimer user_timer;
    
    PROCESS_BEGIN();
    
    sensor_init();                                              //��������ʼ��
    etimer_set(&user_timer,200);                                //����etimer��ʱ��
    
    while(1)
    {  
        sensor_poll();                 //��������ѯ����
        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    //�ȴ�etimer��ʱ������¼�
        etimer_reset(&user_timer);                              //����etimer��ʱ��
    }
    
    PROCESS_END();
}

            
          