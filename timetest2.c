#include <stdio.h>
#include <time.h>

void get_current_time(char *current_time)
{
  time_t now = time(NULL);
  struct tm *pnow = localtime(&now);

  sprintf(current_time,"today is %2d:%2d:%2d:%2d\n",
		 pnow->tm_year+1900,
		 pnow->tm_mon+1,
		 pnow->tm_mday,
		 pnow->tm_hour
		 );

  return;

}

int main(void)
{

  char current_time[20];

  get_current_time(current_time);

  printf("%s\n",current_time);

  return 0;


}
