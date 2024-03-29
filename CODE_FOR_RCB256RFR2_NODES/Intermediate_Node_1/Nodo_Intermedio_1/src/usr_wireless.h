
#ifndef USR_WIRELESS_H_
#define USR_WIRELESS_H_

#include "wireless_api.h"

#define max_data 101
typedef struct
{
	uint8_t length;			// Frame Length
	uint16_t fcf;			// Frame Control Field
	uint8_t sec_num;		// Sequence Number
	uint16_t PAN_dest;		// PAN Destination Address
	uint16_t src_addr;		// Source Address
	uint16_t dest_addr;		// Destination Address
	uint8_t MQTT_SN_message[max_data];	// Payload
	uint16_t fcs;			// Frame Control Sequence
} ieee_802_15_42_frame;

ieee_802_15_42_frame receivedFrame;

/**
* \brief This function needs to be edited by the user for adding application tasks
*/
void usr_wireless_app_task(void);

/**
* \brief This function needs to be edited by the user for adding  tasks when a frame is received
* \param frame pointer to the received frame
*/
void usr_frame_received_cb(frame_info_t *frame);

/**
* \brief This function needs to be edited by the user for adding  tasks when a frame is transmitted
* \param status  Status of frame transmission i.e MAC_SUCCESS,MAC_NO_ACK,CHANNEL_ACCESS_FAILURE etc
* \param frame pointer to the transmitted frame
*/
void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame);

#endif /* USER_WIRELESS_H_ */