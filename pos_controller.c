
#include "pos.h"

/**
 * ATMメイン処理 (スレッド関数)
 *   [ARGUMENT]
 *      __arg  : スレッド引数構造体(ThreadParameter型)
 *  [RETURN]
 *     NONE
 **/

void *pos_controller(void *__arg){
  ThreadParameter *threadParam = (ThreadParameter *)__arg; //スレッド引数構造体
  char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
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

  int count = 0;



  selfId = pthread_self(); //自分自身のスレッドIDを取得
  printf("[C_THREAD %u] POS CONTROLLER START (%d)\n", selfId, threadParam->soc);

  while(1){
    /* リクエストコマンド受信 */
    recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
    if( recvLen < 1 )
      break;
    recvBuf[recvLen-1] = '\0'; // <LF>を消去
    printf("[C_THREAD %u] RECV=> %s\n", selfId, recvBuf);
    /* リクエストコマンド解析 */
    cnt = sscanf(recvBuf, "%s", comm);
	printf("%s\n",comm);  //test

    /* コマンド判定 */
    if(strcmp( comm, "NEW_ITEM" ) == 0){  //new_item
      /*新商品登録*/
      if( sscanf(recvBuf,"%s %s %s", comm2, item_name, t_or_f) == 3){
		new_item_func(threadParam->con, item_name, t_or_f, sendBuf);
      }else{
		sprintf(sendBuf, "%s\n","ER_STAT"); //temp.
	  }
	}else if(strcmp( comm, "CREATE_NEW_DIRECTION" ) == 0){  //create_new_direction
      /*新商品登録*/
      if( sscanf(recvBuf,"%s %s %s %s %s", comm2, store_num, item_code, available_period,inventory_num) == 5){
		create_new_direction_func(threadParam->con, store_num, item_code, available_period, inventory_num, sendBuf);
      }else{
		sprintf(sendBuf, "%s\n","ER_STAT"); //temp.
	  }
	}else if(strcmp( comm, "CREATE_DIRECTION" ) == 0){  //create_direction

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
		sprintf(sendBuf, "%s%s", "新しい発注はありません。", ENTER);
		sendLen = strlen(sendBuf);
		send(threadParam->soc, sendBuf, sendLen, 0);
		printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf);
		break;
	  }

	  ///resend送り先店舗名・商品番号表示
	  count += sprintf(sendBuf, "以下のような発注依頼が来ています。\n");

	  /*発注依頼一覧表示SQL*/
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

		///送り先店舗名・商品番号表示
		count += sprintf(sendBuf+count, "送り先店舗番号：%d 商品番号：%d\n", ship_store_num_int, item_code_int );
	  }

	  count += sprintf(sendBuf+count, "-------------------------------------------\n");

	  ///resend送り先店舗名・商品番号表示
	  count += sprintf(sendBuf+count, "以下の内容を入力してください。\n");


	  /*発注依頼一覧表示*/
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

		///resend送り先店舗名・商品番号表示
		count += sprintf(sendBuf+count, "%s送り先店舗番号：%d 商品番号：%d\n", sendBuf1,ship_store_num_int, item_code_int );
		sendLen = strlen(sendBuf);
		send(threadParam->soc, sendBuf, sendLen, 0);
		printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf1);

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
		if( sscanf(recvBuf,"%s %s %s", purchase_unit_price, purchase_num, available_period) == 3){
		  create_direction_func(threadParam->con, order_code_int, ship_store_num_int, item_code_int, purchase_unit_price, purchase_num, available_period, sendBuf);

		}else{
		  sprintf(sendBuf, "%stes\n","ER_STAT"); //temp.
		  printf("tes1%s", recvBuf);
		}


	  }


	}else if(strcmp( comm, "CREATE_STORE" ) == 0){  //create_store
	  /*新店舗登録*/
	  if( sscanf(recvBuf,"%s %s %s %s", comm2, place_num, store_name, password) == 4){
		create_store_func(threadParam->con, place_num, store_name, password, sendBuf);
	  }else{
		sprintf(sendBuf, "%s\n","ER_STAT"); //temp.
	  }
	}


	sendLen = strlen(sendBuf);
	send(threadParam->soc, sendBuf, sendLen, 0);
	printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf);
  }//END while()

  printf("[C_THREAD %u] POS CONTROLLER END (%d)\n\n", selfId, threadParam->soc);

  /* データベース接続を切断 */
  PQfinish(threadParam->con);
  /* ソケットを切断 */
  close(threadParam->soc);
  /* スレッド引数構造の領域解放 */
  free(threadParam);
}//END
