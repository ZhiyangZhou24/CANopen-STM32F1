#include "canopen_app.h"
#include "canopen_drv.h"
#include "Master.h"
#include "objacces.h"

UNS8 id;
void heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	id = heartbeatID;
}


void CANOpen_Node_Init(void)
{
  UNS8 nodeID = 0x00;  //节点ID
	UNS8 Slave_node_ID=1;
	UNS32 i=0;
	UNS32 Config_Code;
	UNS16 index;
	UNS8 subindex;	
	UNS32 size = sizeof(UNS32); 
	//REAL32 *p;
  setNodeId(&Master_Data, nodeID);
  setState(&Master_Data, Initialisation);
  setState(&Master_Data, Operational);
  /*主站上电之后，让网络中节点0x01启动*/
	for(i=0;i<20;i++)
	{
		masterSendNMTstateChange(&Master_Data, Slave_node_ID, NMT_Start_Node);
		Slave_node_ID++;
		delay_ms(10);
	}

	/*Transmit PDO  Parameter*/
	Config_Code = 0x181;
	index=0x1800;subindex=1;
	for(index=0x1800;index<0x1814;index++)
	{
		writeLocalDict( &Master_Data, /*CO_Data* d*/
										index,       /*UNS16 index*/
										subindex,         /*UNS8 subind*/ 
										&Config_Code,  /*void * pSourceData,*/ 
										&size,        /* UNS8 * pExpectedSize*/
										RW);          /* UNS8 checkAccess */
		Config_Code++;
	}
	
	/*Transmit PDO  Mapping*/
	Config_Code = 0x20000108;
	index=0x1A00;subindex=1;i=1;
	for(index=0x1A00;index<0x1A14;index++)
	{
		Config_Code =0x20000008|(i++)<<8;
		writeLocalDict( &Master_Data, /*CO_Data* d*/
										index,       /*UNS16 index*/
										subindex,         /*UNS8 subind*/ 
										&Config_Code,  /*void * pSourceData,*/ 
										&size,        /* UNS8 * pExpectedSize*/
										RW);          /* UNS8 checkAccess */
		//subindex++;
	}
	
	/*Receive PDO  Parameter*/
	Config_Code = 0x281;
	index=0x1400;subindex=1;
	for(index=0x1400;index<0x1414;index++)
	{
		writeLocalDict( &Master_Data, /*CO_Data* d*/
										index,       /*UNS16 index*/
										subindex,         /*UNS8 subind*/ 
										&Config_Code,  /*void * pSourceData,*/ 
										&size,        /* UNS8 * pExpectedSize*/
										RW);          /* UNS8 checkAccess */
		Config_Code++;
	}
	
	/*Receive PDO  Mapping*/
	Config_Code = 0x20010020;
	index=0x1600;subindex=1;i=1;
	for(index=0x1600;index<0x1614;index++)
	{
		Config_Code =0x20010020|(i++)<<8;
		writeLocalDict( &Master_Data, /*CO_Data* d*/
										index,       /*UNS16 index*/
										subindex,         /*UNS8 subind*/ 
										&Config_Code,  /*void * pSourceData,*/ 
										&size,        /* UNS8 * pExpectedSize*/
										RW);          /* UNS8 checkAccess */
		//subindex++;
	}

}


UNS8 Config_table[2] = {0,2};
unsigned char ret=0;
void CANOpen_App_Task(void)
{
	/*向从机写数据*/
	ret = writeNetworkDict(&Master_Data,0x01,0x2002,0x01,1,uint8,&Config_table[0],0);

	/*调用完writeNetworkDict函数后表明已经建立了一条SDOline如果还需要写的话，必须关闭本次的SDOline*/
	/*此函数与操作服务器对象字典的函数成对出现*/
	closeSDOtransfer(&Master_Data,0x01,SDO_CLIENT);

//	ret = writeNetworkDict(&Master_Data,0x02,0x2002,0x02,1,uint8,&Config_table[1],0);
//	closeSDOtransfer(&Master_Data,0x03,SDO_CLIENT);

	Config_table[0]=!Config_table[0];
}

