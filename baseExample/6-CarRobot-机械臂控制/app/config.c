#include "flamanage.h"
#include "config.h"
#include <contiki.h>
#include "w25qxx.h"
#include "user.h"



void config_save_mode()
{
    FlaVPageWrite(5, 0, &connect_mode, 1);
}


void config_read_mode()
{
    FlaVPageRead(5, 0, &connect_mode, 1);
}


void config_init(void)
{
    FlaManageInit();
    int flag = 0;
    FlaVPageRead(1, 0, &flag, 4);
    if (flag != 0x5a5a5a5c) 
    {
        FlaFormat();
        flag = 0x5a5a5a5c;
        FlaVPageWrite(1, 0, &flag, 4);
        config_save_mode();
    } 
    else 
    {
        config_read_mode();
    }
}



PROCESS(config, "config");

/*********************************************************************************************
* ���ƣ�PROCESS_THREAD(config, ev, data)
* ���ܣ��������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PROCESS_THREAD(config, ev, data)
{
    PROCESS_BEGIN();                                              //��ʼ����
    W25QXX_Init();
    config_init();                                                //���ó�ʼ��
    PROCESS_END();                                                //��������
}