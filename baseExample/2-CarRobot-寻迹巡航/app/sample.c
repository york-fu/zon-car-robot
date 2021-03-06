/*********************************************************************************************
* 文件：sample.c
* 作者：
说明：
*   传感器控制参数设置：  A0表示环境光照度数据;
*                        A1表示环境温度数据;
*                        A2表示环境湿度数据;
*                        V0表示主动上报时间间隔;
*                        D0(Bit0/Bit1)表示上报状态，默认设置D0=3;
* 修改：
* 注释：
*********************************************************************************************/
#include "sample.h"
#include "zxbee.h"
#include "pcf8563.h"
#include "w25qxx.h"
#include "bh1750.h"
#include "htu21d.h"
#include "led.h"
#include "infrared.h"
#include "rtc.h"
#include "config.h"
#include "delay.h"
#include "GPS.h"
#include "motor.h"
#include "user.h"
#include "ble.h"


/*温度传感器参数*/
extern float htu21dValue_t;
extern float htu21dValue_h;

/*********************************************************************************************
传感器参数
*********************************************************************************************/
float A0, A1, A2;                                               //光照度、温度、湿度
RTC_TimeTypeDef RTC_TimeStruct;                                 //时间
RTC_DateTypeDef RTC_DateStruct;                                 //日期


/*********************************************************************************************
* 名称：sensor_init()
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_init(void)
{
    bh1750_init();                                              //光照度传感器
    My_RTC_Init();                                              //初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	        //配置WAKE UP中断,1秒中断一次
} 


/*********************************************************************************************
* 名称：sensor_poll()
* 功能：轮询传感器，并主动上报传感器数据
* 参数：无
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_poll(void)
{
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);  
    
    A0 = bh1750_get();                                            //读取光照度值
    A1 = htu21dValue_t;                                           //读取温度
    A2 = htu21dValue_h;                                           //读取湿度
}

