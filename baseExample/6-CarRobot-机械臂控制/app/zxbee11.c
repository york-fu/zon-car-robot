#include "contiki.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stimer.h>
#include "usart.h"
//#include "rf/zigbee-net.h"
//#include "dev/relay.h"



PROCESS(zxbee, "zxbee");

extern float gLight, gTemp, gHumi;
extern uint16_t gADC1, gADC2, gADC3, gADC4;
extern unsigned int PM1_0, PM2_5, PM10;
extern process_event_t process_event_key;

extern int z_process_command_call(char* ptag, char* pval, char* obuf);
extern void sensor_init(void);
extern void sensor_poll(unsigned int);
extern char *sensor_type(void);

static void (*_onrecv_call)(char *, int) = NULL;

static char D0 = 0xF0;                                          // �����ϱ����أ�Ĭ��ȫ��
static unsigned int V0 = 30;                                    // �����ϱ�ʱ��������λ��,0�������ϱ�
static float /*A0, A1, A2, A3,*/ A4, A5, A6, A7;
static char D1 = 0;
//static float A0, A1, A2, A3, A4, A5, A6, A7;


void zxbee_send(char *str)
{
  char buf[32];
  int r;
  int len = strlen(str);
  r = sprintf(buf, "at+send=%u\n", len);
  usart6_send(buf, r);
  usart6_send(str, len);
}

static char wbuf[96];
static void map(void)
{
}
  
void zxbeeBegin(void)
{
  wbuf[0] = '{';
  wbuf[1] = '\0';
}
int zxbeeAdd(char* tag, char* val)
{
  int offset = strlen(wbuf);
  sprintf(&wbuf[offset],"%s=%s,", tag, val);
  return 0;
}
char* zxbeeEnd(void)
{
  int offset = strlen(wbuf);
  wbuf[offset-1] = '}';
  wbuf[offset] = '\0';
  if (offset > 2) return wbuf;
  return NULL;
}

static int _process_command_call(char* ptag, char* pval, char* obuf)
{
  int ret = -1;
 if (memcmp(ptag, "TYPE", 4) == 0) {
    if (pval[0] == '?') {
      //extern int zigbee_type(void);
      //int t = zigbee_type();
      //if (t < 0)  t = 2;
      //ret = sprintf(obuf, "TYPE=1%d%s", t, sensor_type());
    }
  }
  return ret;
}
static void _zxbee_onrecv_fun(char *pkg, int len)
{
   char *p;
  char *ptag = NULL;
  char *pval = NULL;
  //int len = strlen(pkg);
  char *pwbuf = wbuf+1;
  
  
  if (pkg[0] != '{' || pkg[len-1] != '}') return;
  
  pkg[len-1] = 0;
  p = pkg+1; 
  do {
    ptag = p;
    p = strchr(p, '=');
    if (p != NULL) {
      *p++ = 0;
      pval = p;
      p = strchr(p, ',');
      if (p != NULL) *p++ = 0;
      /*if (process_command_call != NULL) */{
        int ret;
        ret = _process_command_call(ptag, pval, pwbuf);
        if (ret < 0) {
           ret = z_process_command_call(ptag, pval, pwbuf);
        }
        if (ret > 0) {
          pwbuf += ret;
          *pwbuf++ = ',';
        } 
      }
    }
  } while (p != NULL);
  if (pwbuf - wbuf > 1) {
    wbuf[0] = '{';
    pwbuf[0] = 0;
    pwbuf[-1] = '}';
    zxbee_send(wbuf);
  }
}


int z_process_command_call(char* ptag, char* pval, char* obuf)
{
  int ret = -1;							
  if (memcmp(ptag, "D1", 2) == 0) {				// ��ȡD1״̬
    if (pval[0] == '?') {
      ret = sprintf(obuf, "D1=%d", D1);
    }
  }
  if (memcmp(ptag, "CD1", 3) == 0) {				// �ر�D1
     int v = atoi(pval);
       D1 &= ~v;
       //sensor_control(D1);
     }
  
  if (memcmp(ptag, "OD1", 3) == 0) {				// ��D1
     int v = atoi(pval);  
       D1 |= v;
       //sensor_control(D1);
     }
  
  
  if (memcmp(ptag, "D0", 2) == 0) {				// ��ȡD0״̬
    if (pval[0] == '?') {
      ret = sprintf(obuf, "D0=%d", D0);
    }
  }
  if (memcmp(ptag, "CD0", 3) == 0) {				// �ر�D0
     int v = atoi(pval);
     if (v > 0) {
       D0 &= ~v;
     }
  }
  if (memcmp(ptag, "OD0", 3) == 0) {				// ��D0
     int v = atoi(pval);
     if (v > 0) {
       D0 |= v;
     }
  }
  if (memcmp(ptag, "V0", 2) == 0) {				// ��ȡV0��ֵ
    if (pval[0] == '?') {
      ret = sprintf(obuf, "V0=%d", V0);
    } else {
      int v0 = atoi(pval);
      if (v0 >= 0) V0 = v0;
    }
  }
  if (memcmp(ptag, "A4", 2) == 0) {				// ��ȡA4��ֵ
    if (pval[0] == '?') {
      ret = sprintf(obuf, "A4=%.1f", A4);
    }
  }
  if (memcmp(ptag, "A5", 2) == 0) {				// ��ȡA5��ֵ
    if (pval[0] == '?') {
      ret = sprintf(obuf, "A5=%.1f", A5);
    }
  }
  if (memcmp(ptag, "A6", 2) == 0) {				// ��ȡA6��ֵ
    if (pval[0] == '?') {
      ret = sprintf(obuf, "A6=%.1f", A6);
    }
  }
   if (memcmp(ptag, "A7", 2) == 0) {				// ��ȡA7��ֵ
    if (pval[0] == '?') {
      ret = sprintf(obuf, "A7=%.1f", A7);
    }
  } 
  return ret;
}


void zxbee_set_onrecv(void (*f)(char *, int))
{
  _onrecv_call = f;
}

PROCESS_THREAD(zxbee, ev, data)
{
  static struct etimer _timer;
  static struct etimer _map_timer;
  //static unsigned int tick = 0;
  PROCESS_BEGIN();
  
  //relay_on(2);
  
  zxbee_set_onrecv(&_zxbee_onrecv_fun);
  //sensor_init();
  etimer_set(&_timer, 5+(rand()%25) * CLOCK_SECOND);
  etimer_set(&_map_timer, CLOCK_SECOND);
  while(1) {
    PROCESS_YIELD();
    if (etimer_expired(&_map_timer)) {
      map();
      //sensor_poll(++tick);
      etimer_set(&_map_timer, CLOCK_SECOND);
    }
    if (ev==process_event_key) { 
    }
  }
  PROCESS_END();
}