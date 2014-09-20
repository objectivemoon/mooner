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
void				do_signal_sigusr2()
{
  send_log(LOG_INFO, "[39] reload RMQ Server List DNS\n");
  set_rmq_server_list();
}

void				signal_prep()
{
  struct sigaction	sig_usr2;
  struct sigaction	sig_int;
  struct sigaction	sig_hup;
  
  sig_usr2.sa_handler = do_signal_sigusr2;
  sigemptyset (&sig_usr2.sa_mask);
  sig_usr2.sa_flags = 0;
  sigaction (SIGUSR2, &sig_usr2,NULL);

  sig_int.sa_handler = mooner_exit;
  sigemptyset (&sig_int.sa_mask);
  sig_int.sa_flags = 0;
  sigaction (SIGINT, &sig_int,NULL);

  sig_hup.sa_handler = flush_log_file;
  sigemptyset (&sig_hup.sa_mask);
  sig_hup.sa_flags = 0;
  sigaction (SIGHUP, &sig_hup,NULL);
}

