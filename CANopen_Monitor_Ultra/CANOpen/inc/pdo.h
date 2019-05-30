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

/** @defgroup pdo Process Data Object (PDO)
 *  PDO is a communication object defined by the DPO communication parameter and PDA mapping parameter objects.
 *  It is an uncomfirmed communication service without protocol overhead.
 *  @ingroup comobj
 */
 
#ifndef __pdo_h__
#define __pdo_h__

#include <applicfg.h>
#include <def.h>

#include "can.h"

typedef struct struct_s_PDO_status s_PDO_status;

#include "data.h"

/* Handler for RxPDO event timers : empty function that user can overload */
void _RxPDO_EventTimers_Handler(CO_Data *d, UNS32 pdoNum);

/* Status of the TPDO : */
#define PDO_INHIBITED 0x01
#define PDO_RTR_SYNC_READY 0x01

/** The PDO structure */
struct struct_s_PDO_status {
  UNS8 transmit_type_parameter;
  TIMER_HANDLE event_timer;
  TIMER_HANDLE inhibit_timer;
  Message last_message;
};

#define s_PDO_status_Initializer {0, TIMER_NONE, TIMER_NONE, Message_Initializer}

/** definitions of the different types of PDOs' transmission
 * 
 * SYNCHRO(n) means that the PDO will be transmited every n SYNC signal.
 */
#define TRANS_EVERY_N_SYNC(n) (n) /*n = 1 to 240 */
#define TRANS_SYNC_ACYCLIC    0    /* Trans after reception of n SYNC. n = 1 to 240 */
#define TRANS_SYNC_MIN        1    /* Trans after reception of n SYNC. n = 1 to 240 */
#define TRANS_SYNC_MAX        240  /* Trans after reception of n SYNC. n = 1 to 240 */
#define TRANS_RTR_SYNC        252  /* Transmission on request */
#define TRANS_RTR             253  /* Transmission on request */
#define TRANS_EVENT_SPECIFIC  254  /* Transmission on event */
#define TRANS_EVENT_PROFILE   255  /* Transmission on event */

/**
  * @brief复制所有要在process_var中传输的数据
  *准备要发送的索引中定义的PDO
  * * pwCobId：返回cobid的值。 （分指数1）
  * @param * d CAN对象数据结构的指针
  * @param numPdo PDO号码
  * @param * pdo指向CAN消息结构的指针
  * @return 0或0xFF如果错误。
 */
UNS8 buildPDO(CO_Data* d, UNS8 numPdo, Message *pdo);

/**
? * @ingroup pdo
? * @brief将网络上的PDO请求帧发送给从站。
? * @param * d CAN对象数据结构的指针
? * @param RPDOIndex接收PDO的索引
? * @return
? *  - 成功返回CanFestival文件描述符。
? *  - 如果未找到RPDO索引，则返回0xFF。
?
? * @return 0xFF如果错误，其他成功。
?*/
UNS8 sendPDOrequest( CO_Data* d, UNS16 RPDOIndex );

/**
? * @brief计算PDO帧接收
? * bus_id取决于硬件
? * @param * d CAN对象数据结构的指针
? * @param * m指示CAN消息结构
? * @return 0xFF如果错误，否则返回0
?*/
UNS8 proceedPDO (CO_Data* d, Message *m);

/** 
 * @brief Used by the application to signal changes in process data
 * that could be mapped to some TPDO.
 * This do not necessarily imply PDO emission.
 * Function iterates on all TPDO and look TPDO transmit 
 * type and content change before sending it.    
 * @param *d Pointer on a CAN object data structure
 */
UNS8 sendPDOevent (CO_Data* d);
UNS8 sendOnePDOevent (CO_Data* d, UNS8 pdoNum);

/** 
 * @brief Enable a PDO by setting to 0 the bit 32 of the COB-ID parameter
 * @param *d Pointer on a CAN object data structure
 * @param pdoNum The PDO number
 */
void PDOEnable (CO_Data * d, UNS8 pdoNum);

/** 
 * @brief Disable a PDO by setting to 1 the bit 32 of the COB-ID parameter
 * @param *d Pointer on a CAN object data structure
 * @param pdoNum The PDO number
 */
void PDODisable (CO_Data * d, UNS8 pdoNum);

/** 
 * @ingroup pdo
 * @brief Function iterates on all TPDO and look TPDO transmit 
 * type and content change before sending it.
 * @param *d Pointer on a CAN object data structure
 * @param isSyncEvent
 */
UNS8 _sendPDOevent(CO_Data* d, UNS8 isSyncEvent);

/** 
 * @brief Initialize PDO feature 
 * @param *d Pointer on a CAN object data structure
 */
void PDOInit(CO_Data* d);

/** 
 * @brief Stop PDO feature 
 * @param *d Pointer on a CAN object data structure
 */
void PDOStop(CO_Data* d);

/** 
 * @ingroup pdo
 * @brief Set timer for PDO event
 * @param *d Pointer on a CAN object data structure
 * @param pdoNum The PDO number
 */
void PDOEventTimerAlarm(CO_Data* d, UNS32 pdoNum);

/** 
 * @ingroup pdo
 * @brief Inhibit timer for PDO event
 * @param *d Pointer on a CAN object data structure
 * @param pdoNum The PDO number
 */
void PDOInhibitTimerAlarm(CO_Data* d, UNS32 pdoNum);

/* copy bit per bit in little endian */
void CopyBits(UNS8 NbBits, UNS8* SrcByteIndex, UNS8 SrcBitIndex, UNS8 SrcBigEndian, UNS8* DestByteIndex, UNS8 DestBitIndex, UNS8 DestBigEndian);
#endif
