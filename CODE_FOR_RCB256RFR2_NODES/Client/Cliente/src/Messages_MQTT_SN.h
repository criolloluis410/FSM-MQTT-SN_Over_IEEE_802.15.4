/*
 * Mensajes_MQTT_SN.h
 *
 * Created: 19/11/2021 11:00:41
 *  Author: Personal
 */ 


#ifndef MESSAGES_MQTT_SN_H_
#define MESSAGES_MQTT_SN_H_
#include "usr_wireless.h"

uint8_t ADVERTICE[5];
uint8_t SEARCHGW[3];
uint8_t GWINFO[6];//longitud maxima.
uint8_t CONNECT[29];//longitud maxima.
uint8_t CONNACK[3];
uint8_t WILLTOPICREQ[2];
uint8_t WILLTOPIC[29];//longitud maxima.
uint8_t WILLMSGREQ[2];
uint8_t WILLMSG[29];//longitud maxima.
uint8_t REGISTER[29];//longitud maxima.
uint8_t REGACK[7];
uint8_t PUBLISH[60];//longitud maxima.
uint8_t PUBACK[7];
uint8_t PUBCOMP[4];
uint8_t PUBREC[4];
uint8_t PUBREL[4];
uint8_t SUBSCRIBE[29];//longitud maxima.
uint8_t SUBACK[8];
uint8_t UNSUBSCRIBE[29];//longitud maxima.
uint8_t UNSUBACK[4];
uint8_t PINGREQ[4];
uint8_t PINGRESP[2];
uint8_t DISCONNECT[4];
uint8_t WILLTOPICUPD[29];//longitud maxima.
uint8_t WILLMSGUPD[29];//longitud maxima.
uint8_t WILLTOPICRESP[3];
uint8_t WILLMSGRESP[3];

int funcionADVERTICE(uint8_t *msg);
int funcionSEARCHGW(uint8_t *msg);
int funcionGWINFO(uint8_t *msg, char tipoNodo);
int funcionCONNECT(uint8_t *msg, uint8_t flags, uint8_t *clientId, int clidLongitud);
int funcionCONNACK(uint8_t *msg,uint8_t returnCode);
int funcionWILLTOPICREQ(uint8_t *msg);
int funcionWILLTOPIC(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLongitud);
int funcionWILLMSGREQ(uint8_t *msg);
int funcionWILLMSG(uint8_t *msg, uint8_t *willMsg, int willMsgLongitud);
int funcionREGISTER(uint8_t *msg, uint8_t *topicName, int topicNameLongitud);
int funcionREGACK(uint8_t *msg, uint8_t returnCode);
int functionPUBLISH(uint8_t *msg, uint8_t flags, uint8_t *data, int dataLongitud);
int funcionPUBACK(uint8_t *msg, uint8_t returnCode);
int funcionPUBCOMP(uint8_t *msg);
int funcionPUBREC(uint8_t *msg);
int functionPUBREL(uint8_t *msg);
int funcionSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameoId, int topicNameoIdLongitud);
int funcionSUBACK(uint8_t *msg, uint8_t flags, uint8_t returnCode);
int funcionUNSUBSCRIBE(uint8_t *msg, uint8_t flags, uint8_t *topicNameoId, int topicNameoIdLongitud);
int funcionUNSUBACK(uint8_t *msg);
int funcionPINGREQ(uint8_t *msg);
int funcionPINGRESP(uint8_t *msg);
int funcionDISCONNECT(uint8_t *msg, char tipoDISC);
//int funcionDISCONNECT(uint8_t *msg);
int funcionWILLTOPICUPD(uint8_t *msg, uint8_t flags, uint8_t *willTopic, int willTopicLongitud);
int funcionWILLMSGUPD(uint8_t *msg, uint8_t *willMsg, int willMsgLongitud);
int funcionWILLTOPICRESP(uint8_t *msg, uint8_t returnCode);
int funcionWILLMSGRESP(uint8_t *msg, uint8_t returnCode);

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





#endif /* MENSAJES_MQTT-SN_H_ */