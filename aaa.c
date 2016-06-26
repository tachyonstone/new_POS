#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define BUFSIZE  1024

void clear_stdinbuf(){
  int c;
  while ((c = getchar()) != '\n')
    ;
  return;
}

void create_sql(char* sql1, char* sql2){
  char input[BUFSIZE];
  int c;
  double tmp_double;
  int tmp_int;
  int char_count = 0;
  double small_temperature, big_temperature;
  int small_age, big_age;
  //long int before_period1, after_period1, before_period2, after_period2;
  char before_period1[10], after_period1[10], before_period2[10], after_period2[10];
  int store_num_flag = 0, weather_flag = 0, temperature_flag = 0, day_flag = 0, gender_flag = 0, age_flag = 0;
  char char_period[BUFSIZE] ,char_store_num[BUFSIZE], char_weather[BUFSIZE], char_temperature[BUFSIZE], char_day[BUFSIZE], char_gender, char_age[BUFSIZE];

  printf("比較期間を入力 20150101 20150102 20150103 20150104\n");
  fgets(char_period, BUFSIZE, stdin);
  if(strlen(char_period) >= BUFSIZE)
    clear_stdinbuf();
  sscanf(char_period, "%s %s %s %s", before_period1, after_period1, before_period2, after_period2);

  // 結合のため入力必須？
  printf("店番号を指定しますか 0 or 1\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if(strncmp(input, "1\n", 2) == 0){
    store_num_flag = 1;
    printf("店番号を入力\n");
    fgets(char_store_num, BUFSIZE, stdin);
    if(strlen(char_store_num) >= BUFSIZE)
      clear_stdinbuf();
  }
  
  printf("天気を指定しますか 0 or 1\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if((strncmp(input, "1\n", 2)) == 0){
    weather_flag = 1;
    //天気の形式！！！！
    printf("天気を入力\n");
    fgets(char_weather, BUFSIZE, stdin);
    if(strlen(char_weather) >= BUFSIZE)
      clear_stdinbuf();
    }
  
  printf("気温を指定しますか 0 or 1\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if((strncmp(input, "1\n", 2)) == 0){
    temperature_flag = 1;
    //エラー処理！！
    printf("気温の範囲を入力 10.4-15.8\n");
    fgets(char_temperature, BUFSIZE, stdin);
    if(strlen(char_temperature) >= BUFSIZE)
      clear_stdinbuf();
    sscanf(char_temperature, "%lf-%lf", &small_temperature, &big_temperature);
    if(small_temperature > big_temperature){
      tmp_double = small_temperature;
      small_temperature = big_temperature;
      big_temperature = tmp_double;
    }
  }

  printf("曜日を指定しますか 0 or 1\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if((strncmp(input, "1\n", 2)) == 0){
    day_flag = 1;
    //曜日の形式！！！！
    printf("曜日を入力\n");
    fgets(char_day, BUFSIZE, stdin);
    if(strlen(char_day) >= BUFSIZE)
      clear_stdinbuf();
  }

  printf("性別を指定しますか\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if((strncmp(input, "1\n", 2)) == 0){
    gender_flag = 1;
    printf("性別を入力 男1 女0");
    //エラー処理！！！
    fgets(&char_gender, 1, stdin);
    clear_stdinbuf();
  }
   
  printf("年齢を指定しますか\n");
  fgets(input, BUFSIZE, stdin);
  if(strlen(input) >= BUFSIZE)
    clear_stdinbuf();
  if((strncmp(input, "1\n", 2)) == 0){
    printf("年齢の範囲を入力　50-60");
    fgets(char_age, BUFSIZE, stdin);
    if(strlen(char_age) >= BUFSIZE)
      clear_stdinbuf();
    sscanf(char_age, "%d-%d", &small_age, &big_age);
    if(small_age > big_age){
      tmp_int = small_age;
      small_age = big_age;
      big_age = tmp_int;
    }
  }
  
  char_count += sprintf(sql1, "SELECT sales_info1.slip_num, sales_info1.store_num, item_code, sell_unit_price, num, subtotal, weather,temprature, data_time, day, gender, age FROM sales_info1, sales_info2 WHERE %s <= data_time AND data_time <= %s ", before_period1, after_period1);

  if(weather_flag){
    char_count += sprintf(sql1 + char_count, " AND weather = ");
    char_count += sprintf(sql1 + char_count , "%s", char_weather);
  }
  
  if(temperature_flag){
    char_count += sprintf(sql1 + char_count, " AND temperature = ");
    char_count += sprintf(sql1 + char_count, " %f <= temperature AND temperature <= %f", small_temperature, big_temperature);
  }

  if(day_flag){
    char_count += sprintf(sql1 + char_count, " AND day = %s", char_day);
  }

  if(gender_flag){
    char_count += sprintf(sql1 + char_count, " AND gender = %c", char_gender);
  }

  if(age_flag){
    char_count += sprintf(sql1 + char_count, " AND %d <= age AND %d <= age", small_age, big_age);
  }
    
  return;

}

int main(voidd){
  char sql1[BUFSIZE], sql2[BUFSIZE];
  create_sql(sql1, sql2);
  printf("%s\n", sql1);
  return 0;
}
