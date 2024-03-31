/*
 * Mensajes_MQTT_SN.c
 *
 * Created: 19/11/2021 11:00:26
 *  Author: Personal
 */ 
#include "Messages_MQTT_SN.h"

int funcionADVERTICE(uint8_t *msg){
	uint8_t Length=0x05;
	uint8_t MsgType=0x00;
	uint8_t GwId=0x01;
	uint8_t Duration1=0x03;
	uint8_t Duration2=0x84;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=GwId;
	msg[3]=Duration1;
	msg[4]=Duration2;
	//ESTRUCTURA_ADVERTICE ADVERTICE_STR={Length,MsgType,GwId,Duration1,Duration2};
	return sizeof(ADVERTICE);
}
int funcionSEARCHGW(uint8_t *msg){
	uint8_t Length=0x03;
	uint8_t MsgType=0x01;
	uint8_t Radius=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Radius;
	return sizeof(SEARCHGW);
}
int funcionGWINFO(uint8_t *msg, char tipoNodo){
	int logitud=0;
	uint8_t Length;
	uint8_t MsgType=0x02;
	uint8_t GwId=0x01;
	uint8_t GwAddTipoRed=0x01;//Indica sobre que red opera el GATEWAY(Zigbee)
	uint8_t GwAdd1=0xCA;
	uint8_t GwAdd2=0xFE;
	if (tipoNodo=='c')
	{
		Length=0x06;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=GwId;
		msg[3]=GwAddTipoRed;
		msg[4]=GwAdd1;
		msg[5]=GwAdd2;
		logitud=6;
	}
	if (tipoNodo=='g')
	{
		Length=0x03;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=GwId;
		logitud=3;
	}
	return logitud;
}
int funcionCONNECT(uint8_t *msg, uint8_t flags, uint8_t *clientId,int clidLongitud){
	int logitud=0;
	uint8_t Length;
	uint8_t MsgType=0x04;
	uint8_t Flags=flags;
	uint8_t ProtocolId=0x01;
	uint8_t Duration1=0x03;
	uint8_t Duration2=0x84;
	Length=clidLongitud+0x06;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=ProtocolId;
	msg[4]=Duration1;
	msg[5]=Duration2;
	int j=6;
	for (int i=0;i<clidLongitud;i++)
	{
		msg[j]=clientId[i];
		j++;
	}
	logitud=6+clidLongitud;
	return logitud;
}
int funcionCONNACK(uint8_t *msg,uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x05;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return sizeof(CONNACK);
}
int funcionWILLTOPICREQ(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x06;
	msg[0]=Length;
	msg[1]=MsgType;
	return sizeof(WILLTOPICREQ);
}
int funcionWILLTOPIC(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLongitud){
	//int logitud=0;
	uint8_t Length=0x03+willTopicLongitud;
	uint8_t MsgType=0x07;
	uint8_t Flags=flags;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	int j=3;
	for (int i=0;i<willTopicLongitud;i++)
	{
		msg[j]=willTopic[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionWILLMSGREQ(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x08;
	msg[0]=Length;
	msg[1]=MsgType;
	return sizeof(WILLMSGREQ);
}
int funcionWILLMSG(uint8_t *msg, uint8_t *willMsg, int willMsgLongitud){
	//int logitud=0;
	uint8_t Length=0x02+willMsgLongitud;
	uint8_t MsgType=0x09;
	msg[0]=Length;
	msg[1]=MsgType;
	int j=2;
	for (int i=0;i<willMsgLongitud;i++)
	{
		msg[j]=willMsg[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionREGISTER(uint8_t *msg, uint8_t *topicName, int topicNameLongitud){
	//int logitud=0;
	uint8_t Length=0x06+topicNameLongitud;
	uint8_t MsgType=0x0A;
	uint8_t topicId1=0x00;
	uint8_t topicId2=0x01;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=topicId1;
	msg[3]=topicId2;
	msg[4]=msgId1;
	msg[5]=msgId2;
	int j=6;
	for (int i=0;i<topicNameLongitud;i++)
	{
		msg[j]=topicName[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionREGACK(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x07;
	uint8_t MsgType=0x0B;
	uint8_t topicId1=0x00;
	uint8_t topicId2=0x01;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=topicId1;
	msg[3]=topicId2;
	msg[4]=msgId1;
	msg[5]=msgId2;
	msg[6]=ReturnCode;
	return Length;
}
int functionPUBLISH(uint8_t *msg, uint8_t flags, uint8_t *data, int dataLongitud){
	//int logitud=0;
	uint8_t Length=0x07+dataLongitud;
	uint8_t MsgType=0x0C;
	uint8_t Flags=flags;
	uint8_t topicId1=0x00;
	uint8_t topicId2=0x01;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=topicId1;
	msg[4]=topicId2;
	msg[5]=msgId1;
	msg[6]=msgId2;
	int j=7;
	for (int i=0;i<dataLongitud;i++)
	{
		msg[j]=data[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionPUBACK(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x07;
	uint8_t MsgType=0x0D;
	uint8_t topicId1=0x00;
	uint8_t topicId2=0x01;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=topicId1;
	msg[3]=topicId2;
	msg[4]=msgId1;
	msg[5]=msgId2;
	msg[6]=ReturnCode;
	return Length;
}
int funcionPUBCOMP(uint8_t *msg){
	uint8_t Length=0x04;
	uint8_t MsgType=0x0E;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=msgId1;
	msg[3]=msgId2;
	return Length;
}
int funcionPUBREC(uint8_t *msg){
	uint8_t Length=0x04;
	uint8_t MsgType=0x0F;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=msgId1;
	msg[3]=msgId2;
	return Length;
}
int functionPUBREL(uint8_t *msg){
	uint8_t Length=0x04;
	uint8_t MsgType=0x10;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=msgId1;
	msg[3]=msgId2;
	return Length;
}
int funcionSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameoId, int topicNameoIdLongitud){
	//int logitud=0;
	uint8_t Length=0x07+topicNameoIdLongitud;
	uint8_t MsgType=0x12;
	uint8_t Flags=flags;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=msgId1;
	msg[4]=msgId2;
	int j=5;
	for (int i=0;i<topicNameoIdLongitud;i++)
	{
		msg[j]=topicNameoId[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionSUBACK(uint8_t *msg, uint8_t flags, uint8_t returnCode){
	uint8_t Length=0x08;
	uint8_t MsgType=0x13;
	uint8_t Flags=flags;
	uint8_t topicId1=0x00;
	uint8_t topicId2=0x01;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=topicId1;
	msg[4]=topicId2;
	msg[5]=msgId1;
	msg[6]=msgId2;
	msg[7]=ReturnCode;
	return Length;
}
int funcionUNSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameoId, int topicNameoIdLongitud){
	uint8_t Length=0x07+topicNameoIdLongitud;
	uint8_t MsgType=0x14;
	uint8_t Flags=flags;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=msgId1;
	msg[4]=msgId2;
	int j=5;
	for (int i=0;i<topicNameoIdLongitud;i++)
	{
		msg[j]=topicNameoId[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionUNSUBACK(uint8_t *msg){
	uint8_t Length=0x04;
	uint8_t MsgType=0x15;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=msgId1;
	msg[3]=msgId2;
	return Length;
}
int funcionPINGREQ(uint8_t *msg){
	uint8_t Length=0x04;
	uint8_t MsgType=0x16;
	uint8_t msgId1=0x00;
	uint8_t msgId2=0x01;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=msgId1;
	msg[3]=msgId2;
	return Length;
}
int funcionPINGRESP(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x17;
	msg[0]=Length;
	msg[1]=MsgType;
	return Length;
}
int funcionDISCONNECT(uint8_t *msg, char tipoDISC){//---------------------------------
	int longitud=0;
	uint8_t Length;
	uint8_t MsgType=0x18;
	uint8_t Duration1=0x03;
	uint8_t Duration2=0x84;
	if (tipoDISC=='d')
	{
		Length=0x02;
		msg[0]=Length;
		msg[1]=MsgType;
		longitud=2;
	}
	else if (tipoDISC=='s')
	{
		Length=0x04;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=Duration1;
		msg[3]=Duration2;
		longitud=4;
	}

	return longitud;
}
int funcionWILLTOPICUPD(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLongitud){
	uint8_t Length=0x03+willTopicLongitud;
	uint8_t MsgType=0x1A;
	uint8_t Flags=flags;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	int j=3;
	for (int i=0;i<willTopicLongitud;i++)
	{
		msg[j]=willTopic[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionWILLMSGUPD(uint8_t *msg, uint8_t *willMsg, int willMsgLongitud){
	uint8_t Length=0x02+willMsgLongitud;
	uint8_t MsgType=0x1C;
	msg[0]=Length;
	msg[1]=MsgType;
	int j=2;
	for (int i=0;i<willMsgLongitud;i++)
	{
		msg[j]=willMsg[i];
		j++;
	}
	//logitud=3+willTopicLongitud;
	return Length;
}
int funcionWILLTOPICRESP(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x1B;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return Length;
}
int funcionWILLMSGRESP(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x1D;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return Length;
}