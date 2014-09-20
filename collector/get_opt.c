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
#include "moon.h"
#include "mooner.h"

/*
**
*/
void			get_opt(int ac,char **av)
{
  int ch;
  
  while ((ch = getopt(ac, av, OPT_ARG)) != MINUS_ONE)
    switch (ch) {
    case CHAR_g:
      e->option |= OPT_GARBAGE;
      break;
    case CHAR_s:
      e->rmq_srv_dns_field=optarg;
      break;
    case CHAR_l:
      e->option |= OPT_LOG_FILE;
      e->log_file=optarg;
      init_log_file();
      break;
    case CHAR_v:
      e->option |= OPT_VERBOSE;
      break;
    case CHAR_y:
      e->option |= OPT_SECRET_OPT;
      break;
    case CHAR_d:
      e->option |= OPT_DEBUG;
      break;
    case CHAR_p:
      e->option |= OPT_PORT;
      e->port = atoi(optarg);
      set_filter(&(e->filter), e->port);
      break;
    case CHAR_t:
      e->synflood_ret_time=atoi(optarg);
      break;
    case CHAR_i:
      e->option |= OPT_DEV;
      e->dev=optarg;
      break;
    case CHAR_r:
      e->option |= OPT_SET_RMQ;
      e->rmqserverlist=malloc(sizeof(*e->rmqserverlist) + 1);
      memset(e->rmqserverlist,0,sizeof(*e->rmqserverlist) + 1);
      e->rmqserverlist[0].target=server_name(optarg);
      e->rmqserverlist[0].port=server_port(optarg);
      e->rmqserver = e->rmqserverlist;
      break;
    case CHAR_u:
      usage();
      mooner_exit(EXIT_USAGE);
      break;
    case CHAR_a:
      e->max_tab_size=atoi(optarg);
      if (e->msg_tab)
	free(e->msg_tab);
      e->msg_tab = init_msg_tab(e->max_tab_size);
    case CHAR_k:
      e->option |= OPT_CRYPTO;
      e->public_key_file=optarg;
      break;
    case CHAR_U:
      e->client_login=optarg;
      break;
    case CHAR_P:
      e->client_pass=optarg;
      break;
    case CHAR_T:
      e->client_login=optarg;
      e->client_pass=e->client_login;
      e->rmq_exchange=e->client_login;
      break;
    case CHAR_q:
      e->rmq_routingkey=optarg;
      break;
    case CHAR_e:
      e->rmq_exchange=optarg;
      break;
    case CHAR_V:
      printf(MY_RELEASE);
      printf("\n");
      exit(0);
    default:
      usage();
      mooner_exit(EXIT_BADOPT);
    }
}

char				*server_name(char *str)
{
  char				*ret;
  int				n;
  char				*tmp;

  tmp=ret=str;
  n=0;
  while (*str)
    {
      if (*str == CHAR_COMMA)
	break;
      else
	n++;
      str++;
    }
  if (!*str)
    return (ret);
  if (n==0)
    usage();
  ret=malloc(n+1);
  memcpy(ret, tmp, n);
  ret[n]=ZERO;

  return(ret);
}

int				server_port(char *str)
{
  while (*str && *str != CHAR_COMMA)
    str++;
  if (*str == CHAR_COMMA)
    return atoi(str+1);
  else
    return (DEF_RMQ_PORT);
}
