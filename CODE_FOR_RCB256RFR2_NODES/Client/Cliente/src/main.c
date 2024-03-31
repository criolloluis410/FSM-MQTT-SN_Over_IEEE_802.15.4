/* ********************************************************************************************************************************
ESCUELA POLIT�CNICA NACIONAL
FACULTAD DE INGENIER�A EL�CTRICA Y ELECTR�NICA
 													
AUTOR:	 LUIS EDUARDO CRIOLLO CAJAMARCA												
TEMA:  	 DESARROLLO DE LA M�QUINA DE ESTADOS FINITOS DEL PROTOCOLO MQTT-SN 
         PARA SU OPERACI�N SOBRE IEEE 802.15.4 EN TOPOLOGIAS LINEALES 
***********************************************************************************************************************************/
// ********************************************************** NODO CLIENTE ******************************************************* //
/* === INCLUDES ============================================================ */

#include "tal.h"
#include "wireless_api.h"

/* === PROTOTYPES ====================================================== */

/**
 * \brief Application task
 */
static void app_task(void);

/**
 * \brief Tasks Handled By the Stack and application are Performed here,this function is handled in a loop to perform tasks continuously
 */
static void WirelessTask(void);

/* === IMPLEMENTATION ====================================================== */

/**
 * \brief Main function of the application
 */
int main(void)
{	
	/* Initialize the Wireless Module */
	wireless_init();    
	
	/*The Modules selected in the wizard are initialized here */
	modules_init();

	while (1)
	{
		WirelessTask();
	}
}

/**
 * \brief Tasks Handled By the Stack and application are Performed here,this function is handled in a loop to perform tasks continuously 
 */
void WirelessTask(void)
{
	/* These methods are called to perform the default tasks of the MAC Stack */
	pal_task();
	tal_task();

	/* Custom (User) tasks are performed here. */
	app_task();
}

/**
 * \brief Application task
 */
 void app_task(void)
{
	usr_wireless_app_task();
}

/**
 *
 * \brief This method (callback) is called when a frame has been transmitted by the transceiver
 * \param status  Status of frame transmission i.e MAC_SUCCESS,MAC_NO_ACK,CHANNEL_ACCESS_FAILURE etc
 * \param frame pointer to the transmitted frame
 */
void tal_tx_frame_done_cb(retval_t status, frame_info_t *frame)
{
	/*Perform application tasks when a frame is transmitted here*/
	usr_frame_transmitted_cb(status, frame);
}

/**
 *
 * \brief This method (callback) is called when a frame is received by the transceiver
 * \param frame pointer to the received frame
 *
 */
void tal_rx_frame_cb(frame_info_t *frame)
{
	/*Perform application tasks when a frame is received here*/
	usr_frame_received_cb(frame);
	// Free-up the buffer which was used for reception once the frame is extracted.
	bmm_buffer_free(frame->buffer_header);
}


