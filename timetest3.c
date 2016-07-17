#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 20

int sub_days(char *before_day, char *after_day)
{

  int year1, month1, day1;
  int year2, month2, day2;

  struct tm d;
  time_t t1, t2;
  double diff;

  sscanf(before_day,"%4d%2d%2d", &year1, &month1, &day1);
  sscanf(after_day,"%4d%2d%2d", &year2, &month2, &day2);

  // 開始・終了日を time_t 型の変数にする
  memset(&d, 0, sizeof(d));
  d.tm_year = year1 - 1900;
  d.tm_mon = month1 - 1;
  d.tm_mday = day1;
  t1 = mktime(&d);
  d.tm_year = year2 - 1900;
  d.tm_mon = month2 - 1;
  d.tm_mday = day2;
  t2 = mktime(&d);

  diff = difftime(t2, t1);

  // difftime() の返り値は「秒」で double 型
  // ÷60÷60÷24 で日数にして、+0.5 は四捨五入のため
  return (int)(diff / 60 / 60 / 24 + 0.5);
}


int main(void)
{
  char before_day[BUFSIZE];
  char after_day[BUFSIZE];

  fgets(before_day, BUFSIZE, stdin);
  fgets(after_day, BUFSIZE, stdin);

  printf("%d\n", sub_days(before_day, after_day));

  return 0;

}
