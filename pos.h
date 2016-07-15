/**
 * pos.h
 **/

#ifndef _INCLUDE_POS_
#define _INCLUDE_POS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <postgresql/libpq-fe.h>   //libpqライブラリを使用するために必要

#define BUFSIZE 1024
#define PORT 10000     //POSサーバのポート番号
#define ENTER "\n" //<LF>

//*** プロトコルコマンド ***//


//*** レスポンスステータス ***//
#define OK_STAT    "+OK"     //成功
#define ER_STAT    "-ERR"    //失敗

//*** エラーコード ***//


//*** スレッド関数に渡す引数情報構造体 ***//
typedef struct _ThreadParamter {
  struct in_addr c_ipaddr;  //IPアドレス
  in_port_t      c_port;    //ポート番号
  int            soc;       //コネクション用ソケットディスクリプタ
  PGconn         *con;      //PGconnオブジェクト（データベース）
}ThreadParameter;

extern int receive_message(int __s, char *__buf, int __maxlen);
extern int setup_listen(u_short __port);
extern void *pos_controller(void *arg);
extern int new_item_func(PGconn *__con, char *item_code,char *t_or_f, char *sendBuf);
extern int create_direction_func(PGconn *__con, int order_code_int, int ship_store_num_int, int item_code_int, int order_day_int, char *purchase_unit_price, char *purchase_num, char *available_period, char *__sendBuf);
extern void *atm_service(void *__arg);
extern int create_store_func(PGconn *__con,char *place_num,char *store_name,char *password,char *__sendBuf);
extern int create_direction_recv_func(ThreadParameter *threadParam, char *sendBuf);
#endif
