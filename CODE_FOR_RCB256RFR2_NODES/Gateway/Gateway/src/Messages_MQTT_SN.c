/*
 * Mensajes.c
 *
 * Created: 18/11/2021 11:00:08
 *  Author: Personal
 */ 
#include "Messages_MQTT_SN.h"

int functionADVERTISE(uint8_t *msg){
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
	return sizeof(ADVERTISE);
}
int functionSEARCHGW(uint8_t *msg){
	uint8_t Length=0x03;
	uint8_t MsgType=0x01;
	uint8_t Radius=0x00;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Radius;
	return sizeof(SEARCHGW);
}
int functionGWINFO(uint8_t *msg, char nodeType){
	int length=0;
	uint8_t Length;
	uint8_t MsgType=0x02;
	uint8_t GwId=0x01;
	uint8_t GwAdd1=0x00;
	uint8_t GwAdd2=0x01;
	if (nodeType=='c')
	{
		Length=0x05;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=GwId;
		msg[3]=GwAdd1;
		msg[4]=GwAdd2;
		length=5;
	}
	if (nodeType=='g')
	{
		Length=0x03;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=GwId;
		length=3;
	}
	return length;
}
int functionCONNECT(uint8_t *msg, uint8_t flags, uint8_t *clientId,int clientIdLength){
	int length=0;
	uint8_t Length;
	uint8_t MsgType=0x04;
	uint8_t Flags=flags;
	uint8_t ProtocolId=0x01;
	uint8_t Duration1=0x03;
	uint8_t Duration2=0x84;
	Length=clientIdLength+0x06;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	msg[3]=ProtocolId;
	msg[4]=Duration1;
	msg[5]=Duration2;
	int j=6;
	for (int i=0;i<clientIdLength;i++)
	{
		msg[j]=clientId[i];
		j++;
	}
	length=6+clientIdLength;
	return length;
}
int functionCONNACK(uint8_t *msg,uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x05;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return sizeof(CONNACK);
}
int functionWILLTOPICREQ(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x06;
	msg[0]=Length;
	msg[1]=MsgType;
	return sizeof(WILLTOPICREQ);
}
int functionWILLTOPIC(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLength){
	uint8_t Length=0x03+willTopicLength;
	uint8_t MsgType=0x07;
	uint8_t Flags=flags;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	int j=3;
	for (int i=0;i<willTopicLength;i++)
	{
		msg[j]=willTopic[i];
		j++;
	}
	return Length;
}
int functionWILLMSGREQ(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x08;
	msg[0]=Length;
	msg[1]=MsgType;
	return sizeof(WILLMSGREQ);
}
int functionWILLMSG(uint8_t *msg, uint8_t *willMsg, int willMsgLength){
	uint8_t Length=0x02+willMsgLength;
	uint8_t MsgType=0x09;
	msg[0]=Length;
	msg[1]=MsgType;
	int j=2;
	for (int i=0;i<willMsgLength;i++)
	{
		msg[j]=willMsg[i];
		j++;
	}
	return Length;
}
int functionREGISTER(uint8_t *msg, uint8_t *topicName, int topicNameLength){
	uint8_t Length=0x06+topicNameLength;
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
	for (int i=0;i<topicNameLength;i++)
	{
		msg[j]=topicName[i];
		j++;
	}
	return Length;
}
int functionREGACK(uint8_t *msg, uint8_t returnCode){
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
int functionPUBLISH(uint8_t *msg, uint8_t flags, uint8_t *data, int dataLength){
	uint8_t Length=0x07+dataLength;
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
	for (int i=0;i<dataLength;i++)
	{
		msg[j]=data[i];
		j++;
	}
	return Length;
}
int functionPUBACK(uint8_t *msg, uint8_t returnCode){
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
int functionPUBCOMP(uint8_t *msg){
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
int functionPUBREC(uint8_t *msg){
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
int functionSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameOrId, int topicNameOrIdLength){
	uint8_t Length=0x07+topicNameOrIdLength;
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
	for (int i=0;i<topicNameOrIdLength;i++)
	{
		msg[j]=topicNameOrId[i];
		j++;
	}
	return Length;
}
int functionSUBACK(uint8_t *msg, uint8_t flags, uint8_t returnCode){
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
int functionUNSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameOrId, int topicNameOrIdLength){
	uint8_t Length=0x07+topicNameOrIdLength;
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
	for (int i=0;i<topicNameOrIdLength;i++)
	{
		msg[j]=topicNameOrId[i];
		j++;
	}
	return Length;
}
int functionUNSUBACK(uint8_t *msg){
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
int functionPINGREQ(uint8_t *msg){
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
int functionPINGRESP(uint8_t *msg){
	uint8_t Length=0x02;
	uint8_t MsgType=0x17;
	msg[0]=Length;
	msg[1]=MsgType;
	return Length;
}
int functionDISCONNECT(uint8_t *msg, char discType){
	int length=0;
	uint8_t Length;
	uint8_t MsgType=0x18;
	uint8_t Duration1=0x00;
	uint8_t Duration2=0x10;
	if (discType=='d')
	{
		Length=0x02;
		msg[0]=Length;
		msg[1]=MsgType;
		length=2;
	}
	else if (discType=='s')
	{
		Length=0x04;
		msg[0]=Length;
		msg[1]=MsgType;
		msg[2]=Duration1;
		msg[3]=Duration2;
		length=4;
	}

	return length;
}
int functionWILLTOPICUPD(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLength){
	uint8_t Length=0x03+willTopicLength;
	uint8_t MsgType=0x1A;
	uint8_t Flags=flags;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=Flags;
	int j=3;
	for (int i=0;i<willTopicLength;i++)
	{
		msg[j]=willTopic[i];
		j++;
	}
	return Length;
}
int functionWILLMSGUPD(uint8_t *msg, uint8_t *willMsg, int willMsgLength){
	uint8_t Length=0x02+willMsgLength;
	uint8_t MsgType=0x1C;
	msg[0]=Length;
	msg[1]=MsgType;
	int j=2;
	for (int i=0;i<willMsgLength;i++)
	{
		msg[j]=willMsg[i];
		j++;
	}
	return Length;
}
int functionWILLTOPICRESP(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x1B;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return Length;
}
int functionWILLMSGRESP(uint8_t *msg, uint8_t returnCode){
	uint8_t Length=0x03;
	uint8_t MsgType=0x1D;
	uint8_t ReturnCode=returnCode;
	msg[0]=Length;
	msg[1]=MsgType;
	msg[2]=ReturnCode;
	return Length;
}
