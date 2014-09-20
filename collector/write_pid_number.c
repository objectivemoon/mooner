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

void			write_pid_number()
{
  pid_t			pid;
  FILE			*pidfile;

  pid=getpid();
  pidfile=fopen(e->pidfile_path,"w");
  if (pidfile == NULL)
    {
      send_log(LOG_CRIT, "[40] can't open file [%s]\n", e->pidfile_path);
      exit(1);
    }
  fprintf(pidfile,"%d\n",pid);
  fclose(pidfile);
}
