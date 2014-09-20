/*
**    objectivemoon.io stage1 feeler
**    Copyright (C) 2014 Kwame
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
/*
**
*/
#include "moon.h"

/*
**
*/
int			fnb_a2i(char *str)
{
  int			ret;
  int			minus;

  minus=ONE;
  if (*str == CHAR_MINUS)
    {
      minus=MINUS_ONE;
      str++;
    }
  for (ret=0; *str; str++)
    if ((*str >= CHAR_0) && (*str <= CHAR_9))
      ret = (10*ret) + (*str - CHAR_0);
  return(minus*ret);
}

void			fnb_putchar(char c)
{
  write(1,&c,1);
}

int			fnb_putnstr(char *str, int n)
{
  int			i;

  for (i=0; (i < n) && (*str);i++)
    fnb_putchar(*str++);

  return(i);
}

int			fnb_dopass(char *buf, int size)
{
  struct timeval	tv;
  int			n;
  char			randstate[256];

  gettimeofday(&tv, NULL);
  initstate(tv.tv_sec + tv.tv_usec, randstate, size);
  setstate(randstate);
  for (n=0 ; n<size ; n++)
    {
      int		r;
   
      do 
	{
	  r=random() % 256;
	} while ((r < CHAR_a || r > CHAR_z)
		 && (r < CHAR_A || r > CHAR_Z) 
		 && (r < CHAR_0 || r > CHAR_9));
      buf[n]=r;
    }
  buf[n]=ZERO;
  
  return (n);
}

int			fnb_get_nbr_from_file(char *path)
{
  FILE			*file;
  char			*str;
  int			ret;

  file=fopen(path,"r");
  if (!file)
    {
      fprintf(stderr,"can't open file %s\n", path);
      exit(1);
    }
  str=malloc(METADATA_MAX_SIZE);
  memset(str, ZERO, METADATA_MAX_SIZE);
  fread(str, ONE, METADATA_MAX_SIZE, file);
  str[METADATA_MAX_SIZE]=0;
  fclose(file);
  ret=atoi(str);
  free(str);

  return(ret);
}
