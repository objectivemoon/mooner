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
#include <errno.h>
#include "moon.h"
#include "mooner.h"
#include "rmq_utils.h"

/*
**
*/
t_env			*init_env()
{
  t_env			*ret;

  ret=malloc(sizeof(*ret));
  memset(ret, ZERO, sizeof(*ret));
  set_filter(&(ret->filter),  DEFAULT_PORT);
  ret->rmq_srv_dns_field=RMQ_SRV_DNS_FIELD;
  ret->port=DEFAULT_PORT;
  ret->rmq_exchange=DEF_RMQ_EXCH;
  ret->rmq_routingkey=DEF_RMQ_RKEY;
  ret->max_tab_size=MAX_TAB_SIZE;
  ret->msg_tab=init_msg_tab(MAX_TAB_SIZE);
  ret->pidfile_path=DEFAULT_PID_FILE_PATH;
  ret->passwd_file=PASSWD_FILE;
  ret->to_exec[TH_SYN]=set_syn_packet;
  ret->to_exec[TH_ACK]=set_ack_packet;
  ret->to_exec[TH_SYN + TH_ACK]=set_synack_packet;
  ret->myhostname=get_my_hostname();
  ret->synflood_ret_time=GARBAGE_RETENTION_TIME;
  ret->hash_ack=malloc((HASH_SIZE + 1) * sizeof(*ret->hash_ack));
  memset(ret->hash_ack, ZERO, (HASH_SIZE + 1) * sizeof(*ret->hash_ack));

  return ret;
}

t_msg			*init_msg_tab(int size)
{
  t_msg			*ret;

  ret=malloc(size * (sizeof(*ret)));
  if (!ret)
    {
      send_log(LOG_EMERG, "Can't preallocate memory, try to set it with -a option : ", strerror(errno));
      mooner_exit(1);
    }
  memset(ret, 0, size * (sizeof(*ret)));
  
  return ret;
}
