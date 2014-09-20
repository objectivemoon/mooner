/*
**
*/
#include <stdio.h>

/*
**
*/
#include <moon.h>
#include <moontools.h>

/*
**
*/
int			main()
{
  char			buff[43];
  
  fnb_dopass(buff,COLLECTOR_PASS_SIZE);
  printf("%s\n",buff);
  
  return(0);
}
