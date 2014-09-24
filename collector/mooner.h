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
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <time.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <assert.h>
#include <syslog.h>
#include <stdarg.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

/*
**
*/
#include "fnb_ip.h"

/*
**
*/
#define	MY_RELEASE		"1.0.2"

/*
**
*/
#define SUMMARY_EVERY_US	1000000
#define BUFFSIZE		30
#define WRITE_PACK_BUFFSIZE	4000
#define CHAR_PADDING		'.'
#ifndef __FNB_HIT__
#  define __FNB_HIT__

typedef void			*(*t_call)(void *);

typedef struct			s_opt{
  char				copt;
  int				iopt;
}				t_opt;

typedef struct			s_msg
{
  int				client_rep_time;
  int				server_rep_time;
  time_t			c_syn_tv_sec;
  suseconds_t			c_syn_tv_usec;
  time_t			s_tv_sec;
  suseconds_t			s_tv_usec;
  time_t			c_ack_tv_sec;
  suseconds_t			c_ack_tv_usec;
  struct in_addr		ip_src;
  struct in_addr		ip_dst;
  u_int32_t			c_syn;
  u_int32_t			s_syn;
  u_int32_t			c_ack;
  u_char			tcp_flags;
  char				zero;
}				t_msg;

typedef struct			s_to_send
{
  uint32_t			version;
#define VERSION_1		((char)1)
  uint32_t			data_type;
#define STD_DATA		(1<<0)
#define COL_KEY			(1<<1)
#define DATA_CRYPT		(1<<2)
#define METADATA_COL		(1<<3)
  char				id_collector[COLLECTOR_PASS_SIZE];
  uint64_t			ip_dst;
  uint64_t			ip_src;
  uint32_t			s_r_time;
  uint32_t			c_r_time;
  uint32_t			date;
}				t_to_send;
			
typedef struct			s_m_to_send
{
  uint32_t			version;
  uint32_t			data_type;
  char				id_collector[COLLECTOR_PASS_SIZE];
  char				metadata[METADATA_MAX_SIZE];
}				t_m_to_send;
			
#define ETHER_ADDR_LEN		6
#define SIZE_ETHERNET		14
typedef struct			s_ethernet {
  u_char			ether_dhost[ETHER_ADDR_LEN];
  u_char			ether_shost[ETHER_ADDR_LEN];
  u_short			ether_type;
}				t_ethernet;

typedef struct s_server	{
  int				priority;
  int				weight;
  int				port;
  char				*target;
  int				is_up;
  int				ttl;
}				t_server;

typedef struct			s_env{
  int				option;
  char				*log_file;
  int				port;
  int				synflood_ret_time;
  char				*filter;
  char				*dev;
  t_server			*rmqserverlist;
  int				rmqserverlist_size;
  t_server			*rmqserver;
  char				*rmq_exchange;
  char				*rmq_routingkey;
  amqp_connection_state_t	rmq_conn;
  int				max_tab_size;
#define	MAX_TAB_SIZE		420
  char				*public_key_file;
  t_msg				*msg_tab;
  char				*pidfile_path;
  char				*passwd_file;
  char				*client_login;
  char				*client_pass;
  char				*rmq_srv_dns_field;
  char				*myhostname;
  int				(*to_exec[TH_ACK + TH_SYN + 1])(t_ip *, t_tcp *, struct timeval *);
#define	HASH_SIZE		0xffff
  int				*hash_ack;
  /*
  ** statistics
  */
  int				nbr_sent_packet;
  int				nbr_sent_packet_last_01_minute;
  int				nbr_sent_packet_last_05_minute;
  int				nbr_sent_packet_last_15_minute;
  int				tcp_tab_max_used;
  int				nbr_syn_send;
  int				nbr_syn_send_s;
  int				nbr_synack_send;
  int				nbr_synack_send_s;
  int				nbr_ack_send;
  int				nbr_ack_send_s;
  int				stat_hash_filtered;
  int				stat_hash_filtered_s;
  int				stat_hash_not_filtered;
  int				stat_hash_not_filtered_s;
  int				stat_tcpflood_dest_entries;
  int				stat_tcpflood_dest_entries_s;
}				t_env;

typedef struct			s_logl {
  int				loglevel;
  char				*msg;
}				t_logl;

typedef struct			s_scheduler {
  int				time;
  void				*(*func)(void *);
  char				info[42];
}				t_scheduler;

#endif

/*
**
*/
#define OPT_ARG			"i:vp:dr:ua:yk:U:P:T:s:q:e:t:l:Vg"

/*
**
*/
t_env				*e;
FILE				*gl_log_file;
pthread_mutex_t			gl_lock_garbage;
pthread_mutex_t			gl_lock_send_data;
pthread_mutex_t			gl_lock_set_rmq;

/*
**
*/
t_env				*init_env();
void                            set_filter(char** filter, int port);
void				get_opt(int,char **);
void				usage();
void				handle_packet(u_char *, const struct pcap_pkthdr *,const u_char *);
int				set_mq_host(int);
int				init_rmq(t_server *);
int				send_to_rmq(char *, int);
int				create_rmq_serverlist();
t_msg				*init_msg_tab(int);
void				free_rmq_server_list();
void				set_available_server();
int				randomize_serverlist_of_same_weight(int);
void				send_log(int, char *, ...);
void				do_signal();
void				create_timer_process();
void				signal_prep();
void				amqp_response2logger(amqp_rpc_reply_t, char *);
char				*print_p(t_msg *);
char				*print_it(t_msg *, int);
int				set_syn_packet(t_ip *, t_tcp *, struct timeval *);
int				set_synack_packet(t_ip *, t_tcp *, struct timeval *);
int				set_ack_packet(t_ip *, t_tcp *, struct timeval *);
void				to_send_to_rmq(int);
void				*anti_syn_flood_check(void *);
void				*reload_rmq_server_list_ttl(void *);
void				*th_scheduler();
void				send_perf_data_to_rmq(int ret);
void				set_rmq_server_list();
int				close_rmq_channel(amqp_connection_state_t *);
int				get_tot_weight(int);
int				get_tot_false(int);
int				*init_tab_weight(int, int);
int				nbr_server_of_priority(int);
int				get_pos_next_priority(int);
void				write_pid_number(char *);
void				create_new_collector_code();
void				mooner_exit(int);
void				*send_metadata_to_rmq(void *);
char				*get_my_hostname();
void				fork_call(void *(*procedure)(void *), char *);
void				*check_hostname_change(void *);
char				*server_name(char *);
int				server_port(char *);
void				*write_01_min_stats(void *);
void				*write_05_min_stats(void *);
void				*write_15_min_stats(void *);
void				write_stat(char *, int);
void				*decrease_srv_ttl(void *);
int				lock_wait_rmq(int);
void				init_log_file(void);
void				close_log_file(void);
void				flush_log_file(int);
void				new_process(int, char **);
