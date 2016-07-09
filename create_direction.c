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



int create_direction_recv_func(ThreadParameter *threadParam, char *sendBuf){
  char recvBuf[BUFSIZE];
  int  recvLen, sendLen;
  char recvBuf1[BUFSIZE], sendBuf1[BUFSIZE];
  int  recvLen1, sendLen1;
  char comm[BUFSIZE], comm2[BUFSIZE];   //リクエストコマンド
  float perm1Int, perm2Int, perm3Int;
  pthread_t selfId;                      //自分自身のスレッドID
  int cnt;
  int i,j;

  char item_name[BUFSIZE];
  char t_or_f[BUFSIZE];

  char store_num[BUFSIZE];
  char item_code[BUFSIZE];
  char available_period[BUFSIZE];
  char inventory_num[BUFSIZE];

  /*create_store*/
  char place_num[BUFSIZE];
  char store_name[BUFSIZE];
  char password[BUFSIZE];

  char sql[BUFSIZE];
  char sql0[BUFSIZE];
  char sql1[BUFSIZE];
  char sql2[BUFSIZE];
  PGresult    *res;   //PGresultオブジェクト
  PGresult    *res0;   //PGresultオブジェクト
  PGresult    *res1;   //PGresultオブジェクト
  PGresult    *res2;   //PGresultオブジェクト

  int ship_store_num_int;
  int item_code_int;
  int order_day_int;
  int available_period_int;
  int inventory_num_int;
  int order_code_int;
  char purchase_code_seq[BUFSIZE];
  int nextval;
  int max_row;
  int max_row_1;

  char purchase_unit_price[BUFSIZE];
  char  purchase_num[BUFSIZE];
  int purchase_num_int;

  char purchase_num_ch[BUFSIZE];
  char order_system[BUFSIZE];

  int count = 0;

  /* 商品補充指示書完成SQLを作成 */
  sprintf(sql0, "SELECT * FROM item_sup_direction\
                     WHERE order_code = (SELECT MIN(order_code)\
                     FROM item_sup_direction \
                     WHERE purchase_confirm = 'false')");

  /* SQLコマンド実行 */
  res0 = PQexec(threadParam->con, sql0);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	printf("%s", PQresultErrorMessage(res0));
	sprintf(sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	return -1;
  }

  max_row = PQntuples(res0);
  /* UPDATEされた行数を取得 */
  if(max_row != 1){
	sprintf(sendBuf, "%s%s.\n", "新しい発注はありません。", ENTER);
	sendLen = strlen(sendBuf);
	send(threadParam->soc, sendBuf, sendLen, 0);
	printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf);
	return 0;
  }


  PQclear(res0);

  ///resend送り先店舗名・商品番号表示
  count += sprintf(sendBuf, "以下のような発注依頼が来ています。\n");

  /*発注依頼一覧表示SQL*/
  //sprintf(sql1, "SELECT * FROM item_sup_direction"); //test: a lot of data


  sprintf(sql1, "SELECT * FROM item_sup_direction\
	WHERE purchase_confirm = 'false'");

  /* SQLコマンド実行 */
  res1 = PQexec(threadParam->con, sql1);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res1) != PGRES_TUPLES_OK){
	printf("%s", PQresultErrorMessage(res1));
	sprintf(sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql1,"\n");
	return -1;
  }

  max_row_1 = PQntuples(res1);
  printf("%d\n", max_row_1);

  /*発注依頼一覧表示*/
  for(i=0; i<max_row_1; i++){ /////////////////

	sprintf(sql0, "SELECT * FROM item_sup_direction\
                     WHERE order_code = (SELECT MIN(order_code)\
                     FROM item_sup_direction \
                     WHERE purchase_confirm = 'false')");

	/* SQLコマンド実行 */
	res0 = PQexec(threadParam->con, sql0);
	/* SQLコマンド実行結果状態を確認 */
	if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	  printf("%s", PQresultErrorMessage(res0));
	  sprintf(sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	  return -1;
	}

	order_code_int = atoi(PQgetvalue(res1,i,0));  //発注番号取得
	ship_store_num_int = atoi(PQgetvalue(res1,i,1));  //店舗番号取得
	item_code_int = atoi(PQgetvalue(res1,i,2));  //商品コード取得
	strcpy(purchase_num_ch, PQgetvalue(res1,i,5));  //入荷数取得
    strcpy(order_system, PQgetvalue(res1,i,8));  //発注方式取得

	purchase_num_int = atoi(purchase_num_ch);

	///送り先店舗名・商品番号表示
	if(strcmp(order_system,"t") == 0){
	  count += sprintf(sendBuf+count,"送り先店舗番号：%d 商品番号：%d\n", ship_store_num_int, item_code_int );
	}else{
	  count += sprintf(sendBuf+count,"送り先店舗番号：%d 商品番号：%d 入荷数：%d\n"
					   , ship_store_num_int
					   , item_code_int
					   , purchase_num_int);
	}

  }

  count += sprintf(sendBuf+count, "-------------------------------------------\n");

  ///resend送り先店舗名・商品番号表示
  count += sprintf(sendBuf+count, "以下の内容を入力してください。\n");

  PQclear(res0);

  /*発注依頼表示*/
  for(i=0; i<max_row_1; i++){

	sprintf(sql0, "SELECT * FROM item_sup_direction\
                     WHERE order_code = (SELECT MIN(order_code)\
                     FROM item_sup_direction \
                     WHERE purchase_confirm = 'false')");


	/* SQLコマンド実行 */
	res0 = PQexec(threadParam->con, sql0);
	/* SQLコマンド実行結果状態を確認 */
	if( PQresultStatus(res0) != PGRES_TUPLES_OK){
	  printf("%s", PQresultErrorMessage(res0));
	  sprintf(sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	  return -1;
	}


	order_code_int = atoi(PQgetvalue(res0,0,0));  //発注番号取得
	ship_store_num_int = atoi(PQgetvalue(res0,0,1));  //店舗番号取得
	item_code_int = atoi(PQgetvalue(res0,0,2));  //商品コード取得
	strcpy(purchase_num, PQgetvalue(res0,i,5));  //入荷数取得 //ch
    strcpy(order_system, PQgetvalue(res0,i,8));  //発注方式取得

	PQclear(res0);

	purchase_num_int = atoi(purchase_num_ch);

	///resend送り先店舗名・商品番号表示
	//count += sprintf(sendBuf+count, "%s送り先店舗番号：%d 商品番号：%d\n.\n", sendBuf1,ship_store_num_int, item_code_int );


	if(strcmp(order_system,"t") == 0){
	  count += sprintf(sendBuf+count,"%s送り先店舗番号：%d 商品番号：%d\n.\n"
					   ,sendBuf1
					   ,ship_store_num_int
					   ,item_code_int );
	}else{
	  count += sprintf(sendBuf+count,"%s送り先店舗番号：%d 商品番号：%d 入荷数：%d\n.\n"
					   ,sendBuf1
					   , ship_store_num_int
					   , item_code_int
					   , purchase_num_int);

	}



	sendLen = strlen(sendBuf);
	send(threadParam->soc, sendBuf, sendLen, 0);
	printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf);

	count = 0;
	//while(getchar() != '\n');
	printf("tes1%s", recvBuf);

	/* リクエストコマンド受信 */
	recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
	if( recvLen < 1 )
	  break;
	recvBuf[recvLen-1] = '\0'; // <LF>を消去
	printf("[C_THREAD %u] RECV=> %s\n", selfId, recvBuf);

	/*登録*/
	if( (sscanf(recvBuf,"%s %s %s", purchase_unit_price, purchase_num, available_period) == 3) && (strcmp(order_system,"t") == 0)){
	  create_direction_func(threadParam->con
							,order_code_int
							,ship_store_num_int
							,item_code_int
							,purchase_unit_price
							,purchase_num
							,available_period
							,sendBuf);
	}else if((sscanf(recvBuf,"%s %s", purchase_unit_price, available_period) == 2) && (strcmp(order_system,"f") == 0)){
	  create_direction_func(threadParam->con
							,order_code_int
							,ship_store_num_int
							,item_code_int
							,purchase_unit_price
							,purchase_num_ch  //ch
							,available_period
							,sendBuf);

	}else if(strcmp(recvBuf,"\n")){
	  ;
	}else{
	  sprintf(sendBuf, "%stes\n.\n","ER_STAT"); //temp.
	  printf("tes1%s", recvBuf);
	  return 0;
	}

  }



  return 0;
}



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
	sprintf(__sendBuf, "%s %s%s test:%s.\n", "ER_STAT", "E_CODE", sql,"\n");
	return -1;
  }
  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res)) == 1){
	sprintf(__sendBuf, "%s%s.\n", "配送しました", ENTER);
  }


  /* PGresultに割当られた記憶領域を開放 */
  PQclear(res);
  return 0;
}//END


///////////////////////
int create_direction_func(PGconn *__con, int order_code_int, int ship_store_num_int, int item_code_int, char *purchase_unit_price, char *purchase_num, char *available_period, char *__sendBuf){
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

  int order_day_int;
  int available_period_int;
  int inventory_num_int;
  char purchase_code_seq[BUFSIZE];
  int nextval;
  int max_row;
  int max_row1;
  int store_num_int;
  int purchase_unit_price_int;
  int purchase_num_int;

  int i,j;


  purchase_unit_price_int = atoi(purchase_unit_price);
  purchase_num_int = atoi(purchase_num);
  available_period_int = atoi(available_period);

  //store_num_int = atoi(store_num);
  //item_code_int = atoi(item_code);
  //available_period_int = atoi(available_period);
  //inventory_num_int = atoi(inventory_num);

  /*商品補充指示書追加*/
  sprintf(sql1, "UPDATE item_sup_direction\
                 SET purchase_unit_price = %d,\
                     purchase_num = %d,\
                     available_period = %d,\
                     purchase_confirm = 'true'\
                 WHERE order_code = %d\
                 AND purchase_confirm = 'false'"
		  ,purchase_unit_price_int
		  ,purchase_num_int
		  ,available_period_int
		  ,order_code_int
		  );
  /* SQLコマンド実行 */
  res1 = PQexec(__con, sql1);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res1) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res1));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql1,"\n");
	return -1;
  }


  /* 在庫管理追加SQLを作成 */

  sprintf(sql0, "insert into item_management_%d values (\
                 nextval ('item_management_%d_purchase_code_seq'),\
                 %d,\
                 %d,\
                 %d,\
                 NULL,\
                 NULL,\
                 NULL,\
                 NULL,\
                 NULL,\
                 'false',\
                 NULL,\
                 NULL)"
		  ,ship_store_num_int
		  ,ship_store_num_int
		  ,item_code_int
		  ,available_period_int
		  ,purchase_num_int  //
		  );

  /* SQLコマンド実行 */
  res0 = PQexec(__con, sql0);
  /* SQLコマンド実行結果状態を確認 */
  if( PQresultStatus(res0) != PGRES_COMMAND_OK){
	printf("%s", PQresultErrorMessage(res0));
	sprintf(__sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
	return -1;
  }

  //nextval = atoi(PQgetvalue(res0,0,0));

  /* UPDATEされた行数を取得 */
  if(atoi(PQcmdTuples(res0)) == 1){
	sprintf(__sendBuf, "%s%s.\n", "ok", ENTER);
  }


  /* PGresultに割当られた記憶領域を開放 */
  //PQclear(res);
  PQclear(res0);
  PQclear(res1);

  return 0;
}//END
