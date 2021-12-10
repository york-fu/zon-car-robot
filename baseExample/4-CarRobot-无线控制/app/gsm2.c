#include <contiki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsm.h"
#include "sys/ctimer.h"
#include "mg2639.h"
#include "user.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define _RESPONSE_BUF_SIZE 256
PROCESS(gsm, "gsm");

gsm_info_t gsm_info;

static int gsm_response_mode = 0;
static char  _response_buf[_RESPONSE_BUF_SIZE];
static int _response_offset = 0;

//int gsm_phone_status = PHONE_ST_IDLE;
//atָ��ṹ��
struct _at_request{
    void *req;
    void (*fun)(char * response);
    unsigned int timeout_tm;
    struct _at_request *next;
};

struct _at_request *current_at = NULL;
struct _at_request * list_at = NULL;

/*********************************************************************************************
* ���ƣ�_request_at_1(char *at)
* ���ܣ����� 1
* ������*at --  
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _request_at_1(char *at)
{
    _request_at_3(at, 1000, NULL);
}

/*********************************************************************************************
* ���ƣ�int _request_at_3(char *at, unsigned int to, void (*fun)(char*))
* ���ܣ����atָ�ָ��ȴ��б�
* ������*at --     to --   
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int _request_at_3(char *at, unsigned int to, void (*fun)(char*))
{
    int len = strlen(at);                                         //����ָ���
    char *p = malloc(len+1);                                      //�����ڴ�
    if (p) strcpy(p, at);                                         //����ָ��
    else return -1;
    struct _at_request *pa = malloc(sizeof (struct _at_request)); //�����ڴ�
    if (pa) {
        pa->req = p;
        pa->next = NULL;
        pa->fun = fun;
        pa->timeout_tm = to;                                        //��¼��ʱʱ��
        if (list_at == NULL) list_at = pa;
        else {
            struct _at_request *pl = list_at;
            while (pl->next != NULL) pl = pl->next;
            pl->next = pa; 
        }
    } else {
        free(p);                                                    //�ͷ��ڴ�
        return -1;
    }
    return 0;
}
/*********************************************************************************************
* ���ƣ�end_process_at(void)
* ���ܣ�����������
* �������� 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void end_process_at(void)
{
    if (current_at != NULL) {                                     //ָ��ȴ��б�Ϊ��
        free(current_at->req);
        free(current_at);
        current_at = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////
void check_sim_card(void);
/*********************************************************************************************
* ���ƣ�static void _rp_check_gsm(char *rsp)
* ���ܣ���̬��GSM��麯��
* ������*rsp -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void _rp_check_gsm(char *rsp)                            //GSM���
{
    if (rsp == NULL) {
        //_request_at_3("at\r\n", 3000, _rp_check_gsm);
    } else if (memcmp(rsp, "OK", 2) == 0) {
        gsm_info.gsm_status = 1;
    }
}
/*********************************************************************************************
* ���ƣ�void check_gsm(void)
* ���ܣ�GSM��麯��
* ������*rsp -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void check_gsm(void)
{
    _request_at_3("at\r\n", 3000, _rp_check_gsm);
}

/*********************************************************************************************
* ���ƣ�static void _rs_check_sim_card(char *rsp)
* ���ܣ���̬��SIM��麯��
* ������*rsp -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

static void _rs_check_sim_card(char *rsp)
{
    if (rsp == NULL) {
    } 
    if (memcmp(rsp, "*TSIMINS:", 9)==0) {
        if (rsp[12] == '1') {
            gsm_info.gsm_status = 2;
            gsm_info.simcard_status = 1;
        } else {
            gsm_info.gsm_status = 3;
            gsm_info.simcard_status = 0;     
        }
    } 
}

/*********************************************************************************************
* ���ƣ�void check_sim_card(void)
* ���ܣ�SIM����麯��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void check_sim_card(void)
{
    //_request_at_3("at+cpin?\r\n", 3000, _rs_check_sim_card);
    _request_at_3("at*tsimins?\r\n", 3000, _rs_check_sim_card);
}

/*********************************************************************************************
* ���ƣ�static void _rs_register_network(char *rsp)
* ���ܣ��Ǽ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void _rs_register_network(char *rsp)
{
    if (rsp != NULL && memcmp(rsp, "+COPS:", 6) == 0) {
        //+COPS: 0,2,"46000"
        if (strlen(rsp) > 12) {
            char *p = &rsp[12];
            char *e = strchr(p, '\"');
            if (e) {
                *e = '\0';
                strcpy(gsm_info.network, p);
            }
        }
    }
}
/*********************************************************************************************
* ���ƣ�void gsm_register_network(void)
* ���ܣ�GSM�Ǽ���������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_register_network(void)
{
    _request_at_3("at+cops=0,2\r\n", 5000, NULL);
    _request_at_3("at+cops?\r\n", 5000, _rs_register_network);
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_call_accept(char *res)
* ���ܣ�GSM�绰����
* ������*res -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_call_accept(char *res)
{
    if (res == NULL) {
        printf(" error : gsm mast not timeout whic ruan ata\r\n");
        return;
    }
    if (memcmp(res, "OK", 2) == 0) {
        gsm_info.phone_status = PHONE_ST_TALK;
    } 
    if (memcmp(res, "ERROR", 5) == 0) {
        gsm_info.phone_status = PHONE_ST_IDLE;
    }/*
    if (memcmp(res, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}*/
}
/*********************************************************************************************
* ���ƣ�void gsm_call_accept(void)
* ���ܣ�GSM�绰����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_call_accept(void)
{
    if (gsm_info.phone_status == PHONE_ST_RING) {
        _request_at_3("ata\r\n", 1000, _rs_gsm_call_accept);
    }
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_call_hangup(char *rsp)
* ���ܣ�GSM�绰�Ҷ�
* ������*rsp -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_call_hangup(char *rsp){
    gsm_info.phone_status = PHONE_ST_IDLE;
}

/*********************************************************************************************
* ���ƣ�void gsm_call_hangup(void)
* ���ܣ�GSM�绰�Ҷ�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_call_hangup(void)
{
    _request_at_3("ath\r\n", 1000, _rs_gsm_call_hangup);
}
void _rs_gsm_request_call(char *rsp)
{
    if (rsp == NULL) {
        printf("error : gsm must not timeout when run atd\r\n");
        return;
    }
    if (memcmp(rsp, "OK", 2) == 0) {
        gsm_info.phone_status = PHONE_ST_COUT;
    }
    if (memcmp(rsp, "ERROR", 5) == 0) {
        gsm_info.phone_status = PHONE_ST_IDLE;
    } /*
    if (memcmp(rsp, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}
    if (memcmp(rsp, "BUSY", 4) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}*/
}
/*********************************************************************************************
* ���ƣ�int gsm_request_call(char *pnum)
* ���ܣ�GSM�������
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int gsm_request_call(char *pnum)
{
    char buf[32];
    sprintf(buf, "atd%s;\r\n", pnum);
    if (gsm_info.phone_status == PHONE_ST_IDLE) {
        strcpy(gsm_info.phone_number, pnum);
        _request_at_3(buf, 2000, _rs_gsm_request_call);
        return 1;
    }
    else return 0;
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_get_current_call(char *rsp)
* ���ܣ�GSM�õ���ǰ�ĵ绰
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_get_current_call(char *rsp)
{
    if (memcmp(rsp, "OK", 2) == 0) {
    }
    if (memcmp(rsp, "+CLCC", 5) == 0) {
        //+CLCC: 1,0,0,0,0,"10010",129
        if (strlen(rsp)>12) {
            if (rsp[11] == '0') {
                gsm_info.phone_status = PHONE_ST_TALK;
            }
        }
    }
}
/*********************************************************************************************
* ���ƣ�void gsm_get_current_call(void)
* ���ܣ�GSM�õ���ǰ�ĵ绰
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_get_current_call(void)
{
    _request_at_3("at+clcc\r\n", 1000, _rs_gsm_get_current_call);
}
/*********************************************************************************************
* ���ƣ�void gsm_ppp_open(void)
* ���ܣ�GSM��Ե�ͨѶ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

void gsm_ppp_open(void)
{
    char *apn = "cmnet";
    char buf[32];
    if (memcmp(gsm_info.network, "46000", 5) == 0) {
        apn = "cmnet";
    }
    if (memcmp(gsm_info.network, "46001", 5) == 0) {
        apn = "3gnet";
    }
    sprintf(buf, "at+zpnum=\"%s\",\"\",\"\"\r\n", apn);
    _request_at_3(buf, 3000, NULL);
    _request_at_3("at+zpppopen\r\n", 3000, NULL);
    _request_at_3("at+zpppstatus\r\n", 3000, NULL);
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_get_imei(char *rsp)
* ���ܣ�GSM�յ��ֻ�����
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_get_imei(char *rsp)
{
    if (rsp == NULL) {
        printf("error: get imei not timeout\r\n");
        return;
    }
    int len = strlen(rsp);
    if (len == 15) {
        strcpy(gsm_info.imei, rsp);
    }
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_get_imei(char *rsp)
* ���ܣ�GSM�յ��ֻ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_get_imei(void)
{
    _request_at_3("at+gsn\r\n", 1000, _rs_gsm_get_imei);
}
/*********************************************************************************************
* ���ƣ�void _rs_gsm_send_message(char *rsp)
* ���ܣ�GSM���Ͷ���
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static char short_message[72];
void _rs_gsm_send_message(char *rsp)
{
    if (rsp == NULL) {
        printf("error: send message timeout\r\n");
        return;
    }
    if (rsp[0] == '>')  {
        mg2639_write(short_message, strlen(short_message));
        char ch = 0x1A;
        mg2639_write(&ch, 1);
        printf("\r\nGSM <<< %s\r\n", short_message);
    }
}
/*********************************************************************************************
* ���ƣ�void gsm_send_message(char *num, char *msg)
* ���ܣ�GSM���Ͷ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_send_message(char *num, char *msg)
{
    char buf[32];
    sprintf(buf, "at+cmgs=\"%s\"\r", num);
    sprintf(short_message, msg);
    _request_at_3(buf, 5000, _rs_gsm_send_message);
}

/*********************************************************************************************
* ���ƣ�void _rs_gsm_query_creg(char *rsp)
* ���ܣ�GSM��ѯ
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_gsm_query_creg(char *rsp)
{
    /* +CREG: 2, 1, "712C", "DF6A", 0 */
    if (rsp == NULL) return;
    if (memcmp(rsp, "+CREG:", 6) == 0) {
        if (rsp[7] != '2') {
            _request_at_3("at+creg=2\r\n", 1000, NULL);
            return;
        } else {
            if (strlen(rsp) > 14) {
                char *lac = &rsp[14];
                char *p = strchr(lac, '\"');
                *p = '\0';
                char *ci = p+4;
                p = strchr(ci, '\"');
                *p = '\0';
                char *act = p + 3;
                //printf(" got lac: %s, ci: %s, act:%s\r\n", lac, ci, act);
                strcpy(gsm_info.lac, lac);
                strcpy(gsm_info.ci, ci);
                strcpy(gsm_info.act, act);
            }
        }
    }
}
/*********************************************************************************************
* ���ƣ�void gsm_query_creg(void)
* ���ܣ�GSM��ѯ
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_query_creg(void)
{
    _request_at_3("at+creg?\r\n", 5000, _rs_gsm_query_creg);
}

/*********************************************************************************************
* ���ƣ�int gsm_tcp_recv_register(int id, void (*fun)(char *buf, int len))
* ���ܣ�GSM TCP���ռĴ���
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void (*tcp_recv_call[5])(char *buf, int len);
int gsm_tcp_recv_register(int id, void (*fun)(char *buf, int len))
{
    if (id >= 0 && id < 5) {
        tcp_recv_call[id] = fun;
        return 0;
    } 
    return -1;
}

/*********************************************************************************************
* ���ƣ�static void gsm_tcp_recv(int id, int len, char *buf)
* ���ܣ�GSM TCP����
* ������*rsp
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void gsm_tcp_recv(int id, int len, char *buf)
{
    if (id >= 0 && id < 5) {
        if (tcp_recv_call[id] != NULL) {
            tcp_recv_call[id](buf, len);
        }
    }
}
/*********************************************************************************************
* ���ƣ�gsm_tcp_write(char *buf, int len)
* ���ܣ�GSM TCPд��
* ������*buf --    len -- 
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_write(char *buf, int len)
{
    mg2639_write(buf, len);
}
/*********************************************************************************************
* ���ƣ�void gsm_tcp_close(int id)
* ���ܣ�GSM TCP�ر�
* ������id
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void gsm_tcp_close(int id)
{
}
static struct ctimer gsm_timer;

/*********************************************************************************************
* ���ƣ�static void _gsm_timer_call(void *ptr)
* ���ܣ�GSM ��ʱ������
* ������*ptr
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

static void _gsm_timer_call(void *ptr)
{
    static int tick = 0;
    tick ++;
    if (gsm_info.gsm_status == 0) {
        if (tick % 3 == 0) {
            check_gsm();
        }
    }
    if (gsm_info.gsm_status == 1) {
        static int init = 0;
        if (init == 0) {
            _request_at_3("ate0\r\n", 1000, NULL);
            _request_at_3("at+clip=1\r\n", 1000, NULL);
            _request_at_3("at+speaker=0\r\n", 1000, NULL);
            //_request_at_3("at+tsimins?\r\n", 1000, NULL);
            //_request_at_3("at+cops=0,2\r\n", 1000, NULL);
            _request_at_3("at+zcsq=3\r\n", 1000, NULL);
            gsm_get_imei();
            _request_at_3("at+creg=2\r\n", 1000, NULL);
            //_request_at_3("at+cced=1,3\r\n", 1000, NULL);
            
            _request_at_3("at+cmgf=1\r\n", 1000, NULL);
            init = 1;
        }
        if (init == 1 && tick % 5 == 0) {
            check_sim_card();
        }
    }
    if (gsm_info.gsm_status == 2) {
        
        if (tick % 5 == 1 && strlen(gsm_info.network) == 0) {
            gsm_register_network();
        }
        if (strlen(gsm_info.network) > 0) {
            if (tick % 5 == 2 && gsm_info.ppp_status == 0) {
                gsm_ppp_open();
            }
            /*if (gsm_info.ppp_status != 0 && tick % 60 == 30) {
            _request_at_3("at+zpppstatus\r\n", 1000, NULL);
        }*/
        }
        /*if (tick % 60 == 15) {
        gsm_query_creg();
    }*/
        if (gsm_info.phone_status == PHONE_ST_COUT) {
            gsm_get_current_call();
        }
    }
    if (gsm_info.gsm_status == 3) {
        //��sim��
        if (tick % 5 == 0) check_sim_card();
    }
    ctimer_reset(&gsm_timer);
}

/*********************************************************************************************
* ���ƣ�void _poll_request(void)
* ���ܣ���ѯ����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _poll_request(void)
{
    if (current_at == NULL) {
        current_at = list_at;
        if (current_at != NULL) {
            printf(" gsm <<< %s", (char*)current_at->req);
            if (strstr(current_at->req, "zipsend")) {
                printf("\n");
            }
            
            current_at->timeout_tm = clock_time()+ current_at->timeout_tm;
            mg2639_write(current_at->req, strlen(current_at->req));
            list_at = list_at->next;
        }
    } else {
        unsigned int tm = clock_time();
        if (((int)(tm - current_at->timeout_tm)) > 0) {
            printf(" gsm : timeout\r\n");
            if (current_at->fun != NULL) {
                current_at->fun(NULL);
            }
            end_process_at();
        } 
    }
}
/*********************************************************************************************
* ���ƣ�void _poll_response(void)
* ���ܣ���ѯ��Ӧ
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _poll_response(void)
{
    int ch;
    ch = mg2639_get_ch();
    while (ch >= 0) {
        if (gsm_response_mode == 0) {
            if (_response_offset == 0 && ch == '>') {
                if (current_at != NULL && current_at->fun != NULL) {    
                    current_at->fun(">");
                }
            } else if (_response_offset >= 9 && memcmp(_response_buf, "+ZIPRECV:", 9) == 0) {
                _response_offset = 0;
                /* ��ȡtcp id */
                do {
                    ch = mg2639_get_ch();
                    if (ch >= 0) {
                        _response_buf[_response_offset++] = ch;
                    }
                } while (ch != ',');
                _response_buf[_response_offset-1] = '\0';
                int id = atoi(_response_buf);
                _response_offset = 0;
                /* ��ȡ���ݳ��� */
                do {
                    ch = mg2639_get_ch();
                    if (ch >= 0) {
                        _response_buf[_response_offset++] = ch;
                    }
                } while (ch != ',');
                _response_buf[_response_offset-1] = '\0';
                int dlen = atoi(_response_buf);
                printf("gsm >>> +ZIPRECV: %d,%d\r\n", id, dlen);
                /* ��ȡ���� */
                do {       
                    _response_offset=0;
                    int min = dlen < _RESPONSE_BUF_SIZE ? dlen : _RESPONSE_BUF_SIZE;
                    do {
                        ch = mg2639_get_ch();
                        if (ch >= 0) {
                            _response_buf[_response_offset++] = ch;
                        }
                    } while (_response_offset < min);
                    //_response_buf[_response_offset] = 0;
                    gsm_tcp_recv(id, _response_offset, _response_buf);
                    dlen -= _response_offset;
                } while (dlen > 0);
                _response_offset = 0;
            } else
                if (ch != '\n') {
                    _response_buf[_response_offset++] = ch;
                } else {
                    if (_response_offset>0 && _response_buf[_response_offset-1] == '\r') {
                        _response_buf[_response_offset-1] = '\0';
                        _response_offset -= 1;
                        if (_response_offset > 0) {
                            printf(" gsm >>> %s\r\n", _response_buf);
                            if (current_at != NULL && current_at->fun != NULL) {
                                current_at->fun(_response_buf);
                            }
                            if ((memcmp(_response_buf, "OK", 2) == 0) 
                                ||(memcmp(_response_buf, "ERROR", 5) == 0))
                            {
                                end_process_at();
                            }
                            /* unsolite message */
                            if (memcmp(_response_buf, "+CSQ:", 5) == 0) {
                                char *p = &_response_buf[6];
                                char *e = strchr(p, ',');
                                *e = '\0';
                                int v = atoi(p);
                                if (v == 99) v = 0;
                                gsm_info.signal = v/6;
                            }
                            if (memcmp(_response_buf, "+CLIP: ", 7) == 0) {
                                //+CLIP: "13135308483",161,"","",0
                                gsm_info.phone_status = PHONE_ST_RING;
                                char *p = &_response_buf[8];
                                char *e = strchr(p, '\"');
                                *e = '\0';
                                strcpy(gsm_info.phone_number, p);
                            }
                            if (memcmp(_response_buf, "NO CARRIER", 10) == 0) {
                                gsm_info.phone_status = PHONE_ST_IDLE;
                            }
                            if (memcmp(_response_buf, "BUSY", 4) == 0) {
                                gsm_info.phone_status = PHONE_ST_IDLE;
                            }
                            if (memcmp(_response_buf, "+ZPPPOPEN:CONNECTED", 19) == 0) {
                                gsm_info.ppp_status = 1;
                            }
                            if (memcmp(_response_buf, "+ZPPPOPEN:ESTABLISHED", 21) == 0) {
                                gsm_info.ppp_status = 2;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:ESTABLISHED", 23) == 0) {
                                gsm_info.ppp_status = 2;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:CONNECTED", 21) == 0) {
                                gsm_info.ppp_status = 1;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:DISCONNECTED", 24) == 0) {
                                gsm_info.ppp_status = 0;
                            }
                            //+ZIPCLOSE: 0
                            if (memcmp(_response_buf, "+ZIPCLOSE:", 10) == 0) {
                                char *p = &_response_buf[11];
                                int tid = atoi(p);
                                if (tid >= 0 && tid < 5) {
                                    gsm_tcp_close(tid);
                                }
                            }
                            
                            /*if (memcmp(_response_buf, "+CPIN: READY", 12)==0) {
                            gsm_info.gsm_status = 2;
                            gsm_info.simcard_status = 1;
                        }*/    
                            _response_offset = 0;
                        }
                    }
                }
        }
        
        /**/
        ch = mg2639_get_ch();
    }
}
//////////////////////////////////////////////////////
struct _at_t{
    struct pt _pt;
    char status;
    struct process *cp;
}_at;
process_event_t event_at_finish;
/*********************************************************************************************
* ���ƣ�void _rs_at_test(char *rsp)
* ���ܣ�����
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void _rs_at_test(char *rsp)
{
    if (rsp == NULL) {
        printf("timeout..\r\n");
        
        _at.status = 0;
    } else {
        
        _at.status = 0;
    }
    //process_post(_at.cp, PROCESS_EVENT_NONE, (void *)1);
}
/*********************************************************************************************
* ���ƣ�PT_THREAD(_at_thread(process_event_t ev, void *data))
* ���ܣ�PT������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PT_THREAD(_at_thread(process_event_t ev, void *data))
{
    
    PT_BEGIN(&_at._pt);
    
    //_at.cp = PROCESS_CURRENT();
    _at.status = 1;
    printf("at start...\r\n");
    _request_at_3("at\r\n", 3000, _rs_at_test);
    PT_WAIT_UNTIL(&_at._pt, _at.status == 0);
    printf("at finish....\r\n");
    
    PT_END(&_at._pt);
}
/*********************************************************************************************
* ���ƣ�voidat_test(void)
* ���ܣ�����
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void at_test(void)
{
    PT_INIT(&_at._pt);
    _at_thread(PROCESS_EVENT_NONE,NULL);
}


#include "shell.h"
PROCESS(shell_gsm_process, "gsm shell process");
SHELL_COMMAND(gsm_command, "gsm", "gsm: gsm operator",
              &shell_gsm_process);
/*********************************************************************************************
* ���ƣ�PROCESS_THREAD(shell_gsm_process, ev, data)
* ���ܣ�������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PROCESS_THREAD(shell_gsm_process, ev, data)
{
    
    PROCESS_BEGIN();
    
    //printf("gsm data %s\r\n", data);
    char buf[64];
    if (strlen(data) > 0) {
        sprintf(buf, "%u\r\n", data);
        _request_at_3(buf, 1000, NULL);
    } else 
        gsm_send_message("13135308483", "abc");
    PROCESS_EXIT();
    
    PROCESS_END();
}
/*********************************************************************************************
* ���ƣ�PROCESS_THREAD(gsm, ev, data)
* ���ܣ�GSM������
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
PROCESS_THREAD(gsm, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();                                              //��ʼ����
    if(connect_mode == 1)
    {
        mg2639_init();                                                //gsmģ���ʼ��
        event_at_finish = process_alloc_event();
        
        etimer_set(&et, CLOCK_SECOND/10); // 100 ms
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);           //�ȴ��¼�����
        
        //mytimer_set(&gsm_timer, _gsm_timer_call, NULL);
        //mytimer_add(&gsm_timer, 1000);
        ctimer_set(&gsm_timer, 1000, _gsm_timer_call, NULL);          //���ö�ʱ��
        shell_register_command(&gsm_command);                         //��¼ָ��
        while (1) 
        {
            _poll_request();                                            //��ѯ���� 
            _poll_response();                                           //��ѯ��Ӧ
            etimer_set(&et, CLOCK_SECOND/100);                          //��ʱ������
            PROCESS_YIELD();                                            //�л�����
        }
    }
    PROCESS_END();                                                //��������
}