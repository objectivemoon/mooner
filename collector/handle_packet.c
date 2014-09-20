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
void				handle_packet(u_char *args __attribute__ ((unused)), const struct pcap_pkthdr *header __attribute__ ((unused)), const u_char *packet)
{
  t_ip				*ip;
  t_tcp				*tcp;
  u_int				size_ip;
  u_int				size_tcp;
  struct timeval		tv;
  struct timezone		tz;
  int				to_send_pos=MINUS_ONE;

  ip = (t_ip *)(packet + SIZE_ETHERNET);
  size_ip = IP_HL(ip) * 4;
  if (size_ip < 20)
    return;
  tcp = (t_tcp *)(packet + SIZE_ETHERNET + size_ip);
  size_tcp = TH_OFF(tcp)*4;
  if (size_tcp < 20)
    return;
  if (gettimeofday(&tv,&tz))
    return;
  if ((tcp->th_flags == TH_SYN) || (tcp->th_flags == TH_ACK) || (tcp->th_flags == TH_ACK + TH_SYN))
    to_send_pos=e->to_exec[tcp->th_flags](ip,tcp,&tv);
  if (to_send_pos < ZERO)
    return;
  send_perf_data_to_rmq(to_send_pos);
  pthread_mutex_lock(&gl_lock_garbage);
  memset(&(e->msg_tab[to_send_pos]), 0, sizeof(t_msg));
  pthread_mutex_unlock(&gl_lock_garbage);
}
