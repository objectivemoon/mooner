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

int			average_last_01_minute(int nbr)
{
  return (nbr/ONE_MINUTE);
}

int			average_last_05_minute(int nbr)
{
  return (nbr/FIVE_MINUTES);
}

int			average_last_15_minute(int nbr)
{
  return (nbr/FIFTEEN_MINUTES);
}

int			nothing_to_do(int nbr)
{
  return (nbr);
}
