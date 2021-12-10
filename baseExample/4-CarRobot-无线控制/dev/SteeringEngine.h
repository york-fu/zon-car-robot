#ifndef _SteeringEngine_H_
#define _SteeringEngine_H_

#include "stm32f4xx.h"
#include "extend.h"
#include "contiki.h"

#define STEER1 TIM3->CCR1		//CH1
#define STEER2 TIM3->CCR2		//CH2
#define STEER3 TIM3->CCR3		//CH3
#define STEER4 TIM3->CCR4		//CH4

/*pi*/
#define pi 3.1416
/*机械臂尺寸*/
#define L1 10.0
#define L2 15.0
#define H  11.0

/*********************************************************************************************
机械臂控制结构体
*********************************************************************************************/
typedef struct 
{
    double x;
    double y;          //机械臂目标坐标
    double angle_3;    //3号舵机转动角度
    double angle_4;    //4号舵机张开角度
}_robotArm;


/*********************************************************************************************
机械臂PID参数结构体
*********************************************************************************************/
typedef struct 
{
    float Kp;
    float Ki;
    float Kd;
}_PID;


/*********************************************************************************************
机械臂控制参数
*********************************************************************************************/
extern _robotArm robotArm;



/*********************************************************************************************
机械臂动作事件
*********************************************************************************************/
extern process_event_t robotArm_motion_event;

/*********************************************************************************************
机械臂动作进程
*********************************************************************************************/
extern struct process robotArm_motion;


/*声明舵机控制进程*/
extern struct process SteerControl;

/*********************************************************************************************
* 名称：steeringEngine_init
* 功能：舵机初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steeringEngine_init();

#endif