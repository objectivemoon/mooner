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
#include "moon.h"
#include "mooner.h"
#include <sys/wait.h>

/*
**
*/
void                    save_av_ac(char ***sav, int *sac, char **param, int *len, int ac, char **av)
{
  char                  **tav;
  int                   i;
  int                   size;
  char                  *buff;

  *sac=ac;
  tav=malloc(ac * sizeof(*tav));
  for (i=0 ; i < ac ; i++)
    tav[i]=av[i];
  *sav=tav;
  size=(unsigned long)av[ac - 1] + strlen(av[ac - 1]) - (unsigned long)av[0] + 1;
  buff=malloc(size);
  bcopy(*av, buff, size);
  *param=buff;
  *len=size;
}

void                    restore_av_ac(char **sav, int sac, char *param, int len, int *ac, char ***av)
{
  int                   i;

  *ac=sac;
  for (i=0 ; i < sac ; i++)
    bcopy(param, **av, len);
  free(param);
  free(sav);
}

char			*replace_av0(int ac, char **av, char *str)
{
  return ((str ? strncpy(av[0], str, (unsigned long)av[ac - 1] + strlen(av[ac - 1]) - (unsigned long)av[0]) : NULL));
}

void			new_process(int ac, char **av)
{
  pid_t			pid;
  int			status;
  char                  **sav;
  int                   sac;
  char                  *param;
  int                   len;

  do {
    pid=fork();
    if (pid < 0)
      {
	send_log(LOG_EMERG, "can't fork ! :-) \n");
	
	mooner_exit(1);
      }
    if (pid > 0)
      {
#define		NEW_ARGV0	"cap canaveral launchpad"
	save_av_ac(&sav, &sac, &param, &len, ac, av);
	replace_av0(ac, av, NEW_ARGV0);
	wait(&status);
	restore_av_ac(sav, sac, param, len, &ac, &av);
      }
  } while (pid);
}
