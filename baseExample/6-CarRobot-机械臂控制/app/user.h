#ifndef _user_h_
#define _user_h_

#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "contiki.h"


extern uint8 V1,V2,V3,V4;

extern uint8 connect_mode;
extern char RADIO_MAC[32];

//��������
extern struct process user;

extern process_event_t d2Recv_event;

#endif