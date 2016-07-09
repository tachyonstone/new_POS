#include "pos.h"

/*
 * 新店舗登録
 *  [ARGUMENT]
 *    __conn     : PGconnオブジェクト
 *    __sendBuf  : 送信用配列のポインタ
 *  [RETURN]
 *    Success : 0
 *    Error   : -1
 */

int create_store_func(PGconn *__con,char *place_num,char *store_name,char *password,char *__sendBuf){
  char sql[BUFSIZE];
  char sql0[BUFSIZE];
  char sql1[BUFSIZE];
  char sql2[BUFSIZE];
  PGresult *res;  //PGresultオブジェクト
  PGresult *res0;  //PGresultオブジェクト
  int resultRows;
  int max;

  int max_row;
  int store_num;

  /* 新店舗追加SQLを作成 */
  sprintf(sql, "INSERT INTO store VALUES(%s, %s, '%s', '%s')","nextval ('item_code_seq')", place_num, store_name, password);
  /* SQLコマンド実行 */
  res = PQexec(__con, sql);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res));
	sprintf(__sendBuf, "%s %s%s.\n", "ER_STAT", "E_CODE", "\n");
	return -1;
  }


  sprintf(sql0, "select *  from store");
  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql0);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	printf("%s", PQresultErrorMessage(res0));
	sprintf(__sendBuf, "%s %s%s.\n", "ER_STAT2", "E_CODE", "\n");
	return -1;
  }


  /*CREATE TABLE item_management*/
  max_row = PQntuples(res0);

  printf("t:%d\n", max_row); //test
  store_num   = atoi(PQgetvalue(res0, max_row-1, 0));


  /* 新店舗追加SQLを作成 */
  sprintf(sql0, "CREATE TABLE item_management_%d (\
                  purchase_code	  serial,\
                  item_code	      serial,\
                  available_period   bigint,\
                  inventory_num      integer,\
                  sale_unit_price    integer,\
                  profit	      integer,\
                  procurement_period bigint,\
                  order_system         bool,\
                  purchase_day       bigint,\
                  order_confirm      bool,\
                  order_interval 	  integer,\
                  safe_stock		  integer,\
                  PRIMARY KEY(purchase_code))"
		  , store_num);

  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql0);

  sprintf(__sendBuf, "%s%s.\n", "テーブルを登録しました", ENTER);



  /* 新店舗追加SQLを作成 */
  sprintf(sql1,  "CREATE TABLE tax_%d(\
   tax_rate        real,\
   mutual_tax_rate real)"
		  ,store_num);


  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql1);

  sprintf(__sendBuf, "%s%s.\n", "テーブルを登録しました", ENTER);


  /* 新店舗追加SQLを作成 */
  sprintf(sql2, "CREATE TABLE mod_info_%d (\
                  slip_num	             serial,\
                  before_tax_rate	     real,\
                  before_multiple_tax_rate  real,\
                  before_point_rate         real,\
                  PRIMARY KEY(slip_num))"
		  , store_num);

  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql2);

  sprintf(__sendBuf, "%s%s.\n", "テーブルを登録しました", ENTER);



  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  PQclear(res0);
  return 0;
}//END
