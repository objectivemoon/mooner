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
#include <sys/time.h>
#include "moon.h"
#include "mooner.h"


/*
**
*/

char			*print_it(t_msg *p, int pos)
{
  char			ip_src[INET_ADDRSTRLEN];
  char			ip_dst[INET_ADDRSTRLEN];

  printf("---------------------------------------------\n");
  if (p->c_syn_tv_sec)
    {
      inet_ntop(AF_INET, &(p->ip_src), ip_src, INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &(p->ip_dst), ip_dst, INET_ADDRSTRLEN);
      printf("SYN     %4d :\n",pos);
      printf("    src->dst : %15s -> %15s\n", ip_src,ip_dst);
      printf("         sec : %d\n", (int)p->c_syn_tv_sec);
      printf("        usec : %d\n", (int)p->c_syn_tv_usec);
      printf("         seq : %u\n", ntohl(p->c_syn));
    }
  if (p->s_tv_sec)
    {
      printf("SYN+ACK :\n");
      printf("         sec : %d\n", (int)p->s_tv_sec);
      printf("        usec : %d\n", (int)p->s_tv_usec);
      printf("         seq : %u\n", ntohl(p->s_syn));
    }
  if (p->c_ack_tv_sec)
    {
      printf("ACK     :\n");
      printf("         sec : %d\n", (int)p->c_ack_tv_sec);
      printf("        usec : %d\n", (int)p->c_ack_tv_usec);
    }
  return (0);
}
