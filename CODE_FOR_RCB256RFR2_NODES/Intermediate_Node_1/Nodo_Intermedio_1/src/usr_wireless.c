/* ********************************************************************************************************************************
ESCUELA POLIT�CNICA NACIONAL
FACULTAD DE INGENIER�A EL�CTRICA Y ELECTR�NICA
 													
TOPIC: DEVELOPMENT OF THE FINITE STATE MACHINE OF THE MQTT-SN PROTOCOL
       FOR OPERATION OVER IEEE 802.15.4 IN LINEAR TOPOLOGIES
***********************************************************************************************************************************/
// ****************************************************** Intermediate Node 1 *****************************************************//

// Libraries //
#include "usr_wireless.h"
#include "wireless_config.h"


void usr_wireless_app_task(void)
{

}

// Reception function //
void usr_frame_received_cb(frame_info_t *frame)
{
	memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
	memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
	bmm_buffer_free(frame->buffer_header); // Free buffer data, avoid memory leakage

	uint16_t dirOrigen; // Message Origin Address
	uint8_t msgTypeMQTT_SN; // MQTT-SN Message Type
	uint8_t longitudMQTT_SN; // MQTT-SN Message Length

	dirOrigen = receivedFrame.src_addr;
	longitudMQTT_SN = receivedFrame.MQTT_SN_message[0];
	msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

	if (dirOrigen == 0x0001) // Receive the origin address of node 0x0001
	{ // By means of the msgTypeMQTT_SN variable, the type of received message is verified.
		if (msgTypeMQTT_SN == 0x00) // MQTT-SN message type received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message, longitudMQTT_SN, 3); // MQTT-SN message send
		}
		else if (msgTypeMQTT_SN == 0x01) // MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message, longitudMQTT_SN, 3); // MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x02)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x04)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x05)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x06)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x07)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x08)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x09)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0A)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0B)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0C)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0D)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0E)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0F)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x10)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x12)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x13)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x14)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x15)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x16)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x17)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x18)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1A)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1B)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1C)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1D)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
	}

    if (dirOrigen == 0x0003) // Receive the origin address of node 0x001
    { // By means of the msgTypeMQTT_SN variable, the type of received message is verified.
	    if (msgTypeMQTT_SN == 0x00) // MQTT-SN message type received
	    {
		    transmit_sample_frame(receivedFrame.MQTT_SN_message, longitudMQTT_SN, 1); // MQTT-SN message send
	    }
	    else if (msgTypeMQTT_SN == 0x01) // MQTT-SN message received
	    {
		    transmit_sample_frame(receivedFrame.MQTT_SN_message, longitudMQTT_SN, 1); // MQTT-SN message send
	    }
		else if (msgTypeMQTT_SN==0x02)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x04)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x05)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x06)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x07)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x08)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x09)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0A)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0B)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0C)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0D)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0E)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x0F)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x10)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x12)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x13)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x14)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x15)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x16)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x17)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x18)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1A)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1B)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,1);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1C)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
		else if (msgTypeMQTT_SN==0x1D)//MQTT-SN message received
		{
			transmit_sample_frame(receivedFrame.MQTT_SN_message,longitudMQTT_SN,3);//MQTT-SN message send
		}
	}

}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}