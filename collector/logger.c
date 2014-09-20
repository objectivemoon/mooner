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

void				send_log(int loglevel, char *format, ...)
{
  va_list			ap;
  t_logl			*tmp;
  
  if ((e->option & OPT_VERBOSE) || (e->option & OPT_LOG_FILE))
    {
      t_logl			msg[] = {
	{LOG_EMERG,   "!!!! EMERGENCY : "},
	{LOG_ALERT,   "!    ALERT     : "},
	{LOG_CRIT,    "!!!  CRITICAL  : "},
	{LOG_ERR,     "!!   ERROR     : "},
	{LOG_WARNING, "---- WARNING   : "},
	{LOG_NOTICE,  "---  NOTICE    : "},
	{LOG_INFO,    "++   INFO      : "},
	{LOG_DEBUG,   "+    DEBUG     : "},
	{0,0}
      };
      if (e->option & OPT_LOG_FILE)
	{
	  char			date[42];
	  time_t		t;

	  if (!gl_log_file)
	    return;
	  t=time(NULL);
	  strftime(date, 42,"%D %H:%M:%S",localtime(&t));
	  fprintf(gl_log_file,"%s ",date);
	}
      for (tmp=msg ; tmp->msg ; tmp++)
	if (loglevel == tmp->loglevel)
	  {
	    if (e->option & OPT_LOG_FILE)
	      fprintf(gl_log_file,"%s",tmp->msg);
	    else
	      fprintf(stderr,"%s",tmp->msg);
	    break;
	  }
      va_start(ap, format);
      if (e->option & OPT_LOG_FILE)
	vfprintf(gl_log_file, format, ap);
      else
	vfprintf(stderr, format, ap);
      va_end(ap);
      flush_log_file(0);
    }
  else
    {
      openlog (FNB_COLLECTOR_NAME, LOG_PID , LOG_DAEMON);
      va_start(ap, format);
      vsyslog (loglevel,format,ap);
      va_end(ap);
      closelog ();
    }
}

void				init_log_file()
{
  if (e->option & OPT_LOG_FILE)
    {
      gl_log_file=fopen(e->log_file, "a");
      if (gl_log_file == NULL)
	{
	  fprintf(stderr,"[45] can't open log file [%s]\n", e->log_file);
	  exit(1);
	}
    }
}

void				close_log_file()
{
  FILE				*tmp;

  if (e->option & OPT_LOG_FILE)
    {
      tmp=gl_log_file;
      gl_log_file=0;
      fclose(tmp);
    }
}

void				flush_log_file(int I_do_not_use_this_var __attribute__ ((unused)))
{
  if ((e->option & OPT_LOG_FILE) && (gl_log_file != NULL))
    fflush(gl_log_file);
}
