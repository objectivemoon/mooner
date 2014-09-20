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

typedef struct				s_file_desc {
  char					*file_desc;
  char					*file_path;
  int					(*calc_func)(int nbr);
}					t_file_desc;

/*
**
*/
int					main();
int					average_last_01_minute(int);
int					average_last_05_minute(int);
int					average_last_15_minute(int);
int					nothing_to_do(int);
