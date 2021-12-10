/*********************************************************************************************
* �ļ���fallalg.c
* ���ߣ�zonesion 2016.12.2
* ˵�����������
* �޸ģ�Meixin 2017.05.31 ������ע��
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include <contiki.h>
#include <stdio.h>
#include <string.h>
#include <sys/stimer.h>
#include "gsm.h"
#include "zxbee.h"
//#include "zxbee.h"


PROCESS(utcp, "utcp");
int gsm_tcp_0_send(int len);
#define app_id  "2093722535"
#define app_key "qye7RhIYNwXAccrsY0UlwYkSPAd71cOQ"

/*********************************************************************************************
* ����
*********************************************************************************************/
static struct etimer et;                                        //etimer��ʱ��
struct stimer tcp_0_timer;                                      //stimer��ʱ��
static int tcp_0_status = 0;                                    //tcp״̬��־
static int request_send_cced_flag = 0;                          //����ҳ���ͺ���
static int tcp_0_send_len = 0;                                  //�������ݳ���
static char tcp_send_buf[256];                                  //�洢��������
static char cced_buf[24*7];                                     //��վ��Ϣ����
static void package_decode(char *buf, int len);                 //���ݰ����뺯��

/*********************************************************************************************
* ���ƣ�void _rs_gsm_query_cced(char *rsp)
* ���ܣ�GSM��ѯ
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_query_cced(char *rsp)
{
  if (rsp == NULL) {
    return;
  };
  if (memcmp(rsp, "OK", 2) == 0) {                              //OKָ��ƥ��
    printf(" cced %s\r\n", cced_buf);
    if (strlen(cced_buf) > 0) {
      if (tcp_0_send_len == 0) {
        sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{A2=%s}\"}", cced_buf);
        gsm_tcp_0_send(strlen(tcp_send_buf));
      } else {
        request_send_cced_flag = 1;
      }
    }
  }
  if (memcmp(rsp, "+CCED:", 6) == 0) {                          //��+CCED��ָ��
    //+CCED: 460,0,712c,df6a,536,56,34,0,0,0,0,0,0
    if (memcmp(rsp, "+CCED: 0,0,0,0,0,0,0,0,0,0,0,0,0", 32) == 0) return;
    char *p1 = &rsp[7];
    char *e1 = strchr(p1, ',');
    char *e2 = strchr(e1+1, ',');
    char *e3 = strchr(e2+1, ',');
    char *e4 = strchr(e3+1, ',');
    *e4 = 0;
    char *e5 = strchr(e4+1, ',');
    char *e6 = strchr(e5+1, ',');
    char *e7 = strchr(e6+1, ',');
    *e7 = 0;
 
    char dlen = strlen(cced_buf);
    if (dlen > 0) {
      cced_buf[dlen++] = ';';
    }
    strcpy(&cced_buf[dlen], p1);
    dlen = strlen(cced_buf);
    cced_buf[dlen++] = ',';
    strcpy(&cced_buf[dlen], e6+1);
  }
}
/*********************************************************************************************
* ���ƣ�void gsm_query_cced(void)
* ���ܣ�GSM��ѯ
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_query_cced(void)
{
  cced_buf[0] = 0;
  gsm_request_at("at+cced=0,15\r\n", 3000, _rs_gsm_query_cced);
}

/*********************************************************************************************
* ���ƣ�void _rs_gsm_tcp_0_status(char *rsp)
* ���ܣ�GSM TCP״̬
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_tcp_0_status(char *rsp)
{
  if (rsp == NULL) return;
  if (strcmp(rsp, "+ZIPSTATUS:ESTABLISHED") == 0) {
    tcp_0_status = 2;
  }
  if (strcmp(rsp, "+ZIPSTATUS:DISCONNECTED") == 0) {
    tcp_0_status = 0;
  }
}
/*********************************************************************************************
* ���ƣ�void gsm_tcp_0_status(void)
* ���ܣ�GSM TCP״̬
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_0_status(void)
{
  gsm_request_at("at+zipstatus=0\r\n", 3000, _rs_gsm_tcp_0_status);
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_tcp_0_open(char *rsp)
* ���ܣ�GSM TCP��
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_tcp_0_open(char *rsp)
{
  if (rsp == NULL) return;
  if (strcmp(rsp, "+ZIPSETUP:CONNECTED") == 0) {
    gsm_tcp_0_status();
  }
}


/*********************************************************************************************
* ���ƣ�void gsm_tcp_0_open(void)
* ���ܣ�GSM TCP��
* ������
* ���أ���
* �޸ģ�120.24.67.222
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_0_open(void)
{
  
  gsm_request_at("at+zipclose=0\r\n", 3000, NULL);
  gsm_request_at("at+ziptimeout=15,30\r\n", 3000, NULL);
  gsm_request_at("at+zipsetup=0,120.24.67.222,28082\r\n", 30000, _rs_gsm_tcp_0_open);
}
/*********************************************************************************************
* ���ƣ�void gsm_tcp_0_close(void)
* ���ܣ�GSM TCP�ر�
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_0_close(void)
{
   gsm_request_at("at+zipclose=0\r\n", 3000, NULL);
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_tcp_0_send(char *rsp)
* ���ܣ�GSM TCP����
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

void _rs_gsm_tcp_0_send(char *rsp)
{
  if (rsp == NULL) {
    tcp_0_send_len = 0;
    return;
  }
  if (strcmp(rsp, "ERROR") == 0) {
    tcp_0_send_len = 0;
    return;
  }
  if (strcmp(rsp, ">") == 0) {
    printf("tcp <<< %s\r\n", tcp_send_buf);
    gsm_tcp_write(tcp_send_buf, tcp_0_send_len);
    tcp_0_send_len = 0;
    stimer_set(&tcp_0_timer, 60);
  }
}

/*********************************************************************************************
* ���ƣ�int gsm_tcp_0_send(int len)
* ���ܣ�GSM TCP������������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int gsm_tcp_0_send(int len)
{
  char buf[32];
  if (tcp_0_send_len == 0 && (gsm_info.phone_status == PHONE_ST_IDLE)) {
    tcp_0_send_len = len;
    sprintf(buf, "at+zipsend=0,%d\r", len);
    gsm_request_at(buf, 30000, _rs_gsm_tcp_0_send);
    return len;
  }
  return -1;
}

/*********************************************************************************************
* ���ƣ�void gsm_tcp_0_recv(char *buf, int len)
* ���ܣ�GSM TCP����
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_0_recv(char *buf, int len)
{
  printf(" tcp >> %s\r\n", buf);
  //for (int i=0; i<len; i++) printf(" %02X", buf[i]);
  //printf("\r\n");
  package_decode(buf, len);
}


/*********************************************************************************************
* ���ƣ�int tag_process_b(void)
* ���ܣ���ǹ���_b
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int tag_process_b(void)
{
  if (tcp_0_send_len == 0) {
     //{"method":"sensor","data":"adc"}
    sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{");
    //tcp_send_buf[0] = '\0';
    return 0;
  }
  printf("w: tcp 0 send buf not empty!\r\n");
  return -1;
}

/*********************************************************************************************
* ���ƣ�void tag_process_e(void)
* ���ܣ���ǹ���_e
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void tag_process_e(void)
{
  int len = strlen(tcp_send_buf);
  tcp_send_buf[len++] = '}'; 
  tcp_send_buf[len++] = '\"'; //replace '&'
  tcp_send_buf[len++] = '}';
  tcp_send_buf[len] = 0;
  if (len > 29) {
    gsm_tcp_0_send(strlen(tcp_send_buf));                        //����������Ϣ
  }
}
/*********************************************************************************************
* ���ƣ�void package_decode(char *buf, int len)
* ���ܣ�������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void package_decode(char *buf, int len)
{
  if (buf[len-1] != 0) {
    printf("error: tcp 0 package format\r\n");
    return;
  }
  len --;
  //printf(buf);
  //printf("\r\n");
  /* {"method":"control","data":"adc"} */
  if (strstr(buf, "method") && strstr(buf, "control")) {
    char *pdat = strstr(buf, "data");
    if (pdat) {
      pdat = strstr(pdat, ":\"{");
      pdat += 3;
      buf[len - 3] = 0;
      printf("data : %s\r\n", pdat);
      
        if (tag_process_b() < 0) return;
 
        char *p = pdat;
        while (p != NULL) {
          char *e = strchr(p, ',');
          char *np = NULL;
          if (e != NULL) {
            *e = 0;
            np = e + 1;
          } 
          char *v = strchr(p, '=');
          if (v != NULL) {
            *v = '\0';
            v = v + 1;
            int ret;
            ret = z_process_command_call(p, v);
            //tag_process(p, v);
            if ((ret > 0) && (np > 0)) {
              int buf_lens = strlen(tcp_send_buf);
              tcp_send_buf[buf_lens] = ',';
              tcp_send_buf[buf_lens+1] = 0;
        }
          }
          p = np; 
        }
        if (tcp_send_buf[28] != 0)
          tag_process_e();
    }
  }
}
/*********************************************************************************************
* ���ƣ�void package_decode(char *buf, int len)
* ���ܣ���֤��
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int package_auth(void)
{
#if 0
  tcp_send_buf[0] = 'T';
  memcpy(&tcp_send_buf[1], gsm_info.imei, 15);
  tcp_send_buf[16] = '\n';
  return  17;
#else
  //{"method":"authenticate","uid":"12345","key":"3Wl6cI8TNi8fMkhCXkZzXzCTsst4LgqY","addr":"adfbc"}
  sprintf(tcp_send_buf,
     "{\"method\":\"authenticate\",\"uid\":\"%s\",\"key\":\"%s\",\"addr\":\"%s\",\"version\":\"0.1.0\", \"autodb\":false}",
     app_id, app_key, gsm_info.imei);
  return strlen(tcp_send_buf);
#endif
}
/*********************************************************************************************
* ���ƣ�int package_heartbeat(void)
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int package_heartbeat(void)
{
#if 0
  tcp_send_buf[0] = '\n';
  return 1;
#else
  static unsigned int seq = 0;
  sprintf(tcp_send_buf, "{\"method\":\"echo\", \"timestamp\":(%u, \"seq\":%u)}", 
          clock_time(), seq++);
  return strlen(tcp_send_buf);
#endif
}

/*********************************************************************************************
* ���ƣ�int package_data(char *buf)
* ���ܣ����ݰ�
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int package_data(char *buf)
{
  if (tcp_0_status == 2 && tcp_0_send_len == 0) 
  {
    sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"%s\"}", buf);
    gsm_tcp_0_send(strlen(tcp_send_buf));
    return 1;
  } 
  else 
  {
    printf("tcp can't send %s\r\n", buf);
    printf("tcp not connect or busy\r\n");
    return 0;
  }
}
/*********************************************************************************************
* ���ƣ�PROCESS_THREAD(utcp, ev, data)
* ����: UTCP������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PROCESS_THREAD(utcp, ev, data)
{ 
  PROCESS_BEGIN();
  
  gsm_tcp_recv_register(0, gsm_tcp_0_recv);                     //�󶨴�����
  while (1) {
    static unsigned int tick = 0;
    static int last_tcp_0_status = 0;
    tick ++;
    if (gsm_info.ppp_status == 2) {                             //����״̬
      if (tcp_0_status == 0) {
        gsm_tcp_0_open();                                       //��tcp
        tcp_0_status = 1;
        stimer_set(&tcp_0_timer, 60);                           //��ʱ������
      }
      if (tcp_0_status == 1 && stimer_expired(&tcp_0_timer)) {
        tcp_0_status = 0;
      }
      if (tcp_0_status != 0) { // 
        
        if (tick % 30 == 1) {                                   // check tcp 0 connect;
          gsm_tcp_0_status();
        }
      }
      if (tcp_0_status == 2) {
        if (last_tcp_0_status != 2) {                           //��1��
          gsm_tcp_0_send(package_auth());
        }
        if (request_send_cced_flag != 0) {
          if (tcp_0_send_len == 0) {
            sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{A2=%s}\"}", cced_buf);
            gsm_tcp_0_send(strlen(tcp_send_buf));
            request_send_cced_flag = 0;
          }
        }
        if (tcp_0_send_len==0 && stimer_expired(&tcp_0_timer)) {
          gsm_tcp_0_send(package_heartbeat());                  //��������������Ϣ
        }
      }
      last_tcp_0_status = tcp_0_status;
    }
    
    etimer_set(&et, CLOCK_SECOND);                              //��ʱ������
    PROCESS_YIELD();
  }
  
  PROCESS_END();
}
