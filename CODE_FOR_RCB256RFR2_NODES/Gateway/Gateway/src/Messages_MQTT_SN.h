/*
 * Mensajes.h
 *
 * Created: 18/11/2021 11:00:34
 *  Author: Personal
 */ 


#ifndef MESSAGES_MQTT_SN_H_
#define MESSAGES_MQTT_SN_H_

#include "usr_wireless.h"

uint8_t ADVERTISE[5];
uint8_t SEARCHGW[3];
uint8_t GWINFO[6];
uint8_t CONNECT[29];
uint8_t CONNACK[3];
uint8_t WILLTOPICREQ[2];
uint8_t WILLTOPIC[29];
uint8_t WILLMSGREQ[2];
uint8_t WILLMSG[29];
uint8_t REGISTER[29];
uint8_t REGACK[7];
uint8_t PUBLISH[60];
uint8_t PUBACK[7];
uint8_t PUBCOMP[4];
uint8_t PUBREC[4];
uint8_t PUBREL[4];
uint8_t SUBSCRIBE[29];
uint8_t SUBACK[8];
uint8_t UNSUBSCRIBE[29];
uint8_t UNSUBACK[4];
uint8_t PINGREQ[4];
uint8_t PINGRESP[2];
uint8_t DISCONNECT[4];
uint8_t WILLTOPICUPD[29];
uint8_t WILLMSGUPD[29];
uint8_t WILLTOPICRESP[3];
uint8_t WILLMSGRESP[3];

int functionADVERTISE(uint8_t *msg);
int functionSEARCHGW(uint8_t *msg);
int functionGWINFO(uint8_t *msg, char nodeType);
int functionCONNECT(uint8_t *msg, uint8_t flags, uint8_t *clientId, int clientIdLength);
int functionCONNACK(uint8_t *msg, uint8_t returnCode);
int functionWILLTOPICREQ(uint8_t *msg);
int functionWILLTOPIC(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLength);
int functionWILLMSGREQ(uint8_t *msg);
int functionWILLMSG(uint8_t *msg, uint8_t *willMsg, int willMsgLength);
int functionREGISTER(uint8_t *msg, uint8_t *topicName, int topicNameLength);
int functionREGACK(uint8_t *msg, uint8_t returnCode);
int functionPUBLISH(uint8_t *msg, uint8_t flags, uint8_t *data, int dataLength);
int functionPUBACK(uint8_t *msg, uint8_t returnCode);
int functionPUBCOMP(uint8_t *msg);
int functionPUBREC(uint8_t *msg);
int functionPUBREL(uint8_t *msg);
int functionSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameOrId, int topicNameOrIdLength);
int functionSUBACK(uint8_t *msg, uint8_t flags, uint8_t returnCode);
int functionUNSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameOrId, int topicNameOrIdLength);
int functionUNSUBACK(uint8_t *msg);
int functionPINGREQ(uint8_t *msg);
int functionPINGRESP(uint8_t *msg);
int functionDISCONNECT(uint8_t *msg, char discType);
// int functionDISCONNECT(uint8_t *msg);////////////////
int functionWILLTOPICUPD(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLength);
int functionWILLMSGUPD(uint8_t *msg, uint8_t *willMsg, int willMsgLength);
int functionWILLTOPICRESP(uint8_t *msg, uint8_t returnCode);
int functionWILLMSGRESP(uint8_t *msg, uint8_t returnCode);

//*****************************************Estructura para mensajes****************************************************************
//Structures can also be used to create mqtt-sn messages, but for the realization of this project we chose to use only
//for the use of vectors because its use is simpler and there was no inconvenience during its use, on the other hand it is not possible to create mqtt-sn messages.
//it is not discarded the use of structures in future advances of the project for what we have an example:
  
#define max_length 101
typedef struct
{
	uint8_t Length;
	uint8_t MsgType;
	uint8_t GwId;
	uint8_t Duration1;
	uint8_t Duration2;
	
}ESTRUCTURA_ADVERTICE;

#endif /* MENSAJES_H_ */