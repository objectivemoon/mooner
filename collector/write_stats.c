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
void				write_stat(char *file_path, int nbr_packet)
{
  FILE				*stat_file;

  if (IS_VERBOSE)
    send_log(LOG_DEBUG, "[41] Write stats [%d] to file [%s]\n", nbr_packet, file_path);
  stat_file=fopen(file_path,"w");
  if (stat_file == NULL)
    send_log(LOG_ERR, "[42] Can't open file [%s] : stats are not available\n", file_path);
  fprintf(stat_file,"%d",nbr_packet);
  fclose(stat_file);
}

void				*write_01_min_stats(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  write_stat(NBR_SEND_01M_FILE, e->nbr_sent_packet - e->nbr_sent_packet_last_01_minute);
  e->nbr_sent_packet_last_01_minute=e->nbr_sent_packet;
  write_stat(MAX_TCP_TAB_USED_FILE, e->tcp_tab_max_used);
  write_stat(NBR_SYN_FILE, e->nbr_syn_send);
  e->nbr_syn_send_s /= 60;
  e->nbr_syn_send=0;
  write_stat(NBR_SYNACK_FILE, e->nbr_synack_send_s);
  e->nbr_synack_send_s /= 60;
  e->nbr_synack_send_s=0;
  write_stat(NBR_ACK_FILE, e->nbr_ack_send);
  e->nbr_ack_send_s /= 60;
  e->nbr_ack_send=0;
  write_stat(NBR_ACK_FILTERED, e->stat_hash_filtered);
  write_stat(NBR_ACK_NOT_FILTERED, e->stat_hash_not_filtered);
  write_stat(NBR_TCPFLOOD_FILE, e->stat_tcpflood_dest_entries);
  e->stat_tcpflood_dest_entries=0;

  return (I_do_not_use_this_var);
}

void				*write_05_min_stats(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  write_stat(NBR_SEND_05M_FILE , e->nbr_sent_packet - e->nbr_sent_packet_last_05_minute);
  e->nbr_sent_packet_last_05_minute=e->nbr_sent_packet;

  return (I_do_not_use_this_var);
}

void				*write_15_min_stats(void *I_do_not_use_this_var __attribute__ ((unused)))
{
  write_stat(NBR_SEND_15M_FILE,  e->nbr_sent_packet - e->nbr_sent_packet_last_15_minute);
  e->nbr_sent_packet_last_15_minute=e->nbr_sent_packet;

  return (I_do_not_use_this_var);
}

