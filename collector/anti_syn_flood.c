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

/*
**
*/
void				*anti_syn_flood_check(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  int				n;
  struct timeval		begin;
  int				count;

  gettimeofday(&begin,NULL);
  for (n=count=0 ; n < e->max_tab_size ; n++)
    if (e->msg_tab[n].c_syn_tv_sec && (begin.tv_sec - e->msg_tab[n].c_syn_tv_sec) > GARBAGE_RETENTION_TIME)
      {
	pthread_mutex_lock(&gl_lock_garbage);
	memset(&(e->msg_tab[n]),0,sizeof(t_msg));
	pthread_mutex_unlock(&gl_lock_garbage);
	count++;
      }
  e->stat_tcpflood_dest_entries += count;
  if (IS_VERBOSE)
    send_log(LOG_DEBUG, "[0] Start anti synflood collecting at %d : check %d entry, found %d to destroy (elasped time = %ds)\n",\
	     begin.tv_sec,e->max_tab_size, count, e->synflood_ret_time);
  if (count * 100 / e->max_tab_size >= 80)
    send_log(LOG_WARNING, "[46] You reach more than 80% of the max size of your tcp connection buffer. Maybe you should restart with option -a\n");
  return I_do_not_use_this_var;
}

