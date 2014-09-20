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
int			set_mq_host(int begin)
{
  int			ret;

  if (e->option & OPT_SET_RMQ)
    {
      send_log(LOG_INFO, "[32] Broker server as been set by -r option to [%s]:[%d] no resolver check to be done\n", e->rmqserver->target, e->rmqserver->port);
      if (!init_rmq(e->rmqserver))
	return (ONE);
      send_log(LOG_INFO, "[33] Broker as been initialized\n");

      return (ZERO);
    }
  pthread_mutex_lock(&gl_lock_set_rmq);
  if (create_rmq_serverlist())
    {
      pthread_mutex_unlock(&gl_lock_set_rmq);

      return (ONE);
    }
  ret = randomize_serverlist_of_same_weight(begin);
  pthread_mutex_unlock(&gl_lock_set_rmq);

  return (ret);
}

void			free_rr_entries(ldns_rr_list *srv, ldns_pkt *p, ldns_resolver *res, ldns_rdf *domain)
{
  if (srv)
    ldns_rr_list_deep_free(srv);
  if (p)
    ldns_pkt_free(p);
  if (res)
    ldns_resolver_deep_free(res);
  if (domain)
    ldns_rdf_free(domain);
}

int			create_rmq_serverlist()
{
  ldns_rr_list		*srv=0;
  ldns_pkt		*p=0;
  ldns_resolver		*res=0;
  ldns_rdf		*domain=0;
  unsigned int		i;
  int			count;

  if (e->rmqserver)
    {
      e->rmqserver=0;
      free_rmq_server_list();
    }
  if (IS_VERBOSE)
    send_log(LOG_INFO, "[47] try to get servername from service [%s]\n", e->rmq_srv_dns_field);
  if ((domain=ldns_dname_new_frm_str(e->rmq_srv_dns_field)) == NULL)
    {
      free_rr_entries(srv, p, res, domain);
      
      return (ONE);
    }
  if ((ldns_resolver_new_frm_file(&res, NULL)) != LDNS_STATUS_OK)
    {
      free_rr_entries(srv, p, res, domain);

      return (ONE);
    }
  if (ldns_resolver_query_status(&p, res, domain, LDNS_RR_TYPE_SRV, LDNS_RR_CLASS_IN, LDNS_RD) != LDNS_STATUS_OK)
    {
      free_rr_entries(srv, p, res, domain);
      
      return (ONE);
    }
  if ((srv=ldns_pkt_rr_list_by_type(p, LDNS_RR_TYPE_SRV, LDNS_SECTION_ANSWER)) == NULL)
    {
      free_rr_entries(srv, p, res, domain);
      
      return (ONE);
    }
  ldns_rr_list_sort(srv); 
  e->rmqserverlist_size=srv->_rr_count;
  e->rmqserverlist=malloc((sizeof *(e->rmqserverlist) + 2) * srv->_rr_count);
  memset(e->rmqserverlist,0,(sizeof *(e->rmqserverlist) + 2) * srv->_rr_count);
  for (i = 0 ; i < srv->_rr_count ; i++)
    {
      e->rmqserverlist[i].ttl = srv->_rrs[i]->_ttl;
      e->rmqserverlist[i].priority = ldns_rdf2native_int16(srv->_rrs[i]->_rdata_fields[0]);
      e->rmqserverlist[i].weight = ldns_rdf2native_int16(srv->_rrs[i]->_rdata_fields[1]);
      e->rmqserverlist[i].port = ldns_rdf2native_int16((srv->_rrs[i]->_rdata_fields)[2]);
      e->rmqserverlist[i].target = malloc(srv->_rrs[i]->_rdata_fields[3]->_size + 8);
      memset(e->rmqserverlist[i].target,0,srv->_rrs[i]->_rdata_fields[3]->_size + 8);
      strncpy(e->rmqserverlist[i].target,ldns_rdf2str(srv->_rrs[i]->_rdata_fields[3]),srv->_rrs[i]->_rdata_fields[3]->_size);
      e->rmqserverlist[i].is_up = TRUE;
    }
  for (count=0 ; count < e->rmqserverlist_size ; count++)
    send_log(LOG_DEBUG, "[34] Srv [%d] TTL[%d] P[%4d] W[%4d] p[%d] [%s]\n",	\
	     count,								\
	     e->rmqserverlist[count].ttl,					\
	     e->rmqserverlist[count].priority,					\
	     e->rmqserverlist[count].weight,					\
	     e->rmqserverlist[count].port,					\
	     e->rmqserverlist[count].target);
  free_rr_entries(srv, p, res, domain);
  
  return (ZERO);
}

void				free_rmq_server_list()
{
  int				i;

  for (i=0 ; i < e->rmqserverlist_size ; i++)
    free(e->rmqserverlist[i].target);
  if (e->rmqserverlist)
    free(e->rmqserverlist);
  e->rmqserverlist=0 ;
}

int				randomize_serverlist_of_same_weight(int begin)
{
  int				j;
  int				tot_weight;
  int				priority;
  int				nbr_false_server;
  int				*tab_weight;

  if ((begin == MINUS_ONE) || !(e->rmqserverlist[begin].port))
    return MINUS_ONE;
  priority=e->rmqserverlist[begin].priority; 
  tot_weight=get_tot_weight(priority);
  nbr_false_server=get_tot_false(priority);
  tab_weight=init_tab_weight(priority, tot_weight);
  if (nbr_false_server == nbr_server_of_priority(priority))
    return (randomize_serverlist_of_same_weight(get_pos_next_priority(begin)));
  else
    {
      int			pos;

      do {
	struct timeval		tv;

	usleep(ONE);
	gettimeofday(&tv,NULL);
	srand((int)(tv.tv_sec + tv.tv_usec));
	if (tot_weight <= 0 )
	  return (MINUS_ONE);
	j=rand() % tot_weight;
	pos=tab_weight[j];
      } while (!e->rmqserverlist[pos].is_up);
      free(tab_weight);
      if (init_rmq(&(e->rmqserverlist[pos])))
	{
	  send_log(LOG_INFO, "[35] Broker as been initialized\n");
	  e->rmqserver=&(e->rmqserverlist[pos]);
  
	  return (ZERO);
	}
	else
	  {
	    e->rmqserverlist[pos].is_up = FALSE;
	    return (randomize_serverlist_of_same_weight(begin));
	  }
    }
  return MINUS_ONE;
}

void				*reload_rmq_server_list_ttl(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  send_log(LOG_INFO, "[37] Broker [%s] TTL at 0 reconnecting...\n",e->rmqserver->target);
  if (e->rmq_conn)
    close_rmq_channel(&e->rmq_conn);
  set_rmq_server_list();

  return (I_do_not_use_this_var);
}

void				set_rmq_server_list()
{
  int				retry;
  
  retry=0;
  while (set_mq_host(STARTING_POINT))
    {
      send_log(LOG_WARNING, "[38] I can't be sent to Cap Canaveral ! \n Can't open any connection to any broker\n ... try again in 1 minute (retry : [%d])\n", retry);
      retry++;
      sleep(ONE_MINUTE);
    }
}

void				*decrease_srv_ttl(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  if (e->option & OPT_SET_RMQ)
    return (I_do_not_use_this_var);
  pthread_mutex_lock(&gl_lock_set_rmq);
  if ((e->rmqserver) && ((e->rmqserver->ttl -= TTL_TIME) <= 0))
    {
      pthread_mutex_unlock(&gl_lock_set_rmq);
      reload_rmq_server_list_ttl(I_do_not_use_this_var);
    }
  else
      pthread_mutex_unlock(&gl_lock_set_rmq);
  
  return (I_do_not_use_this_var);
}
