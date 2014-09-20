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
char				*get_my_hostname()
{
  char				*myhostname;

  myhostname=malloc(HOSTNAME_MAX_SIZE);
  gethostname(myhostname, HOSTNAME_MAX_SIZE);
  return(myhostname);
}

void				*check_hostname_change(void *n)
{
  char			*tmp;
  
  tmp=get_my_hostname();
  if (strcmp(tmp,e->myhostname))
    {
      char		*tmp1;

      send_log(LOG_NOTICE, "[14] hostname as been change resending [%s] -> [%s]", e->myhostname, tmp);
      tmp1=e->myhostname;
      e->myhostname=tmp;
      free(tmp1);
      send_metadata_to_rmq(n);
    }
  else
    free(tmp);

  return n;
}
