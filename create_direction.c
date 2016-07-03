#include "pos.h"

/*
 * 商品補充指示書作成
 *  [ARGUMENT]
 *    __conn     : PGconnオブジェクト
 *    __tax       : tax
 *    __sendBuf  : 送信用配列のポインタ
 *  [RETURN]
 *    Success : 0
 *    Error   : -1
 */

int new_item_func(PGconn *__con,char *item_name,char *t_or_f, char *__sendBuf){
  char        sql[BUFSIZE];
  char sql0[BUFSIZE];
  PGresult    *res;   //PGresultオブジェクト
  PGresult    *res0;   //PGresultオブジェクト
  double tax_double;
  int         resultRows;
  int max;

  char item_code[BUFSIZE];
  char available_period[BUFSIZE];
  char inventory_num[BUFSIZE];

  /* 新商品追加SQLを作成 */
  sprintf(sql, "INSERT INTO item VALUES(%s, '%s', %s)","nextval ('item_code_seq')", item_name, t_or_f);
  /* SQLコマンド実行 */
  res = PQexec(__con, sql);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res));
	sprintf(__sendBuf, "%s %s%s", "ER_STAT", "E_CODE", "\n");
	return -1;
  }
  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res)) == 1){
	sprintf(__sendBuf, "%s%s", "新商品を変更しました", ENTER);
  }
  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  return 0;
}//END

int create_new_direction_func(PGconn *__con,char *store_num,char *item_code,char *available_period, char *inventory_num, char *__sendBuf){
  char sql[BUFSIZE];
  char sql0[BUFSIZE];
  PGresult    *res;   //PGresultオブジェクト
  PGresult    *res0;   //PGresultオブジェクト
  PGresult    *res1;   //PGresultオブジェクト
  PGresult    *res2;   //PGresultオブジェクト

  double tax_double;
  int         resultRows;
  int max;

  int store_num_int;
  int item_code_int;
  int available_period_int;
  int inventory_num_int;
  char purchase_code_seq[BUFSIZE];
  int nextval;

  store_num_int = atoi(store_num);
  item_code_int = atoi(item_code);
  available_period_int = atoi(available_period);
  inventory_num_int = atoi(inventory_num);


  /* 在庫管理追加SQLを作成 */

  sprintf(sql0, "select * from nextval ('item_management_%d_purchase_code_seq')"
		  , store_num_int
		  );
  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql0);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	printf("%s", PQresultErrorMessage(res0));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	return -1;
  }

  nextval = atoi(PQgetvalue(res0,0,0));

  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res0)) == 1){
	sprintf(__sendBuf, "%s%s", "ok", ENTER);
  }


  sprintf(sql, "INSERT INTO item_management_%d VALUES(%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s)"
		  , store_num_int
		  , nextval
		  , item_code_int
		  , available_period_int
		  , inventory_num_int
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  );
  /* SQLコマンド実行 */
  res = PQexec(__con, sql);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT", "E_CODE", sql,"\n");
	return -1;
  }
  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res)) == 1){
	sprintf(__sendBuf, "%s%s", "配送しました", ENTER);
  }






  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  return 0;
}//END



///////////////////////
int create_direction_func(PGconn *__con,char *item_code,char *available_period, char *inventory_num, char *__sendBuf){
  char sql[BUFSIZE];
  char sql0[BUFSIZE];
  char sql1[BUFSIZE];
  char sql2[BUFSIZE];
  PGresult    *res;   //PGresultオブジェクト
  PGresult    *res0;   //PGresultオブジェクト
  PGresult    *res1;   //PGresultオブジェクト
  PGresult    *res2;   //PGresultオブジェクト

  double tax_double;
  int         resultRows;
  int max;

  int ship_store_num_int;
  int item_code_int;
  int order_day_int;
  int available_period_int;
  int inventory_num_int;
  char purchase_code_seq[BUFSIZE];
  int nextval;
  int max_row;
  int store_num_int;

  int i,j;

  //store_num_int = atoi(store_num);
  //item_code_int = atoi(item_code);
  //available_period_int = atoi(available_period);
  //inventory_num_int = atoi(inventory_num);




  /* 在庫管理追加SQLを作成 */

  sprintf(sql0, "select * from nextval ('item_management_%d_purchase_code_seq')"
		  , store_num_int
		  );
  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql0);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	printf("%s", PQresultErrorMessage(res0));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	return -1;
  }

  nextval = atoi(PQgetvalue(res0,0,0));

  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res0)) == 1){
	sprintf(__sendBuf, "%s%s", "ok", ENTER);
  }


  sprintf(sql, "INSERT INTO item_management_%d VALUES(%d,%d,%d,%d,%s,%s,%s,%s,%s,%s,%s,%s)"
		  , store_num_int
		  , nextval
		  , item_code_int
		  , available_period_int
		  , inventory_num_int
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  , "null"
		  );
  /* SQLコマンド実行 */
  res = PQexec(__con, sql);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT", "E_CODE", sql,"\n");
	return -1;
  }
  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res)) == 1){
	sprintf(__sendBuf, "%s%s", "配送しました", ENTER);
  }






  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  return 0;
}//END
