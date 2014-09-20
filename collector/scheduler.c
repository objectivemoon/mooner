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
#include <errno.h>
/*
**
*/
int				gl_nbr_thread;

/*
**
*/

void				*thread_and_exit(void *procedure)
{
  t_call			call;

  call=*(t_call *)(&procedure);
  call(NULL);
  if (IS_VERBOSE)
    send_log(LOG_INFO, "Scheduler exiting thread %d\n", --gl_nbr_thread);
  pthread_exit(NULL);

  return (NULL);
}

void				fork_call(void *(*procedure)(void *), char *info)
{
  pthread_t			proc;
  t_call			call;
  void				*to_pass;
  int				ret;

  call=procedure;
  to_pass=*(void **)(&call); /* HAHAHA ! Try to know why I did that ! :-) */
  if (IS_VERBOSE)
    send_log(LOG_INFO, "Scheduler create  thread %d to [%s]\n", ++gl_nbr_thread, info);
  ret=pthread_create(&proc, NULL, thread_and_exit, to_pass);
  pthread_detach(proc);
  if (ret != ZERO)
    send_log(LOG_EMERG, "[28] Can't create Thread for scheduling\n");
}

void				*th_scheduler()
{
  int				sched_time;
  t_scheduler			tasks[]=		/*should be move to global or not to add task on execution ? */
    {
      {300, send_metadata_to_rmq, "(1) send metadata"},
      {3, check_hostname_change, "(2) check for hostname change"},
      {e->synflood_ret_time, anti_syn_flood_check, "(3) anti syn flood flushing buffer"},
      {5, decrease_srv_ttl, "(4) decrease DNS TTL"},
      {60, write_01_min_stats, "(5) write last 01 minutes sent packet"},
      {300, write_05_min_stats, "(6) write last 05 minutes sent packet"},
      {900, write_15_min_stats, "(7) write last 15 minutes sent packet"},
      {0, 0, {0}}
    };
  
  if (IS_VERBOSE)
    gl_nbr_thread=0;
  sleep(TWO);
  sched_time=ZERO;
  while (TRUE)
    {
      t_scheduler		*tmp;

      if (IS_VERBOSE)
	{
	  time_t		t;
	  char			date[42];

	  t=time(NULL);
	  strftime(date, 42,"Scheduling %M:%S\n",localtime(&t));
	  send_log(LOG_INFO,"%s", date);
	}
      for (tmp=tasks ; tmp->time ; tmp++)
	if (sched_time % tmp->time == 0)
	    fork_call(tmp->func, tmp->info);
      sleep(1);
      sched_time++;
    }
  return ((void *)ZERO);
}

