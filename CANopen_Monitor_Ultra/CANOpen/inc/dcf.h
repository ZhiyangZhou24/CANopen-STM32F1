/*
This file is part of CanFestival, a library implementing CanOpen Stack. 

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "data.h"

#define DCF_STATUS_INIT         0
#define DCF_STATUS_READ_CHECK   1
#define DCF_STATUS_WRITE        2
#define DCF_STATUS_SAVED        3
#define DCF_STATUS_VERIF_OK     4

UNS8 init_consise_dcf(CO_Data* d, UNS8 nodeId);
UNS8 check_and_start_node(CO_Data* d, UNS8 nodeId);
