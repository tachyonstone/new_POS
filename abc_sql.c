#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>
#include <stdbool.h>
#include <time.h>

#define BUFSIZE 1024
#define DAY 5

long int add_data_time(long int data_time, int add_day);

typedef struct {
  long int slip_num;
  int store_num;
  int weather;
  double temprature;
  long int data_time;
  char day[DAY];
  int age;
  bool gender;
} sales_info;

long int calc_data_time(long int data_time, int add_day){
  char char_data_time[8];
  int year, mon, day;
  char tmp[BUFSIZE];
  char c_year[4], c_mon[2], c_day[2];
  char *ends;
  time_t  s_t1;
  struct tm t1,*t2;
  
  sprintf(char_data_time, "%ld", data_time);
  year = atoi(strncpy(c_year, char_data_time, 4));
  mon  = atoi(strncpy(c_mon, char_data_time + 4, 2));
  day  = atoi(strncpy(c_day, char_data_time + 6, 2));
  
  t1.tm_sec  = 0;
  t1.tm_min  = 0;
  t1.tm_hour = 0;
  t1.tm_mday = day;
  t1.tm_mon  = mon - 1;
  t1.tm_year = year - 1900;
  
  s_t1 = mktime(&t1);
  s_t1 = s_t1 + 3600 * 24 * add_day;

  t2 = localtime(&s_t1);
  sprintf(tmp, "%d%02d%02d", t2->tm_year + 1900, t2->tm_mon + 1 , t2->tm_mday);

  return strtol(tmp, &ends, 0);
}

void clear_stdinbuf(){
  int c;
  while ((c = getchar()) != '\n')
    ;
  return;
}

void create_sql(char* sql1, char* sql2){
  char input;
  double tmp_double;
  int tmp_int;
  double small_temperature, big_temperature;
  int small_age, big_age;
  //long int before_period1, after_period1, before_period2, after_period2;
  char before_period1[10], after_period1[10], before_period2[10], after_period2[10];
  int store_num_flag = 0, weather_flag = 0, temperature_flag = 0, day_flag = 0, gender_flag = 0, age_flag = 0;
  char char_period[BUFSIZE] ,char_store_num[BUFSIZE], char_weather[BUFSIZE], char_temperature[BUFSIZE], char_day[BUFSIZE], char_gender, char_age[BUFSIZE];

  printf("比較期間を入力 20150101-20150102,20150103-20150104");
  fgets(char_period, BUFSIZE, stdin);
  clear_stdinbuf();
  sscanf(char_period, "%s-%s,%s-%s", before_period1, after_period1, before_period2, after_period2);

  printf("店番号を指定しますか 0 or 1\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    store_num_flag = 1;
    printf("店番号を入力\n");
    fgets(char_store_num, BUFSIZE, stdin);
    clear_stdinbuf();
  }
  
  printf("天気を指定しますか 0 or 1\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    weather_flag = 1;
    //天気の形式！！！！
    printf("天気を入力\n");
    fgets(char_weather, BUFSIZE, stdin);
    clear_stdinbuf();
  }
  
  printf("気温を指定しますか 0 or 1\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    temperature_flag = 1;
    //エラー処理！！
    printf("気温の範囲を入力 10.4-15.8\n");
    fgets(char_temperature, BUFSIZE, stdin);
    clear_stdinbuf();
    sscanf(char_temperature, "%f-%f", &small_temperature, &big_temperature);
    if(small_temperature > big_temperature){
      tmp_boule = small_temperature;
      small_temperature = big_temperature;
      big_temperature = tmp_double;
    }
  }

  printf("曜日を指定しますか 0 or 1\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    day_flag = 1;
    //曜日の形式！！！！
    printf("曜日を入力\n");
    fgets(char_day, BUFSIZE, stdin);
    clear_stdinbuf();
  }

  printf("性別を指定しますか\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    gender_flag = 1;
    printf("性別を入力 男1 女0");
    //エラー処理！！！
    fgets(&char_gender, 1, stdin);
    clear_stdinbuf();
  }
   
  printf("年齢を指定しますか\n");
  fgets(&input, 1, stdin);
  clear_stdinbuf();
  if(input == '1'){
    printf("年齢の範囲を入力　50-60");
    fgets(char_age, BUFSIZE, stdin);
    clear_stdinbuf();
    sscanf(char_age, "%d-%d", &small_age, &big_age);
    if(small_age > big_age){
      tmp_int = small_age;
      small_age = big_age;
      big_age = tmp;
    }
  }
  sprintf(sql, "SELECT sales_info1.slip_num, sales_info1.store_num, item_code, sell_unit_price, num, subtotal, weather,temprature, data_time, day, gender, age FROM sales_info1, sales_info2 WHERE 20150720 <= data_time AND data_time <=20150726 AND sales_info1.slip_num = sales_info2.slip_num AND sales_info1.store_num = sales_info2.store_num");
  
  sprintf(sql1, );

}

int main(int argc, char *argv[]){
  char connInfo[BUFSIZE];
  char sql[BUFSIZE];
  char command[BUFSIZE];
  PGconn *con;
  PGresult *res;
  int resultFields, resultRows;
  int i;
  sales_info* sales1, *sales2;
  char *tmp;
  
  sprintf(connInfo, "host=%s port=%s dbname=%s user=%s password=%s",
	  dbHost, dbPort, dbName, dbLogin, dbPwd);
  con = PQconnectdb(connInfo);
  if(PQstatus(con) == CONNECTION_BAD){
    printf("%s", PQresultErrorMessage(res));
    PQfinish(con);
    exit(1);
  }
 
  sprintf(sql, "SELECT sales_info1.slip_num, sales_info1.store_num, item_code, sell_unit_price, num, subtotal, weather,temprature, data_time, day, gender, age FROM sales_info1, sales_info2 WHERE 20150720 <= data_time AND data_time <=20150726 AND sales_info1.slip_num = sales_info2.slip_num AND sales_info1.store_num = sales_info2.store_num");
  res = PQexec(con, sql);
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
    printf("%s", PQresultErrorMessage(res));
    PQfinish(con);
    exit(1);
  }
  resultRows = PQntuples(res);
  sales1 = (sales_info *)calloc(resultRows, sizeof(sales_info));
  for(i = 0; i< resultRows; i++){
    sales1[i].slip_num = strtol(PQgetvalue(res, i, 0), &tmp, 0);
    sales1[i].store_num = atoi(PQgetvalue(res, i, 1));
    sales1[i].weather = atoi(PQgetvalue(res, i, 2));
    sales1[i].temprature = strtod(PQgetvalue(res, i, 3), &tmp);
    sales1[i].data_time = strtol(PQgetvalue(res, i, 4), &tmp, 0);
    strncpy(sales1[i].day, PQgetvalue(res, i, 5), strlen(PQgetvalue(res, i, 5)));				
    sales1[i].gender = PQgetvalue(res, i, 6);
    sales1[i].age = atoi(PQgetvalue(res, i, 7));
  }

  PQclear(res);

  sprintf(sql, "SELECT sales_info1.slip_num, sales_info1.store_num,item_code, sell_unit_price, num, subtotal,weather,temprature, data_time, day, gender, age FROM sales_info1, sales_info2 WHERE 20150713 <= data_time AND data_time < 20150720 AND sales_info1.slip_num = sales_info2.slip_num AND sales_info1.store_num = sales_info2.store_num");

  res = PQexec(con, sql);
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
    printf("%s", PQresultErrorMessage(res));
    PQfinish(con);
    exit(1);
  }
  resultRows = PQntuples(res);
  sales2 = (sales_info *)calloc(resultRows, sizeof(sales_info));

  for(i = 0; i< resultRows; i++){
    sales2[i].slip_num = strtol(PQgetvalue(res, i, 0), &tmp, 0);
    sales2[i].store_num = atoi(PQgetvalue(res, i, 1));
    sales2[i].weather = atoi(PQgetvalue(res, i, 2));
    sales2[i].temprature = strtod(PQgetvalue(res, i, 3), &tmp);
    sales2[i].data_time = strtol(PQgetvalue(res, i, 4), &tmp, 0);
    strncpy(sales2[i].day, PQgetvalue(res, i, 5), strlen(PQgetvalue(res, i, 5)));
    sales2[i].gender = PQgetvalue(res, i, 6);
    sales2[i].age = atoi(PQgetvalue(res, i, 7));
  }
  PQclear(res);
  
  
  

  
  
  PQfinish(con);
  return 0;
}
