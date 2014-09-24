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
** __DMALLOC__
** #define DMALLOC
**
*/

#include "moontools.h"

/*
**
*/
#define COLLECTOR_PASS_SIZE	16
#define METADATA_MAX_SIZE	2048
#define HOSTNAME_MAX_SIZE	256
#define RETRY_NBR		3
#define STARTING_POINT		0
#define FNB_COLLECTOR_NAME	"mooner"
#define CHAR_COMMA		':'
#define CHAR_a			'a'
#define CHAR_A			'A'
#define CHAR_e			'e'
#define CHAR_d			'd'
#define CHAR_g			'g'
#define CHAR_i			'i'
#define CHAR_k			'k'
#define CHAR_l			'l'
#define CHAR_n			'n'
#define CHAR_u			'u'
#define CHAR_U			'U'
#define CHAR_p			'p'
#define CHAR_P			'P'
#define CHAR_q			'q'
#define CHAR_r			'r'
#define CHAR_s			's'
#define CHAR_t			't'
#define CHAR_T			'T'
#define CHAR_v			'v'
#define CHAR_V			'V'
#define CHAR_y			'y'
#define CHAR_Z			'Z'
#define CHAR_z			'z'
#define CHAR_MINUS		'-'
#define CHAR_0			'0'
#define CHAR_9			'9'
#define ZERO			0
#define MINUS_ONE		-1
#define ONE			1
#define TWO			2
#define SIZE_BASE_TEN		10
#define DEFAULT_PORT		80
//#define STR_DEFAULT_PORT	"80"
#define FILTER_MAX_SIZE		84
//#define FILTER_BASE_EXPR	"port "
#define FILTER_BASE_EXPR	"(port %d) && ((tcp[13] & 2 = 2) || (tcp[13] = 16))"
#define FNB_MAXSERVER_NAME_LEN	42
#define FNB_MAXSERVER_NAME_LIST	12
#define DEF_RMQ_PORT		5672
#define DEF_RMQ_EXCH		"mooner"
#define DEF_RMQ_RKEY		"mooner"
#define TRUE			42
#define FALSE			0
#define RMQ_SRV_DNS_FIELD	"columbia.orbit.objectivemoon.io"
#define ONE_MINUTE		60
#define FIVE_MINUTES		300
#define FIFTEEN_MINUTES		900
#define TTL_TIME		5
#define SLASH			"/"
#define DEV_NULL		"/dev/null"
#define RMQ_FNB_USER		"____ANONYMOUS____"
#define RMQ_FNB_PASS		"fnb42!"
#define GARBAGE_RETENTION_TIME	5
#define DEFAULT_PID_FILE_PATH	"/var/run/objectivemoon/mooner/pid"
#define CAP_PID_FILE_PATH	"/var/run/objectivemoon/mooner/capcanaveral_pid"
#define PASSWD_FILE		"/var/run/objectivemoon/mooner/passwd"
#define NBR_SEND_01M_FILE	"/var/run/objectivemoon/mooner/stat_01_nbrsend"
#define NBR_SEND_05M_FILE	"/var/run/objectivemoon/mooner/stat_05_nbrsend"
#define NBR_SEND_15M_FILE	"/var/run/objectivemoon/mooner/stat_15_nbrsend"
#define MAX_TCP_TAB_USED_FILE	"/var/run/objectivemoon/mooner/stat_max_tcp_tab_used"
#define NBR_SYN_FILE		"/var/run/objectivemoon/mooner/stat_nbr_syn_01"
#define NBR_SYNACK_FILE		"/var/run/objectivemoon/mooner/stat_nbr_synack_01"
#define NBR_ACK_FILE		"/var/run/objectivemoon/mooner/stat_nbr_ack_01"
#define NBR_ACK_FILE		"/var/run/objectivemoon/mooner/stat_nbr_ack_01"
#define NBR_ACK_FILE		"/var/run/objectivemoon/mooner/stat_nbr_ack_01"
#define NBR_ACK_FILTERED	"/var/run/objectivemoon/mooner/stat_nbr_ack_filtered"
#define NBR_ACK_NOT_FILTERED	"/var/run/objectivemoon/mooner/stat_nbr_ack_not_filtered"
#define NBR_TCPFLOOD_FILE	"/var/run/objectivemoon/mooner/stat_tcp_flood_dest"

/*
**
*/
#define OPT_PORT		(1<<0)
#define OPT_VERBOSE		(1<<1)
#define OPT_DEV			(1<<2)
#define OPT_SET_RMQ		(1<<3)
#define OPT_DEBUG		(1<<4)
#define OPT_SECRET_OPT		(1<<4)
#define TO_EXIT			(1<<6)
#define OPT_CRYPTO		(1<<7)
#define OPT_LOG_FILE		(1<<8)
#define OPT_GARBAGE		(1<<9)

/*
**
*/
#define LOCK_SERVER_LIST(f)	((e->lock_rmq_server_list) |= (f))
#define UNLOCK_SERVERLIST(f)	((e->lock_rmq_server_list) &= ~(f))
#define BOFF_DEAD_BROKER	(1<<0)
#define BOFF_CREATING_LIST	(1<<1)
#define BOFF_TTL_TO_ZERO	(1<<2)
#define BOFF_CLOSING_CHAN	(1<<3)
#define BOFF_INIT_BROKER	(1<<4)
#define BOFF_SIG_KILL		(1<<5)
#define BOFF_SEND_META_DATA	(1<<6)
#define BOFF_SEND_PERF_DATA	(1<<7)

/*
**
*/
#define LOG_FILE_OR_STDERR	(e->option & OPT_LOG_FILE ? gl_log_file : stderr)
#define LOCK_LOG_FILE		(1<<0)
#define UNLOCK_LOG_FILE		(1<<1)

/*
**
*/
#define IS_VERBOSE		((e->option & OPT_VERBOSE) || (e->option & OPT_DEBUG))
#define IS_PORT			(e->option & OPT_PORT)
#define IS_DEVICE		(e->option & OPT_DEV)
#define IS_SECRET_OPT		(e->option & OPT_SECRET_OPT)
#define EXITING			(e->option & TO_EXIT)
#define EXIT_BADOPT		(1<<0)
#define EXIT_USAGE		(1<<1)
#define EXIT_FORK_ERR		(1<<2)
#define EXIT_MUTEX_ERR		(1<<3)
#define EXIT_THREAD_CREA	(1<<4)

/*
**
*/
#define DTYPE_STDDATA		1
#define DTYPE_SENDUSERKEY	2
#define DTYPE_SENDCRYPTOKEY	3

/*#define RESERVED_UP_TO	256*/

/*
**
*/
#define DUR_META_HOSTNAME	300

/*
**
*/
