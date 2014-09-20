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

#ifndef __FNB_IP
#  define __FNB_IP


typedef struct			s_ip {
  u_char			ip_vhl;
  u_char			ip_tos;
  u_short			ip_len;
  u_short			ip_id;
  u_short			ip_off;
#define IP_RF			0x8000
#define IP_DF			0x4000
#define IP_MF			0x2000
#define IP_OFFMASK		0x1fff
  u_char			ip_ttl;
  u_char			ip_p;
  u_short			ip_sum;
  struct in_addr		ip_src;
  struct in_addr		ip_dst;
}				t_ip;

#define IP_HL(ip)		(((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)		(((ip)->ip_vhl) >> 4)
typedef struct			s_tcp {
  u_short			th_sport;
  u_short			th_dport;
  u_int32_t			th_seq;
  u_int32_t			th_ack;
  u_char			th_offx2;
#define TH_OFF(th)		(((th)->th_offx2 & 0xf0) >> 4)
  u_char			th_flags;
#define TH_FIN			0x01
#define TH_SYN			0x02
#define TH_RST			0x04
#define TH_PUSH			0x08
#define TH_ACK			0x10
#define TH_URG			0x20
#define TH_ECE			0x40
#define TH_CWR			0x80
#define TH_FLAGS		(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
  u_short			th_win;
  u_short			th_sum;
  u_short			th_urp;
}				t_tcp;
#endif
