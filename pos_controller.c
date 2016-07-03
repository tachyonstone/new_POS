
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
  char purchase_code_seq[BUFSIZE];
  int nextval;
  int max_row;


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

	  sprintf(sql0, " select ship_store_num,\
                         item_code,\
                         order_day,\
                         order_system\
                         from item_sup_direction\
                         where purchase_confirm = 'false'"
			  );


	  /* SQLコマンド実行 */
	  res0 = PQexec(threadParam->con, sql0);
	  /* SQLコマンド実行結果状態を確認 */
	  if( PQresultStatus(res0) != PGRES_TUPLES_OK){
		printf("%s", PQresultErrorMessage(res0));
		sprintf(sendBuf, "%s %s%s test:%s", "ER_STAT2", "E_CODE", sql0,"\n");
		return -1;
	  }

	  max_row = PQntuples(res0);

	  for(i=0; i<max_row; i++){ /////////////////

		ship_store_num_int = atoi(PQgetvalue(res0,i,0));
		item_code_int = atoi(PQgetvalue(res0,i,1));


		sprintf(sendBuf, "%d %d\n", ship_store_num_int,	item_code_int );
		sendLen = strlen(sendBuf);
		send(threadParam->soc, sendBuf, sendLen, 0);
		printf("[C_THREAD %u] SEND=> %s\n", selfId, sendBuf);

		//	PQclear(res0);

	  }

	  /* リクエストコマンド受信 */
	  recvLen = receive_message(threadParam->soc, recvBuf, BUFSIZE);
	  if( recvLen < 1 )
		break;
	  recvBuf[recvLen-1] = '\0'; // <LF>を消去
	  printf("[C_THREAD %u] RECV=> %s\n", selfId, recvBuf);
	  /* リクエストコマンド解析 */
	  cnt = sscanf(recvBuf, "%s", comm);
	  printf("%s\n",comm);



	  /* UPDATEされた行数を取得 */
	  /*if(atoi(PQcmdTuples(res0)) == 1){
		sprintf(__sendBuf, "%s%s", "ok", ENTER);
		}*/




	  /*登録*/
	  if( sscanf(recvBuf,"%s %s %s %s %s", comm2, store_num, item_code, available_period,inventory_num) == 5){
		create_direction_func(threadParam->con, store_num, item_code, available_period, inventory_num, sendBuf);
	  }else{
		sprintf(sendBuf, "%s\n","ER_STAT"); //temp.
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
