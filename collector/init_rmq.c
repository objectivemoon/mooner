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

#define SUMMARY_EVERY_US 1000000

/*
**
*/

int					init_rmq(t_server *server)
{
  amqp_socket_t				*socket;
  int					status;
  amqp_rpc_reply_t			res;
  amqp_connection_state_t		conn;
  amqp_connection_state_t		tmp;

  socket=NULL;
  tmp=NULL;
  send_log(LOG_DEBUG, "[16] Try to init broker : P%4d W%4d %6d %s\n",server->priority, server->weight, server->port, server->target); 
  conn = amqp_new_connection();
  socket = amqp_tcp_socket_new(conn);
  if (!socket) {
    if (IS_VERBOSE)
      send_log(LOG_DEBUG, "[17] Can't create TCP socket\n");
    return (0);
  }
  status = amqp_socket_open(socket, (server->target), (server->port));
  if (status) {
    if (IS_VERBOSE)
      send_log(LOG_DEBUG, "[18] Can't open TCP socket to server [%s]\n",server->target);
    return (0);
  }
  res=amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, 
		 (e->client_login ? e->client_login : RMQ_FNB_USER), 
		 (e->client_pass ? e->client_pass:  RMQ_FNB_PASS));
  if (res.reply_type != AMQP_RESPONSE_NORMAL) {
    amqp_response2logger(res,"rmq login");
    send_log(LOG_ERR, "[19] Check that you associate your collector token [%s] with your login on http://houston.objectivemoon.io\n",(e->client_login ? e->client_login : RMQ_FNB_USER));
    return (0);
  }
  amqp_channel_open(conn, 1);
  res = amqp_get_rpc_reply(conn);
  if (res.reply_type != AMQP_RESPONSE_NORMAL)
    {
      amqp_response2logger(res,"Opening channel");
      close_rmq_channel(&conn);
      return (0);
    }
  if (e->rmq_conn)
    tmp=e->rmq_conn;
  e->rmq_conn = conn;
  if (tmp)
    close_rmq_channel(&tmp);

  return (1);
}

void				amqp_response2logger(amqp_rpc_reply_t x, char *context)
{
  switch (x.reply_type) {
  case AMQP_RESPONSE_NONE:
    send_log(LOG_WARNING, "[20] %s: missing RPC reply type!\n", context);
    break;

  case AMQP_RESPONSE_LIBRARY_EXCEPTION:
    send_log(LOG_ERR, "[21] %s: %s\n", context, amqp_error_string2(x.library_error));
    break;

  case AMQP_RESPONSE_SERVER_EXCEPTION:
    switch (x.reply.id) {
    case AMQP_CONNECTION_CLOSE_METHOD: {
      amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;
      send_log(LOG_ERR, "[22] %s: server connection error %d, message: %.*s\n",
              context,
              m->reply_code,
              (int) m->reply_text.len, (char *) m->reply_text.bytes);
      break;
    }
    case AMQP_CHANNEL_CLOSE_METHOD: {
      amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
      send_log(LOG_ERR, "[23] %s: server channel error %d, message: %.*s\n",
              context,
              m->reply_code,
              (int) m->reply_text.len, (char *) m->reply_text.bytes);
      break;
    }
    default:
      send_log(LOG_ERR, "[24] %s: unknown server error, method id 0x%08X\n", context, x.reply.id);
      break;
    }
    break;
  case AMQP_RESPONSE_NORMAL:
    return;
    break;
  }
}

int				close_rmq_channel(amqp_connection_state_t *conn)
{
  amqp_rpc_reply_t		res;
  
  res=amqp_channel_close(*conn, 1, AMQP_REPLY_SUCCESS);
  if (res.reply_type != AMQP_RESPONSE_NORMAL)
    amqp_response2logger(res,"Closing channel");
  res=amqp_connection_close(*conn, AMQP_REPLY_SUCCESS);
  if (res.reply_type != AMQP_RESPONSE_NORMAL)
    amqp_response2logger(res, "Closing connection");
  if (amqp_destroy_connection(*conn))
    send_log(LOG_ERR, "[25] Ending connection\n");
  send_log(LOG_NOTICE, "[26] Connection as been closed\n");
  *conn = ZERO;
  sleep(TWO);

  return (0);
}
