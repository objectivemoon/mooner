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
#include <sys/time.h>
#include "moon.h"
#include "mooner.h"

/*
**
*/

int				set_syn_packet(t_ip *ip, t_tcp *tcp, struct timeval *tv)
{
  int				n;
  
  for (n=0 ; n < e->max_tab_size && e->msg_tab[n].c_syn_tv_sec ; n++)
    ;
  if (n == e->max_tab_size)
    return (MINUS_ONE);
  if (n > e->tcp_tab_max_used)
    e->tcp_tab_max_used=n;
  e->msg_tab[n].c_syn_tv_sec=tv->tv_sec;
  e->msg_tab[n].c_syn_tv_usec=tv->tv_usec;
  e->msg_tab[n].ip_src=ip->ip_src;
  e->msg_tab[n].ip_dst=ip->ip_dst;
  e->msg_tab[n].c_syn=tcp->th_seq;
  e->msg_tab[n].tcp_flags=tcp->th_flags;
  e->msg_tab[n].zero=ZERO;
  if (IS_VERBOSE && IS_SECRET_OPT)
    print_it(&(e->msg_tab[n]),n);
  e->nbr_syn_send++;

  return (MINUS_ONE);
}

int				set_synack_packet(t_ip *ip, t_tcp *tcp, struct timeval *tv)
{
  int				n;
  unsigned int			ack2syn;
  unsigned int			tohash;
	  
  ack2syn=ntohl(tcp->th_ack) - 1;
  for (n=0 ; n < e->max_tab_size ; n++)
    if ((e->msg_tab[n].c_syn_tv_sec) 
	&& (ntohl(e->msg_tab[n].c_syn) == ack2syn) 
	&& (e->msg_tab[n].ip_src.s_addr == ip->ip_dst.s_addr))
      break;
  if (n == e->max_tab_size)
    return (MINUS_ONE);
  e->msg_tab[n].s_tv_sec=tv->tv_sec;
  e->msg_tab[n].s_tv_usec=tv->tv_usec;
  e->msg_tab[n].s_syn=tcp->th_seq;
  tohash=ntohl(tcp->th_seq);
  e->hash_ack[tohash & HASH_SIZE]=tcp->th_seq;
  if (IS_VERBOSE && IS_SECRET_OPT)
    print_it(&(e->msg_tab[n]),n);
  e->nbr_synack_send++;

  return (MINUS_ONE);
}

int				set_ack_packet(t_ip *ip, t_tcp *tcp, struct timeval *tv)
{
  int				n;
  unsigned int			ack2syn;

  ack2syn=ntohl(tcp->th_ack) - 1;
  if (!e->hash_ack[ack2syn & HASH_SIZE])
    {
      e->stat_hash_filtered++;
      return (MINUS_ONE);
    }
  for (n=0 ; n < e->max_tab_size ; n++)
    {
      if ((e->msg_tab[n].c_syn_tv_sec) 
	  && (ntohl(e->msg_tab[n].s_syn) == ack2syn) 
	  && (e->msg_tab[n].ip_src.s_addr == ip->ip_src.s_addr))
	break;
    }
  if (n == e->max_tab_size)
    {
      e->stat_hash_not_filtered++;
      return (MINUS_ONE);
    }
  e->msg_tab[n].c_ack_tv_sec=tv->tv_sec;
  e->msg_tab[n].c_ack_tv_usec=tv->tv_usec;
  e->hash_ack[ack2syn & HASH_SIZE]=0;
  if (IS_VERBOSE && IS_SECRET_OPT)
    print_it(&(e->msg_tab[n]),n);
  e->nbr_ack_send++;

  return (n);
}
