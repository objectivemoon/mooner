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
#include <unistd.h>
#include <sys/time.h>
#include "moon.h"
#include "mooner.h"
#include "rmq_utils.h"

/*
**
*/
void					send_perf_data_to_rmq(int pos)
{
  t_to_send				msg;
  
  e->msg_tab[pos].server_rep_time = (e->msg_tab[pos].s_tv_sec * 1000000 + e->msg_tab[pos].s_tv_usec) - (e->msg_tab[pos].c_syn_tv_sec * 1000000 + e->msg_tab[pos].c_syn_tv_usec);
  e->msg_tab[pos].client_rep_time = (e->msg_tab[pos].c_ack_tv_sec * 1000000 + e->msg_tab[pos].c_ack_tv_usec) - (e->msg_tab[pos].s_tv_sec * 1000000 + e->msg_tab[pos].s_tv_usec);
  e->msg_tab[pos].zero=ZERO; 
  msg.version = VERSION_1;
  msg.data_type = STD_DATA;
  memcpy(msg.id_collector, e->client_login, COLLECTOR_PASS_SIZE);
  msg.ip_dst = e->msg_tab[pos].ip_dst.s_addr;
  msg.ip_src = e->msg_tab[pos].ip_src.s_addr;
  msg.s_r_time = e->msg_tab[pos].server_rep_time;
  msg.c_r_time = e->msg_tab[pos].client_rep_time;
  msg.date = e->msg_tab[pos].c_syn_tv_sec;
  if (IS_VERBOSE)
    send_log(LOG_DEBUG, "[29] Sending data [%d] : ver %d, typ %d, idc %.16s, ips %u, ipc %u, srt %i, crt %i, date %d\n",
	     e->nbr_sent_packet,
	     msg.version, 
	     msg.data_type,
	     msg.id_collector,
	     msg.ip_dst,
	     msg.ip_src,
	     msg.s_r_time,
	     msg.c_r_time,
	     msg.date);
  send_to_rmq((char *)&msg,sizeof(msg));
}


void					*send_metadata_to_rmq(void *n)
{
  t_m_to_send				msg;

  memset(&msg, ZERO, sizeof(msg));
  msg.version = VERSION_1;
  msg.data_type = METADATA_COL;
  memcpy(msg.id_collector, e->client_login, COLLECTOR_PASS_SIZE);
  snprintf(msg.metadata, METADATA_MAX_SIZE, "2:%s\n3:%s\n4:%d\n5:%d\n6:%d\n7:%d\n8:%d\n9:%d\n10:%d",
	   e->myhostname, 
	   MY_RELEASE,
	   e->tcp_tab_max_used,
	   e->nbr_syn_send,
	   e->nbr_synack_send,
	   e->nbr_ack_send,
	   e->stat_hash_filtered,
	   e->stat_hash_not_filtered,
	   e->stat_tcpflood_dest_entries
	   );
  if (IS_VERBOSE)
    send_log(LOG_DEBUG, "[30] Sending Metadata [%d] : ver %d, typ %d, idc %.16s, met [\n%s]\n", e->nbr_sent_packet, msg.version, msg.data_type, msg.id_collector, msg.metadata);
  send_to_rmq((char *)&msg,sizeof(msg));

  return n;
}

int					send_to_rmq(char *str, int len)
{
  amqp_basic_properties_t		props;
  amqp_bytes_t				message_bytes;
  int					ret;
  int					try;

  pthread_mutex_lock(&gl_lock_send_data);
  message_bytes.bytes=str;
  message_bytes.len=len;
  props._flags=AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
  props.content_type=amqp_cstring_bytes("blob");
  props.delivery_mode=2;
  try=0;
  do
    {
      if (e->rmq_conn)
	ret=amqp_basic_publish(e->rmq_conn,				\
				 1,					\
				 amqp_cstring_bytes(e->rmq_exchange),	\
				 amqp_cstring_bytes(e->rmq_routingkey),	\
				 0,					\
				 0,					\
				 &props,
				 message_bytes);
      else
	ret=ONE;
      if (ret == ZERO)
	{
	  e->nbr_sent_packet++;
	  break;
	}
      sleep(ONE);
      try++;
    } while (ret != 0 && try < RETRY_NBR);
  pthread_mutex_unlock(&gl_lock_send_data);
  if ((ret != 0) && (try >= RETRY_NBR))
    {
      send_log(LOG_ALERT, "[31] Can't send data to broker after [%d] retry.\n", RETRY_NBR);
      set_rmq_server_list();
    }

  return (1);
}
