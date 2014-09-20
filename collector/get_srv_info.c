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
#include <ldns/ldns.h>

/*
**
*/
int				get_tot_weight(int priority)
{
  int				tot_weight;
  int				i;

  for (tot_weight=0, i=0 ; i < e->rmqserverlist_size ; i++)
    if (e->rmqserverlist[i].priority == priority)
      tot_weight += e->rmqserverlist[i].weight;

  return (tot_weight);
}

int				get_tot_false(int priority)
{
  int				nbr_false;
  int				i;

  for (nbr_false=0, i=0 ; i < e->rmqserverlist_size ; i++)
    if (e->rmqserverlist[i].priority == priority && e->rmqserverlist[i].is_up == FALSE)
      nbr_false++;

  return (nbr_false);
}

int				*init_tab_weight(int priority, int tot_weight)
{
  int				*tab_weight;
  int				server;
  int				weight;
  int				k;
  
  tab_weight=malloc((tot_weight + 2) * sizeof (*tab_weight));
  for (server=weight=0 ; server < e->rmqserverlist_size ; server++)
    if (e->rmqserverlist[server].priority == priority)
      for (k=0; (weight < tot_weight) && (k < e->rmqserverlist[server].weight) ; weight++, k++)
	tab_weight[weight]=server;
    else
      weight++;
  
  return (tab_weight);
}

int				nbr_server_of_priority(int priority)
{
  int				i;
  int				ret;

  for (ret=0, i=0 ; i < e->rmqserverlist_size ; i++)
    if (e->rmqserverlist[i].priority == priority)
      ret++;

  return (ret);
}

int				get_pos_next_priority(int pos)
{
  int				prev_priority;
  int				new_pos;
  int				i;

  new_pos=MINUS_ONE;
  prev_priority=e->rmqserverlist[pos].priority;
  for (i=pos ; i < e->rmqserverlist_size ; i++)
    if (e->rmqserverlist[i].priority != prev_priority)
      {
	new_pos=i;
	break;
      }

  return (new_pos);
}
