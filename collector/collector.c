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
int			main(int ac, char **av)
{
  pcap_t		*handle;
  char			errbuf[PCAP_ERRBUF_SIZE];
  struct bpf_program	fp;
  char			filter_exp[FILTER_MAX_SIZE];
  bpf_u_int32		mask;
  bpf_u_int32		net;
  u_char		*packet;
  pthread_t		scheduler;

  signal_prep();
  packet=0;
  e=init_env();
  get_opt(ac, av);
#ifndef __APPLE__
  if (!(e->option & OPT_VERBOSE))
    daemon(ZERO,ZERO);
  if (e->option & OPT_GARBAGE)
    new_process(ac, av);
#endif
  write_pid_number(e->pidfile_path);
  if (IS_VERBOSE)
    {
      send_log(LOG_INFO, "[1] verbose : on\n");
      send_log(LOG_DEBUG, "[2] Preallocate msg mem size (alignement not include) : %d x %u = %u\n",
	       e->max_tab_size,sizeof(t_msg),e->max_tab_size * (sizeof(t_msg)));
      send_log(LOG_DEBUG, "[3] device : [%s]\n",e->dev);
      send_log(LOG_DEBUG, "[4] filter : [%s]\n",e->filter);
    }
  if (pthread_mutex_init(&gl_lock_set_rmq, NULL) != 0)
    {
      send_log(LOG_CRIT, "[5] Mutex init failed\n");
      mooner_exit(EXIT_MUTEX_ERR);
    }
  set_rmq_server_list();
  if (pthread_mutex_init(&gl_lock_garbage, NULL) != 0)
    {
      send_log(LOG_CRIT, "[5] Mutex init failed\n");
      mooner_exit(EXIT_MUTEX_ERR);
    }
  if (pthread_mutex_init(&gl_lock_send_data, NULL) != 0)
    {
      send_log(LOG_CRIT, "[44] Mutex init failed\n");
      mooner_exit(EXIT_MUTEX_ERR);
    }
  if (pthread_create(&scheduler,NULL,th_scheduler,NULL) != ZERO)
    {
      send_log(LOG_CRIT, "[6] Can't create Thread for scheduling\n");
      mooner_exit(EXIT_THREAD_CREA);
    }
  if (!IS_DEVICE)
    e->dev = pcap_lookupdev(errbuf);
  if (e->dev== NULL) {
    send_log(LOG_ERR, "[7] Couldn't find default dev: %s\n", errbuf);
    mooner_exit(2);
  }
  if (pcap_lookupnet(e->dev, &net, &mask, errbuf) == -1) {
    send_log(LOG_ERR, "[8] Couldn't get netmask for dev %s: %s\n", e->dev, errbuf);
    net = 0;
    mask = 0;
  }
  handle = pcap_open_live(e->dev, BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
    send_log(LOG_ERR, "[9] Couldn't open dev %s: %s\n", e->dev, errbuf);
    mooner_exit(2);
  }
  if (pcap_compile(handle, &fp, e->filter, 0, net) == -1) {
    send_log(LOG_ERR, "[10] Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    mooner_exit(2);
  }
  if (pcap_setfilter(handle, &fp) == -1) {
    send_log(LOG_ERR, "[11] Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    mooner_exit(2);
  }
#ifdef DMALLOC
  dmalloc_debug_setup("debug=0x4f47d03,log=logfile");
#endif
  while(!(EXITING))
    {
      if (IS_VERBOSE)
	send_log(LOG_DEBUG, "[12] entering loop\n");
      pcap_loop(handle, -1, handle_packet, packet);
    }
  pcap_close(handle);
  mooner_exit(ZERO);
  return(ZERO);
}
