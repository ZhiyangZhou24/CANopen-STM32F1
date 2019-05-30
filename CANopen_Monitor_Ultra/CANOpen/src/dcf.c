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


/**
** @file   dcf.c
** @author Edouard TISSERANT and Francis DUPIN
** @date   Mon Jun  4 17:06:12 2007
**
** @brief EXEMPLE OF SOMMARY
**
**
*/


#include "data.h"
#include "sysdep.h"
#include "dcf.h"

typedef struct {
    UNS16 Index;
    UNS8 Subindex;
    UNS32 Size;
    UNS8 *Data;
} dcf_entry_t;

void SaveNode(CO_Data* d, UNS8 nodeId);
static UNS8 read_consise_dcf_next_entry(CO_Data* d, UNS8 nodeId);
static UNS8 write_consise_dcf_next_entry(CO_Data* d, UNS8 nodeId);
UNS8 init_consise_dcf(CO_Data* d,UNS8 nodeId);


#ifdef _MSC_VER
#define inline _inline
#endif  /* _MSC_VER */


void start_node(CO_Data* d, UNS8 nodeId){
    /* Ask slave node to go in operational mode */
    masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
    d->NMTable[nodeId] = Connecting;
}

/**
* @brief Function to be called from post_SlaveBootup
* for starting the configuration manager
*
* @param *d Pointer on a CAN object data structure
* @param nodeId Id of the slave node
* @return 0: configuration manager busy
*         1: nothing to check, node started
*         2: dcf check started
*/
UNS8 check_and_start_node(CO_Data* d, UNS8 nodeId)
{   
    if(d->dcf_status != DCF_STATUS_INIT)
        return 0;
    /* Set the first SDO client as available */
    if(d->firstIndex->SDO_CLT)
        *(UNS8*) d->objdict[d->firstIndex->SDO_CLT].pSubindex[3].pObject = 0;
    else
        return 3;
    if((init_consise_dcf(d, nodeId) == 0) || (read_consise_dcf_next_entry(d, nodeId) == 0)){
        start_node(d, nodeId);
        return 1;
    }
    d->dcf_status = DCF_STATUS_READ_CHECK;
    return 2;
}

/**
** @brief Start the nodeId slave and look for other nodes waiting to be started 
**        If nodeId is 0 the start node is not done
** @param d
** @param nodeId
*/
void start_and_seek_node(CO_Data* d, UNS8 nodeId){
   UNS8 node;
   if(nodeId)
       start_node(d,nodeId);
   for(node = 0 ; node<NMT_MAX_NODE_ID ; node++){
       if(d->NMTable[node] != Initialisation)
           continue;
       if(check_and_start_node(d, node) == 2)
           return;
   }
   d->dcf_status = DCF_STATUS_INIT;
}

/**
**
**
** @param d
** @param nodeId
*/
static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
    UNS32 abortCode = 0;
    UNS8 buf[4], match = 0;
    UNS32 size=4;
    if(d->dcf_status == DCF_STATUS_READ_CHECK){
        if(getReadResultNetworkDict (d, nodeId, buf, &size, &abortCode) != SDO_FINISHED)
            goto dcferror;
        /* Check if data received match the DCF */
        if(size == d->dcf_size){
            match = 1;
            while(size--)
                if(buf[size] != d->dcf_data[size])
                    match = 0;
        }
        if(match) {
            if(read_consise_dcf_next_entry(d, nodeId) == 0){
                start_and_seek_node(d, nodeId);
            }
        }
        else { /* Data received does not match : start rewriting all */
            if((init_consise_dcf(d, nodeId) == 0) || (write_consise_dcf_next_entry(d, nodeId) == 0))
                goto dcferror;                
            d->dcf_status = DCF_STATUS_WRITE;
        }
    }
    else if(d->dcf_status == DCF_STATUS_WRITE){
        if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
            goto dcferror;
        if(write_consise_dcf_next_entry(d, nodeId) == 0){
#ifdef DCF_SAVE_NODE
            SaveNode(d, nodeId);
            d->dcf_status = DCF_STATUS_SAVED;
#else //DCF_SAVE_NODE
            d->dcf_status = DCF_STATUS_INIT;
           start_and_seek_node(d,nodeId);
#endif //DCF_SAVE_NODE
        }
    }
    else if(d->dcf_status == DCF_STATUS_SAVED){
        if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
            goto dcferror;
        masterSendNMTstateChange (d, nodeId, NMT_Reset_Node);
        d->dcf_status = DCF_STATUS_INIT;
        d->NMTable[nodeId] = Unknown_state;
    }
    return;
dcferror:
    MSG_ERR(0x1A01, "SDO error in consise DCF", abortCode);
    MSG_WAR(0x2A02, "slave node : ", nodeId);
	resetClientSDOLineFromNodeId(d, nodeId);
    d->NMTable[nodeId] = Unknown_state;
    d->dcf_status = DCF_STATUS_INIT;
    start_and_seek_node(d,0);
}

/**
* @brief Init the consise dcf in CO_Data for nodeId
*
* @param *d Pointer on a CAN object data structure
* @param nodeId Id of the slave node
* @return 1: dcf check started
*         0: nothing to do
*/
UNS8 init_consise_dcf(CO_Data* d,UNS8 nodeId)
{
    /* Fetch DCF OD entry */
    UNS32 errorCode;
    UNS8* dcf;
    d->dcf_odentry = (*d->scanIndexOD)(d, 0x1F22, &errorCode);
    /* If DCF entry do not exist... Nothing to do.*/
    if (errorCode != OD_SUCCESSFUL) goto DCF_finish;
    /* Fix DCF table overflow */
    if(nodeId > d->dcf_odentry->bSubCount) goto DCF_finish;
    /* If DCF empty... Nothing to do */
    if(! d->dcf_odentry->pSubindex[nodeId].size) goto DCF_finish;
    //dcf = *(UNS8**)d->dcf_odentry->pSubindex[nodeId].pObject;
    dcf = (UNS8*)d->dcf_odentry->pSubindex[nodeId].pObject;
    d->dcf_cursor = dcf + 4;
    d->dcf_entries_count = 0;
    d->dcf_status = DCF_STATUS_INIT;
    return 1;
    DCF_finish:
    return 0;
}

UNS8 get_next_DCF_data(CO_Data* d, dcf_entry_t *dcf_entry, UNS8 nodeId)
{
  UNS8* dcfend;
  UNS32 nb_entries;
  UNS32 szData;
  UNS8* dcf;
  if(!d->dcf_odentry)
     return 0;
  if(nodeId > d->dcf_odentry->bSubCount)
     return 0;
  szData = d->dcf_odentry->pSubindex[nodeId].size;
  //dcf = *(UNS8**)d->dcf_odentry->pSubindex[nodeId].pObject;
  dcf = (UNS8*)d->dcf_odentry->pSubindex[nodeId].pObject;
  nb_entries = UNS32_LE(*((UNS32*)dcf));
  dcfend = dcf + szData;
  if((UNS8*)d->dcf_cursor + 7 < (UNS8*)dcfend && d->dcf_entries_count < nb_entries){
    /* DCF data may not be 32/16b aligned, 
    * we cannot directly dereference d->dcf_cursor 
    * as UNS16 or UNS32 
    * Do it byte per byte taking care on endianess*/
#ifdef CANOPEN_BIG_ENDIAN
    dcf_entry->Index = *(d->dcf_cursor++) << 8 | 
     	               *(d->dcf_cursor++);
#else
    memcpy(&dcf_entry->Index, d->dcf_cursor,2);
       	d->dcf_cursor+=2;
#endif
    dcf_entry->Subindex = *(d->dcf_cursor++);
#ifdef CANOPEN_BIG_ENDIAN
    dcf_entry->Size = *(d->dcf_cursor++) << 24 | 
     	              *(d->dcf_cursor++) << 16 | 
        	          *(d->dcf_cursor++) << 8 | 
        	          *(d->dcf_cursor++);
#else
    memcpy(&dcf_entry->Size, d->dcf_cursor,4);
    d->dcf_cursor+=4;
#endif
    d->dcf_data = dcf_entry->Data = d->dcf_cursor;
    d->dcf_size = dcf_entry->Size;
    d->dcf_cursor += dcf_entry->Size;
    d->dcf_entries_count++;
    return 1;
  }
  return 0;
}

static UNS8 write_consise_dcf_next_entry(CO_Data* d, UNS8 nodeId)
{
    UNS8 Ret;
    dcf_entry_t dcf_entry;
    if(!get_next_DCF_data(d, &dcf_entry, nodeId))
        return 0;
    Ret = writeNetworkDictCallBackAI(d, /* CO_Data* d*/
                    nodeId, /* UNS8 nodeId*/
                    dcf_entry.Index, /* UNS16 index*/
                    dcf_entry.Subindex, /* UNS8 subindex*/
                    (UNS8)dcf_entry.Size, /* UNS8 count*/
                    0, /* UNS8 dataType*/
                    dcf_entry.Data,/* void *data*/
                    CheckSDOAndContinue,/* Callback*/
                    0,   /* no endianize*/
                    0); /* no block mode */
    if(Ret) {
        MSG_ERR(0x1A02,"Error writeNetworkDictCallBackAI",Ret);
    }
    return 1;
}

static UNS8 read_consise_dcf_next_entry(CO_Data* d, UNS8 nodeId)
{
    UNS8 Ret;
    dcf_entry_t dcf_entry;
    if(!get_next_DCF_data(d, &dcf_entry, nodeId))
        return 0;
    Ret = readNetworkDictCallbackAI(d, /* CO_Data* d*/
                   nodeId, /* UNS8 nodeId*/
                   dcf_entry.Index, /* UNS16 index*/
                   dcf_entry.Subindex, /* UNS8 subindex*/
                   0, /* UNS8 dataType*/
                   CheckSDOAndContinue,/* Callback*/
                   0); /* no block mode */
    if(Ret) {
        MSG_ERR(0x1A03,"Error readNetworkDictCallbackAI",Ret);
    }
    return 1;
}

void SaveNode(CO_Data* d, UNS8 nodeId)
{
    UNS8 Ret;
    UNS32 data=0x65766173;
    Ret = writeNetworkDictCallBackAI(d, /* CO_Data* d*/
                    nodeId, /* UNS8 nodeId*/
                    0x1010, /* UNS16 index*/
                    1, /* UNS8 subindex*/
                    4, /* UNS8 count*/
                    0, /* UNS8 dataType*/
                    (void *)&data,/* void *data*/
                    CheckSDOAndContinue,/* Callback*/
                    0,   /* no endianize*/
                    0); /* no block mode */
    if(Ret) {
        MSG_ERR(0x1A04,"Error writeNetworkDictCallBackAI",Ret);
    }
}
