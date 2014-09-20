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
#include "moontop.h"

/*
**
*/
int				main()
{
  t_file_desc			file_list[]=
    {
      {"Average packet/minute last 01 minute	: ", NBR_SEND_01M_FILE, average_last_01_minute},
      {"Average packet/minute last 05 minute	: ", NBR_SEND_05M_FILE, average_last_05_minute},
      {"Average packet/minute last 15 minute	: ", NBR_SEND_15M_FILE, average_last_15_minute},
      {"Max used tcp tab:			: ", MAX_TCP_TAB_USED_FILE, nothing_to_do},
      {"Nbr syn packet recieved			: ", NBR_SYN_FILE, nothing_to_do},
      {"Nbr syn/ack packet			: ", NBR_SYNACK_FILE, nothing_to_do},
      {"Nbr ack packet				: ", NBR_ACK_FILE, nothing_to_do},
      {"nbr ack packet hash filtered		: ", NBR_ACK_FILTERED, nothing_to_do},
      {"nbr ack packet not hash filtered	: ", NBR_ACK_NOT_FILTERED, nothing_to_do},
      {"nbr detected synflood			: ", NBR_TCPFLOOD_FILE, nothing_to_do},
      {0,0,0}
    };
  t_file_desc			*tmp;
  
  for (tmp=file_list ; tmp->file_desc ; tmp++)
    printf("%s %d\n",tmp->file_desc, tmp->calc_func(fnb_get_nbr_from_file(tmp->file_path)));

  return(ZERO);
}
