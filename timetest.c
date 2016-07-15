#include <stdio.h>
#include <time.h>

int main()
{
  time_t now = time(NULL);
  int today;

  struct tm *pnow = localtime(&now);
  //char week[][3];

  printf("today is %2d:%2d:%2d:%2d\n",
		 pnow->tm_year+1900,
		 pnow->tm_mon+1,
		 pnow->tm_mday,
		 pnow->tm_hour
		 );

  today = pnow->tm_mday;

  /*
  sleep(10);

  if(today == 20){
	printf("ok\n");
  }
  */

  return 0;


}
