/*
  This file is part of CanFestival, a library implementing CanOpen
  Stack.

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
  USA
*/

/*!
** @file   lifegrd.c
** @author Edouard TISSERANT
** @date   Mon Jun  4 17:19:24 2007
**
** @brief
**
**
*/

#include <data.h>
#include "lifegrd.h"
#include "canfestival.h"
#include "dcf.h"
#include "sysdep.h"


void ConsumerHeartbeatAlarm(CO_Data* d, UNS32 id);
void ProducerHeartbeatAlarm(CO_Data* d, UNS32 id);
UNS32 OnHearbeatProducerUpdate(CO_Data* d, const indextable * unused_indextable, UNS8 unused_bSubindex);

void GuardTimeAlarm(CO_Data* d, UNS32 id);
UNS32 OnNodeGuardUpdate(CO_Data* d, const indextable * unused_indextable, UNS8 unused_bSubindex);


e_nodeState getNodeState (CO_Data* d, UNS8 nodeId)
{
  e_nodeState networkNodeState = Unknown_state;
  #if NMT_MAX_NODE_ID>0
  if(nodeId < NMT_MAX_NODE_ID)
    networkNodeState = d->NMTable[nodeId];
  #endif
  return networkNodeState;
}

/*! 
** The Consumer Timer Callback
**
** @param d
** @param id
 * @ingroup heartbeato
**/
void ConsumerHeartbeatAlarm(CO_Data* d, UNS32 id)
{
  UNS8 nodeId = (UNS8)(((d->ConsumerHeartbeatEntries[id]) & (UNS32)0x00FF0000) >> (UNS8)16);
  /*MSG_WAR(0x00, "ConsumerHearbeatAlarm", 0x00);*/

  /* timer have been notified and is now free (non periodic)*/
  /* -> avoid deleting re-assigned timer if message is received too late*/
  d->ConsumerHeartBeatTimers[id]=TIMER_NONE;
  
  /* set node state */
  d->NMTable[nodeId] = Disconnected;
  /*! call heartbeat error with NodeId */
  (*d->heartbeatError)(d, nodeId);
}

void proceedNODE_GUARD(CO_Data* d, Message* m )
{
  UNS8 nodeId = (UNS8) GET_NODE_ID((*m));

  if((m->rtr == 1) )
    /*!
    ** Notice that only the master can have sent this
    ** node guarding request
    */
    {
      /*!
      ** Receiving a NMT NodeGuarding (request of the state by the
      ** master)
      ** Only answer to the NMT NodeGuarding request, the master is
      ** not checked (not implemented)
      */
      if (nodeId == *d->bDeviceNodeId )
        {
          Message msg;
          UNS16 tmp = *d->bDeviceNodeId + 0x700;
          msg.cob_id = UNS16_LE(tmp);
          msg.len = (UNS8)0x01;
          msg.rtr = 0;
          msg.data[0] = d->nodeState;
          if (d->toggle)
            {
              msg.data[0] |= 0x80 ;
              d->toggle = 0 ;
            }
          else
            d->toggle = 1 ;
          /* send the nodeguard response. */
          MSG_WAR(0x3130, "Sending NMT Nodeguard to master, state: ", d->nodeState);
          canSend(d->canHandle,&msg );
        }

    }else{ /* Not a request CAN */
      /* The state is stored on 7 bit */
      e_nodeState newNodeState = (e_nodeState) ((*m).data[0] & 0x7F);

      MSG_WAR(0x3110, "Received NMT nodeId : ", nodeId);
      
      /*!
      ** Record node response for node guarding service
      */
      d->nodeGuardStatus[nodeId] = *d->LifeTimeFactor;

      if (d->NMTable[nodeId] != newNodeState)
      {
        (*d->post_SlaveStateChange)(d, nodeId, newNodeState);
        /* the slave's state receievd is stored in the NMTable */
        d->NMTable[nodeId] = newNodeState;
      }

      /* Boot-Up frame reception */
      if ( d->NMTable[nodeId] == Initialisation)
      {
          /*
          ** The device send the boot-up message (Initialisation)
          ** to indicate the master that it is entered in
          ** pre_operational mode
          */
          MSG_WAR(0x3100, "The NMT is a bootup from node : ", nodeId);
          /* call post SlaveBootup with NodeId */
		  (*d->post_SlaveBootup)(d, nodeId);
      }

      if( d->NMTable[nodeId] != Unknown_state ) {
        UNS8 index, ConsumerHeartBeat_nodeId ;
        for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
          {
            ConsumerHeartBeat_nodeId = (UNS8)( ((d->ConsumerHeartbeatEntries[index]) & (UNS32)0x00FF0000) >> (UNS8)16 );
            if ( nodeId == ConsumerHeartBeat_nodeId )
              {
                TIMEVAL time = ( (d->ConsumerHeartbeatEntries[index]) & (UNS32)0x0000FFFF ) ;
                /* Renew alarm for next heartbeat. */
                DelAlarm(d->ConsumerHeartBeatTimers[index]);
                d->ConsumerHeartBeatTimers[index] = SetAlarm(d, index, &ConsumerHeartbeatAlarm, MS_TO_TIMEVAL(time), 0);
              }
          }
      }
    }
}

/*! The Producer Timer Callback
**
**
** @param d
** @param id
 * @ingroup heartbeato
**/
void ProducerHeartbeatAlarm(CO_Data* d, UNS32 id)
{
  (void)id;
  if(*d->ProducerHeartBeatTime)
    {
      Message msg;
      /* Time expired, the heartbeat must be sent immediately
      ** generate the correct node-id: this is done by the offset 1792
      ** (decimal) and additionaly
      ** the node-id of this device.
      */
      UNS16 tmp = *d->bDeviceNodeId + 0x700;
      msg.cob_id = UNS16_LE(tmp);
      msg.len = (UNS8)0x01;
      msg.rtr = 0;
      msg.data[0] = d->nodeState; /* No toggle for heartbeat !*/
      /* send the heartbeat */
      MSG_WAR(0x3130, "Producing heartbeat: ", d->nodeState);
      canSend(d->canHandle,&msg );

    }else{
      d->ProducerHeartBeatTimer = DelAlarm(d->ProducerHeartBeatTimer);
    }
}

/**
 * @brief The guardTime - Timer Callback.
 * 
 * This function is called every GuardTime (OD 0x100C) ms <br>
 * On every call, a NodeGuard-Request is sent to all nodes which have a
 * node-state not equal to "Unknown" (according to NMTable). If the node has
 * not responded within the lifetime, the nodeguardError function is called and
 * the status of this node is set to "Disconnected"
 *
 * @param d 	Pointer on a CAN object data structure 
 * @param id
 * @ingroup nodeguardo
 */
void GuardTimeAlarm(CO_Data* d, UNS32 id)
{
  (void)id;
  if (*d->GuardTime) {
    UNS8 i;

    MSG_WAR(0x00, "Producing nodeguard-requests: ", 0);

    for (i = 0; i < NMT_MAX_NODE_ID; i++) {
      /** Send node guard request to all nodes except this node, if the 
      * node state is not "Unknown_state"
      */
      if (d->NMTable[i] != Unknown_state && i != *d->bDeviceNodeId) {

        /** Check if the node has confirmed the guarding request within
        * the LifeTime (GuardTime x LifeTimeFactor)
        */
        if (d->nodeGuardStatus[i] <= 0) {

          MSG_WAR(0x00, "Node Guard alarm for nodeId : ", i);

          // Call error-callback function
          if (*d->nodeguardError) {
            (*d->nodeguardError)(d, i);
          }

          // Mark node as disconnected
          d->NMTable[i] = Disconnected;

        }

        d->nodeGuardStatus[i]--;

        masterSendNMTnodeguard(d, i);

      }
    }
  } else {
    d->GuardTimeTimer = DelAlarm(d->GuardTimeTimer);
  }



}

/**
 * This function is called, if index 0x100C or 0x100D is updated to
 * restart the node-guarding service with the new parameters
 *
 * @param d 	Pointer on a CAN object data structure 
 * @param unused_indextable
 * @param unused_bSubindex
 * @ingroup nodeguardo
 */
UNS32 OnNodeGuardUpdate(CO_Data* d, const indextable * unused_indextable, UNS8 unused_bSubindex)
{
  (void)unused_indextable;
  (void)unused_bSubindex;
  nodeguardStop(d);
  nodeguardInit(d);
  return 0;
}


/*! This is called when Index 0x1017 is updated.
**
**
** @param d
** @param unused_indextable
** @param unused_bSubindex
**
** @return
 * @ingroup heartbeato
**/
UNS32 OnHeartbeatProducerUpdate(CO_Data* d, const indextable * unused_indextable, UNS8 unused_bSubindex)
{
  (void)unused_indextable;
  (void)unused_bSubindex;
  d->ProducerHeartBeatTimer = DelAlarm(d->ProducerHeartBeatTimer);
  if ( *d->ProducerHeartBeatTime )
	{
		TIMEVAL time = *d->ProducerHeartBeatTime;
		d->ProducerHeartBeatTimer = SetAlarm(d, 0, &ProducerHeartbeatAlarm, 0, MS_TO_TIMEVAL(time));
	}
  return 0;
}

void heartbeatInit(CO_Data* d)
{

  UNS8 index; /* Index to scan the table of heartbeat consumers */
  RegisterSetODentryCallBack(d, 0x1017, 0x00, &OnHeartbeatProducerUpdate);

  d->toggle = 0;

  for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
	{
		TIMEVAL time = (UNS16) ( (d->ConsumerHeartbeatEntries[index]) & (UNS32)0x0000FFFF ) ;
		if ( time )
		{
			d->ConsumerHeartBeatTimers[index] = SetAlarm(d, index, &ConsumerHeartbeatAlarm, MS_TO_TIMEVAL(time), 0);
		}
	}

  if ( *d->ProducerHeartBeatTime )
	{
		TIMEVAL time = *d->ProducerHeartBeatTime;
		d->ProducerHeartBeatTimer = SetAlarm(d, 0, &ProducerHeartbeatAlarm, MS_TO_TIMEVAL(time), MS_TO_TIMEVAL(time));
	}
}


void nodeguardInit(CO_Data* d)
{

  RegisterSetODentryCallBack(d, 0x100C, 0x00, &OnNodeGuardUpdate);
  RegisterSetODentryCallBack(d, 0x100D, 0x00, &OnNodeGuardUpdate);

  if (*d->GuardTime && *d->LifeTimeFactor) {
    UNS8 i;

    TIMEVAL time = *d->GuardTime;
    d->GuardTimeTimer = SetAlarm(d, 0, &GuardTimeAlarm, MS_TO_TIMEVAL(time), MS_TO_TIMEVAL(time));
    MSG_WAR(0x0, "GuardTime: ", time);

    for (i = 0; i < NMT_MAX_NODE_ID; i++) {
      /** Set initial value for the nodes */
      if (d->NMTable[i] != Unknown_state && i != *d->bDeviceNodeId) { 
        d->nodeGuardStatus[i] = *d->LifeTimeFactor;
      }
    }

    MSG_WAR(0x0, "Timer for node-guarding startet", 0);
  }

}

void heartbeatStop(CO_Data* d)
{
  UNS8 index;
  for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
    {
      d->ConsumerHeartBeatTimers[index] = DelAlarm(d->ConsumerHeartBeatTimers[index]);
    }

  d->ProducerHeartBeatTimer = DelAlarm(d->ProducerHeartBeatTimer);
}

void nodeguardStop(CO_Data* d)
{
  d->GuardTimeTimer = DelAlarm(d->GuardTimeTimer);
}


void lifeGuardInit(CO_Data* d)
{
  heartbeatInit(d);
  nodeguardInit(d);
}


void lifeGuardStop(CO_Data* d)
{
  heartbeatStop(d);
  nodeguardStop(d);
}


void _heartbeatError(CO_Data* d, UNS8 heartbeatID){(void)d;(void)heartbeatID;}
void _post_SlaveBootup(CO_Data* d, UNS8 SlaveID){(void)d;(void)SlaveID;}
void _post_SlaveStateChange(CO_Data* d, UNS8 nodeId, e_nodeState newNodeState){(void)d;(void)nodeId;(void)newNodeState;}
void _nodeguardError(CO_Data* d, UNS8 id){(void)d;(void)id;}

