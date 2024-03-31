/* ********************************************************************************************************************************
ESCUELA POLITECNICA NACIONAL
ELECTRICAL AND ELECTRONIC ENGINEERING FACULTY
 													
TOPIC: DEVELOPMENT OF THE FINITE STATE MACHINE OF THE MQTT-SN PROTOCOL
       FOR OPERATION OVER IEEE 802.15.4 IN LINEAR TOPOLOGIES
***********************************************************************************************************************************/
// Libraries //
// *********************************************************************************************************************************
#include "usr_wireless.h" // Initializes the node's functionality
#include "wireless_config.h" // Allows access to files containing transmission characteristics
#include "periodic_timer.h" // Allows the use of timers.
#include "Messages_MQTT_SN.h"// Allows the use of MQTT-SN messages(MSG).

// Variables used in all states //
// ********************************************************************************************************************************
int stateIndex = 0; // Variable to control the transition from one state to another
int temporaryStateIndex = 0; // Variable used when a state can change to two or more states
int successfulReceptionControl = -1; // Variable used when a MSG is received (to avoid blocking in reception)
int stateControl = 1; // Variable to control any state the system is in
int enableReception = 0; // Variable to allow the node to receive any MSG (Can be customized)
int enableTransmission = -1; // Variable to send MQTT-SN MSG, depending on the index a specific message will be selected
int messageLength = 0; // Size of the MQTT-SN Message to be sent
int timerDurationTime = 0; // Variable to assign the duration of the timer for a specific state
int addTimer = 0; // Variable to activate the timer when necessary
//TimerEnd = 0; // Variable to indicate that the timer time has ended
int jump = 0; // It is used to assign a Delay after using the Transmit_sample_frame() function
int test = 0;

// ********************************************************* GATEWAY NODE ******************************************************* //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 1. GATEWAY_ADVERTISEMENT_AND_DISCOVERY_PROCEDURE ******************************************* //
/*
// Procedure Variables //
// Timer durations, values are lower for easier testing
int durationTX = 25; // int durationTX = 900;
int durationRX = 25; // int durationTX = 900;

int announcementBroker = 0; // Used to indicate if the broker is functioning correctly
int gwBackup = 0; // Variable to configure the backup state of the gateway node 0 or 1
int NADV = 2; // Maximum number of times the node waits for announcement messages

// State Function Declarations //
void initialState(void); // INACTIVE S0

void state_1(void); // WAIT_ANNOUNCEMENT S1  

void state_2(void); // BACKUP S2

// Function where state functions are executed //
void usr_wireless_app_task(void)
{
    if (stateIndex == 0) // INACTIVE
    {
        initialState();
    }
    else if (stateIndex == 1) // WAIT_ANNOUNCEMENT S1  
    {
        state_1();
    }
    else if (stateIndex == 2) // BACKUP S2
    {
        state_2();
    }
}

// Function to receive MQTT-SN messages over IEEE802.15.4 frames //
void usr_frame_received_cb(frame_info_t *frame)
{
    if (enableReception == 1)
    {
        memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
        memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source buffer to destination
        bmm_buffer_free(frame->buffer_header); // Free buffer data to avoid overlap.

        uint8_t msgTypeMQTT_SN;
        msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

        if (msgTypeMQTT_SN == 0x00) // RX ADVERTISE from another GW
        {
            enableReception = 0; // Disable reception to avoid interference in state change.
            successfulReceptionControl = 1; // This variable allows the node to exit the reception loop
            // it is in and allows the state to continue with its operation.
        }
        else if (msgTypeMQTT_SN == 0x01) // RX SEARCHGW
        {
            enableReception = 0; // Disable reception to avoid interference in state change.
            successfulReceptionControl = 1; // This variable allows the node to exit the reception loop
            // it is in and allows the state to continue with its operation.
        }
    }
}

// Predefined Function // 
void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}
// State Function Implementation //
// ============================================================================================================================= //
void initialState(void) // INACTIVE S0
{
    uint8_t State[44] = ":''''''INACTIVE''''''::       S-00         :";
    if (stateControl == 1)
    {
        stateControl = 2;
        delay_ms(1500); // wait time to appreciate the start of state
    }
    else if (stateControl == 2)
    {
        stateControl = 3;
        transmit_sample_frame(State, 44, 1); // Message to indicate the current state
    }
    else if (stateControl == 3)
    {
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // The node will remain in this state until it receives a signal from the broker,
        //
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // Broker Signal ////////////////////////////////////////////////////////////////////////////////////////////////////// //
        if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
        {
            delay_ms(200); // Delay for the button to work
            controlLED(1);
            announcementBroker = 1; // Indicates that the broker is ready to operate
        }
        else if (announcementBroker == 1)
        {
            announcementBroker = 0;
            stop_timer1(); // Stop all timers
            transmit_sample_frame((uint8_t *)":        broker      ::       Ready==1     :", 44, 1); // Indicates Broker Ready signal
            jump = 1; // Jump to avoid blocking
        }
        else if (jump == 1)
        {
            jump = 0;
            delay_ms(400); // Delay for the button to work
            addTimer = 1; // When the signal from the server arrives, configuration of Timer will continue
        } // TX ADVERTISE  and Add TIMER TADV
        else if (addTimer == 1)
        {
            addTimer = 0; // The timer is assigned only once so the variable returns to 0
            messageLength = functionADVERTISE(ADVERTISE); // Invocation of the function that creates the message to be sent, returns the message length
            transmit_sample_frame(ADVERTISE, messageLength, 0); // Message transmission ADVERTISE to the client
            messageLength = 0; // The length returns to 0 to avoid overlap
            timerDurationTime = durationTX; // The timer has a duration of 15 seconds for testing (should be 15min)
            start_timer1(); // The timer starts its operation
            // STATE CHANGE
            temporaryStateIndex = 1; // Variable that allows to move to the ADVERTISEMENT state.
            stateControl = 4;
        }
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
    }
    else if (stateControl == 4)
    {
        stateControl = 5;
        delay_ms(1500); // waiting time to appreciate the end of state
    }
    else if (stateControl == 5)
    {
        stateControl = 1; // This control variable is set to one so that the following states can start their operation.
        transmit_sample_frame((uint8_t *)":      GW_END        ::........S-00........:", 44, 1); // State indicator
        stateIndex = temporaryStateIndex; // Change of state to ADVERTISEMENT.
        temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
    }
}
// ============================================================================================================================= //
void state_1(void) // WAIT_ANNOUNCEMENT S1
{
    uint8_t State[44] = ":'WAIT_ADVERTISEMENT'::       S-01         :";         
					  //":        GFIN        ::........S-00........:"
    if (stateControl == 1)
    {
        stateControl = 2;
        delay_ms(1500); // wait time to appreciate the start of state
    }
    else if (stateControl == 2)
    {
        stateControl = 3;
        transmit_sample_frame(State, 44, 1); // state indicator
        enableReception = 1; // Variable that enables the function that receives messages.
    }
    else if (stateControl == 3)
    {
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // The node remains in this state until the reception of the required message (SEARCHGW) is received,
        // the Tadv Timer ends, the gateway is used as a backup, or when the broker indicates that it is not working.
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // End of Timer Tadv ////////////////////////////////////////////////////////////////////////////////////////////// //
        if (TimerEnd == 1) // If the condition indicating that TADV time has ended is met
        {
            TimerEnd = 0; // Disable the reception of the timer signal as it has ended
            transmit_sample_frame((uint8_t *)":        TADV:      ::       Finish      :", 42, 1); // Message indicating the end of Timer Tadv
											//:        broker     ::        TADV:      :
			jump = 1; // Move to use a delay
        }
        else if (jump == 1) // A delay is added to transmit another message later
        {
            jump = 0;
            delay_ms(200); // Delay
            enableTransmission = 0;
        }
        // TX ADVERTICE
        else if (enableTransmission == 0) // Once the timer signal is received, proceed to transmit a msg ADVERTICE with the variable aT=0
        {
            enableTransmission = -1; // Transmission is disabled because only an ADVERTICE message needs to be transmitted
            messageLength = functionADVERTISE(ADVERTISE); // Invocation of the function that creates the message to be sent, returns the message length
            transmit_sample_frame(ADVERTISE, messageLength, 0); // Message transmission
            messageLength = 0;
            addTimer = 1; // Proceed to reset the timer
        }
        else if (addTimer == 1) // Proceed to reset the TADV timer
        {
            addTimer = 0;
            timerDurationTime = durationTX; // The timer has a duration of 15 seconds for testing (should be 15min)
            start_timer1(); // The timer restarts its operation
            // EXIT STATE
            temporaryStateIndex = 1; // Return to the ADVERTISEMENT state.
            stateControl = 4; // Exit the loop
        }
        // RX SEARCHGW //
        else if (successfulReceptionControl == 1) // The condition is activated when a SEARCHGW is received
        {
            successfulReceptionControl = 0; // Reception is disabled to avoid blocking
            delay_ms(3500);
            enableTransmission = 2; // Move to tx
        }
        // TX GWINFO
        else if (enableTransmission == 2) // After receiving the required message, proceed to transmit a GWINFO as indicated by the variable value
        {
            enableTransmission = -1; // Transmission is disabled because only one GWINFO message is needed
            // TX GWINFO
            messageLength = functionGWINFO(GWINFO, 'g'); // Invocation of the function that creates the message to be sent, returns the message length
            transmit_sample_frame(GWINFO, messageLength, 0); // Message transmission
            messageLength = 0;
            // STATE CHANGE
            temporaryStateIndex = 1; // Return to the WAIT_ANNOUNCEMENT state.
            stateControl = 4;
        }
        // Gateway as backup //
        else if (gwBackup == 1) // To indicate that the GW functions as backup
        {
            delay_ms(200); // Delay for Timer reception
            stop_timer1(); // The timer is stopped, it is no longer necessary
            // STATE CHANGE
            temporaryStateIndex = 2; // Variable to move to the WAIT state.
            stateControl = 4; // Exit the loop
        }
        // brokerReady==0 //
        if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
        {
            delay_ms(200); // Delay for the button to work
            announcementBroker = 1;
        }
        else if (announcementBroker == 1)
        {
            announcementBroker = 0;
            controlLED(0);
            transmit_sample_frame((uint8_t *)":        broker      ::       Ready==0     :", 44, 1); // State indicator
            // STATE CHANGE
            temporaryStateIndex = 0; // Change to INACTIVE state
            stateControl = 4; // Exit the state
        }
    }
    else if (stateControl == 4)
    {
        stateControl = 5;
        delay_ms(1500); // Waiting time to appreciate the end of state
    }
    else if (stateControl == 5)
    {
        stateControl = 1;
        transmit_sample_frame((uint8_t *)":      GW_END        ::........S-01........:", 44, 1); // State indicator
        stateIndex = temporaryStateIndex; // Change to the WAIT_ANNOUNCEMENT state.
        temporaryStateIndex = -1; // Set to -1 for later use.
    }
}
// ============================================================================================================================= //
void state_2(void) // WAIT state
{
    uint8_t State[44] = ":'''''GW_WAIT''''''::       S-02         :";
    if (stateControl == 1)
    {
        stateControl = 2;
        delay_ms(1500); // Waiting time to appreciate the start of state
    }
    else if (stateControl == 2)
    {
        stateControl = 3;
        transmit_sample_frame(State, 44, 1); // State indicator
        enableReception = 1; // The function that will allow message reception is enabled.
    }
    else if (stateControl == 3)
    {
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // The node will remain in this state until receiving an ADVERTICE message from another GW
        // or if the Timer is restarted N times.
        // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
        // RX ADVERTICE //
        if (successfulReceptionControl == 1)
        {
            successfulReceptionControl = 0; // Reception is disabled to avoid blocking
            addTimer = 1; // Move to add Timer NADVTADV
        }
        else if (addTimer == 1)
        {
            addTimer = 0;; // The timer is assigned only once so the variable returns to 0
            // SET TIMER
            int NTadv = durationRX * NADV;
            timerDurationTime = NTadv; // The timer corresponds to the NADVTADV timer (should be 15min)
            start_timer1(); // The timer restarts its operation
            // STATE CHANGE
            temporaryStateIndex = 2; // Variable to return to the WAIT state
            stateControl = 4; // Exit the state
        }
        // Finaliza el Timer NADVTADV ////////////////////////////////////////////////////////////////////////////////////////// //
        else if (TimerEnd == 1)
        {
            // If the condition indicating that the time TADV has ended is met
            TimerEnd = 0; // Disable the reception of the timer signal as it has ended
            addTimer = 2; // Move to reset the timer
        }
        else if (addTimer == 2) // Timer TADV
        {
            addTimer = 0; // The timer is assigned only once so the variable returns to 0
            transmit_sample_frame((uint8_t *)":     Nadv*Tadv:    ::       Finish      :", 42, 1); // Message indicating the end of Nadv*Tadv
											//:        TADV:      ::     Nadv*Tadv:    :
			timerDurationTime = durationTX; // Timer duration TADV
            start_timer1(); // The timer starts its operation
            // STATE CHANGE
            temporaryStateIndex = 1; // Variable to move to the ADVERTISEMENT state.
            stateControl = 4; // Exit the state
        }
    }
    else if (stateControl == 4)
    {
        stateControl = 5;
        delay_ms(1500); // Waiting time to appreciate the end of state
    }
    else if (stateControl == 5)
    {
        stateControl = 1; // Control variable set to one so that the following states can start their operation.
        transmit_sample_frame((uint8_t *)":      GW_END        ::........S-02........:", 44, 1); // State indicator
        stateIndex = temporaryStateIndex; // Change to the WAIT state
        temporaryStateIndex = -1; // Set to -1 for later use
    }
}

// ============================================================================================================================= //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 2. CLIENT’S_CONNECTION_SETUP  ************************************************************** //
/*
// Procedure variables //
int proc2Count = 0; // Allows checking a sequence of messages
int conditional = 0; // Variable to avoid blocking in comparisons

// Declaration of State functions //
void initialState(int signal); // WAIT CONNECTION S0

void state_1(int signal); // RX_TOPIC S1

void state_2(int signal); // RX_MSG S2

// Function where state functions are executed //
void usr_wireless_app_task(void) {
	if (stateIndex == 0) // WAIT CONNECTION S0
	{
		initialState(0);
	} else if (stateIndex == 1) // RX_TOPIC S1
	{
		state_1(0);
	} else if (stateIndex == 2) // RX_MSG S2
	{
		state_2(0);
	}
}

// Function to receive MQTT-SN messages over IEEE802.15.4 frames //
void usr_frame_received_cb(frame_info_t *frame) {
	if (enableReception == 1) {
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Allocate memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source buffer to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data to avoid overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x04) // RX CONNECT
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 4; // This variable allows the node to exit the reception loop
		} else if (msgTypeMQTT_SN == 0x07) // RX WILLTOPIC
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 7; // This variable allows the node to exit the reception loop
		} else if (msgTypeMQTT_SN == 0x09) // RX WILLMSG
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 9; // This variable allows the node to exit the reception loop
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame) {
	// Placeholder function for frame transmission callback
}

// State function programming //
// ============================================================================================================================= //
void initialState(int signal) { // WAIT CONNECTION S0
					   //:      GW_END        ::      GW_END        :
	uint8_t State[44] = ":'''WAIT_CONNECTION''::       S-00         :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // RX_Connect status indicator
		enableReception = 1; // Variable enabling message receiving function.
		} else if (stateControl == 3) {
		// Node remains in this state until CONNECT message arrives, depends on the will flag of that message to stay or change state.
		// RX CONNECT //
		if (successfulReceptionControl == 4) // Condition activates when a CONNECT is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1;
			} else if (jump == 1) {
			jump = 0;
			delay_ms(4000); // wait time to appreciate response to reception
			conditional = 1; // Proceed to compare different WILL cases
		}
		// Will Flag= True
		else if (conditional == 1 && receivedFrame.MQTT_SN_message[2] == 0b00001000) {
			conditional = 0;
			enableTransmission = 6; // Proceed to send request
			} else if (enableTransmission == 6) { // TX WILLTOPICREQ
			enableTransmission = -1; // Transmission is disabled because only one message is needed
			messageLength = functionWILLTOPICREQ(WILLTOPICREQ); // Function prepares message to be sent, returns message length
			transmit_sample_frame(WILLTOPICREQ, messageLength, 0); // Message transmission
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // change to RX_TOPIC state
			stateControl = 4;
		}
		// Will Flag= FALSE
		else if (conditional == 1 && receivedFrame.MQTT_SN_message[2] == 0b00000000) 
		{
			conditional = 0;
			enableTransmission = 5; // Proceed to send ack
		} 
		
		else if (enableTransmission == 5) 
		{ // TX CONNACK
			enableTransmission = -1; // Transmission is disabled because only one message is needed
			uint8_t ReturnCode;
			ReturnCode = 0x00; // 0x00; 0x01 congestion; 0x03 not supported;
			messageLength = functionCONNACK(CONNACK, ReturnCode); // Function invocation to create message for transmission, returns message length
			transmit_sample_frame(CONNACK, messageLength, 0); // Message transmission
			messageLength = 0;
			proc2Count = 1;
			controlLED(proc2Count); // Indicates processing of message sequence.
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate end of state
		} else if (stateControl == 5) {
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t * ) ":      GW_END        ::........S-00........:", 44, 1); // State indicator
		                                  //:      GW_END        ::      GW_END        :
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to state
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ============================================================================================================================= //
void state_1(int signal) { // STATE RX_TOPIC S1
                       //:      GW_END        ::      GW_END        :
	uint8_t State[44] = ":''''''RX_TOPIC''''''::       S-01         :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
		} 
	else if (stateControl == 2) 
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Variable enabling message receiving function.
	}
	else if (stateControl == 3) {
		// Node remains in this state until it receives a WILLTOPIC
		// RX WILLTOPIC
		if (successfulReceptionControl == 7) // Condition activates when a WILLTOPIC is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Add transmission delay
		} 
		else if (jump == 1) 
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to reception
			enableTransmission = 8; //
		} 
		else if (enableTransmission == 8) 
		{ // TX WILLMSGREQ
			enableTransmission = -1; // Transmission is disabled because only one message is needed
			messageLength = functionWILLMSGREQ(WILLMSGREQ); // Function invocation to create the message to be sent, returns the message length
			transmit_sample_frame(WILLMSGREQ, messageLength, 0); // Message transmission
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 2; // State change
			stateControl = 4;
		}
	} 
	else if (stateControl == 4) 
	{
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of state
	} 
	else if (stateControl == 5) 
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t * ) ":      GW_END        ::........S-01........:", 44, 1); // State indicator
		                                ////:      GW_END        ::      GW_END        :
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ============================================================================================================================= //
void state_2(int signal) { // State RX_MSG S2
	                   //:      GW_END        ::      GW_END        :
	uint8_t State[44] = ":''''''RX_MSG''''''''::       S-02         :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages.

		} else if (stateControl == 3) {
		// Node remains in this state until it receives a WILLMSG
		// RX WILLMSG
		if (successfulReceptionControl == 9) // Condition activates when a WILLMSG is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Add transmission delay
			} else if (jump == 1) {
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to reception
			enableTransmission = 5; // Proceed to send CONNACK
			} else if (enableTransmission == 5) { // TX CONNACK
			enableTransmission = -1; // Transmission is disabled because only one message is needed
			uint8_t ReturnCode;
			ReturnCode = 0x00; // 0x00; 0x01 congestion; 0x03 not supported;
			messageLength = functionCONNACK(CONNACK, ReturnCode); // Function invocation to create the message to be sent, returns the message length
			transmit_sample_frame(CONNACK, messageLength, 0); // Message transmission
			messageLength = 0;
			proc2Count = 2;
			controlLED(proc2Count); // Indicates processing of a message sequence.
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of state
		} else if (stateControl == 5) {
		stateControl = 1;
		transmit_sample_frame((uint8_t * ) ":      GW_END        ::........S-02........:", 44, 1); // State indicator
		                                  //:      GW_END        ::      GW_END        :
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

// ============================================================================================================================= //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 3. PROCEDURE_FOR_UPDATING_THE_WILL_DATA **************************************************** //
/*
// Procedure Variables //
int proc3Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int comparatorWILL = 0; // Prevents comparator blocking

// Declaration of State Functions //
void initialState(int signal); // RX Update WILLTOPIC WILLMSG

void usr_wireless_app_task(void) {
	if (stateIndex == 0) // RX Update WILLTOPIC WILLMSG
	{
		initialState(0);
	}
}

void usr_frame_received_cb(frame_info_t *frame) {
	if (enableReception == 1) {
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data to avoid overlap

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x1A) // RX WILLTOPICUPD
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 26; // This variable allows the node to exit the reception loop
		}
		if (msgTypeMQTT_SN == 0x1C) // RX WILLMSGDUPD
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 28; // This variable allows the node to exit the reception loop
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame) {
	// Callback function for transmitted frames
}

void initialState(int signal) { // WAIT WILLTOPIC or WILLMSG S0
	uint8_t State[44] = ":''ESP_WILL_TOP_MSG''::       S-00         :";

	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator
		enableReception = 1; // Variable enabling message receiving function.
		} else if (stateControl == 3) {
		// The node will remain in this state until it receives a WILLTOPICUPD, WILLMSGDUPD, or empty WILLTOPICUPD.
		//////////////////////////////////////////////////////////////////////
		// RX WILLTOPICUPD
		if (successfulReceptionControl == 26) {
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			jump = 1; // Add delay for transmission
			} else if (jump == 1) {
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 27;
			} else if (enableTransmission == 27) {
			enableTransmission = -1; // Transmission disabled because only one message is needed
			uint8_t returnCode = 0x00; // Accepted return
			messageLength = functionWILLTOPICRESP(WILLTOPICRESP, returnCode); // Function invocation to create message for sending, returns message length
			transmit_sample_frame(WILLTOPICRESP, messageLength, 0); // message sending
			messageLength = 0;
			// STATE CHANGE
			proc3Count = 1;
			controlLED(proc3Count);
			temporaryStateIndex = 0; // State change
			stateControl = 4; // exit state
		}
		//////////////////////////////////////////////////////////////////////
		// RX WILLMSGDUPD
		else if (successfulReceptionControl == 28) {
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			comparatorWILL = 1; // Add delay for transmission
			jump = 2;
			} else if (jump == 2) {
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 29;
			} else if (enableTransmission == 29) {
			enableTransmission = -1; // Transmission disabled because only one message is needed
			uint8_t returnCode = 0x00; // Accepted return
			messageLength = functionWILLMSGRESP(WILLMSGRESP, returnCode); // Function invocation to create message for sending, returns message length
			transmit_sample_frame(WILLMSGRESP, messageLength, 0); // message sending
			messageLength = 0;
			// STATE CHANGE
			proc3Count = 2;
			controlLED(proc3Count);
			temporaryStateIndex = 0; // State change
			stateControl = 4; // exit state
		}
		//////////////////////////////////////////////////////////////////////
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of the state
		} else if (stateControl == 5) {
		stateControl = 1;
		transmit_sample_frame((uint8_t *)":      Ends       ::........S-00........:", 44, 1); // state indicator
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 4. PROCEDIMIENTO PARA EL REGISTRO DE NOMBRE DE TEMA **************************************** //
/*
// Procedure Variables //
int proc4Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int twait = 10; // Should be 5 min to reset this procedure
int messageNotice = 0; // Used to send notification messages
int ackComparator = 0; // Prevents blocking in the REGACK comparator

// Declaration of State Functions //
void initialState(void); // WAIT REGISTRATION S0

void state_1(void); // RX_REGACK(from Client) S1

void state_2(void); // CONGESTION S2

void usr_wireless_app_task(void) {
	if (stateIndex == 0) // WAIT REGISTRATION S0
	{
		initialState();
	}
	else if (stateIndex == 1) {
		state_1(); // RX_REGACK(from Client) S1
	}
	else if (stateIndex == 2) {
		state_2(); // CONGESTION S2
	}
}

void usr_frame_received_cb(frame_info_t *frame) {
	if (enableReception == 1) {
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source buffer to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, prevent overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x0A) // RX REGISTER
		{
			enableReception = 0; // Disables reception to avoid interference in state changes.
			successfulReceptionControl = 10; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
		else if (msgTypeMQTT_SN == 0x0B) // RX REGACK
		{
			enableReception = 0; // Disables reception to avoid interference in state changes.
			successfulReceptionControl = 11; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame) {
	// Callback function for transmitted frames, currently empty
}

// State Initial //
void initialState(void) { // WAIT REGISTRATION S0

	uint8_t State[44] = ":'''REGISTRATION''':       S-00         :";
	//                 :'GW_NOTICE':

	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3) {
		// The node will remain in this state until it receives a REGISTER from the Client
		// or when the GW requires registering a topic
		//////////////////////////////////////////////////////////////////////////
		// RX REGISTER
		if (successfulReceptionControl == 10) // Condition is activated when a REGISTER is received
		{
			successfulReceptionControl = -1; // Received the required message, no longer accepts others
			jump = 1; // Add delay for transmission
		}// DELAY
		else if (jump == 1) {
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 11;
		}// TX REGACK
		else if (enableTransmission == 11) // Proceed to transmit a REGACK as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			// TX msg REGACK
			uint8_t ReturnCODE; ReturnCODE = 0x00; // 0x00; 0x01; 0x02; 0x03;
			messageLength = functionREGACK(REGACK, ReturnCODE); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(REGACK, messageLength, 0); // message send
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// It is indicated that it was received: readyRegister == 1 this indication should come from higher layers
		// BUTTON RX HIGHER LAYERS
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate RX readyRegister == 1 a button is used
		{
			delay_ms(200); // Delay for button to work
			messageNotice = 2; // indicate that the condition was met
		}
		// Notice message: readyRegister == 1
		else if (messageNotice == 2) {
			messageNotice = 0; // the notice is sent only once
			transmit_sample_frame((uint8_t *) ":        Ready        ::  readyRegister == 1  :", 44, 1);
			jump = 2;
		}// DELAY
		else if (jump == 2) {
			jump = 0; // Only one jump
			delay_ms(400); // wait time to appreciate the response to a reception
			enableTransmission = 10; // Proceed to send a REGISTER
		}// TX REGISTER
		else if (enableTransmission == 10) // Proceed to transmit a REGISTER as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGISTER message is needed
			uint8_t topicNAME[] = "topicNameGw";
			messageLength = functionREGISTER(REGISTER, topicNAME, (sizeof(topicNAME)) - 1);
			transmit_sample_frame(REGISTER, messageLength, 0); // message send
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of the state
	}
	else if (stateControl == 5) {
		// LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t *) ":      End       ::........S-00........:", 44, 1); // state indicator
		// controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // RX_REGACK From Client S1

uint8_t State[44]  = ":''''RX_REGACK_Clt''''::       S-01         :";
//  :'''''GW_NOTICE''''':

if (stateControl == 1) {
	stateControl = 2;
	delay_ms(1500); // wait time to appreciate the start of state
}
else if (stateControl == 2) {
	stateControl = 3;
	transmit_sample_frame(State, 44, 1); // state indicator
	enableReception = 1; // Variable that enables the function that receives messages.
}
else if (stateControl == 3) {
	// The node will remain in this state until it receives
	//////////////////////////////////////////////////////////////////////////
	// RX REGACK
	if (successfulReceptionControl == 11) // Condition is activated when
	{
		successfulReceptionControl = -1; // Received the required message, no longer accepts others
		ackComparator = 1; //
	}// Comparator
	//////////////////////////////////////////////////////////////////////////
	else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x00) {
		ackComparator = 0;
		proc4Count = 2;
		controlLED(proc4Count); // Indicates how a sequence of messages has been processed
		temporaryStateIndex = 0; // State change
		stateControl = 4;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x02) {
		proc4Count = 0;
		controlLED(proc4Count); // Indicates how a sequence of messages has been processed
		temporaryStateIndex = 0; // State change
		stateControl = 4;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x03) {
		proc4Count = 0;
		controlLED(proc4Count); // Indicates how a sequence of messages has been processed
		temporaryStateIndex = 0; // State change
		stateControl = 4;
	}
	//////////////////////////////////////////////////////////////////////////
	else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x01) {
		ackComparator = 0;
		// SET TIMER TWAIT
		timerDurationTime = twait; // The timer has a duration of 10 seconds for testing
		start_timer1(); // The timer starts its operation
		// STATE CHANGE
		temporaryStateIndex = 2; // State change
		stateControl = 4;
	}
	//////////////////////////////////////////////////////////////////////////
}
else if (stateControl == 4) {
	stateControl = 5;
	delay_ms(1500); // wait time to appreciate the end of the state
}
else if (stateControl == 5) {
	stateControl = 1;
	transmit_sample_frame((uint8_t *) ":      End       ::........S-01........:", 44, 1); // state indicator
	// controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
	stateIndex = temporaryStateIndex; // State change
	temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
}
}
// ****************************************************************************************************************************** //
void state_2(void) { // CONGESTION S2
	
	uint8_t State[44] = ":''''CONGESTION''''::       S-02         :";
	//                 :'''''GW_NOTICE''''':

	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator
	}
	else if (stateControl == 3) {
		// The node will remain in this state until TWAIT time passes
		//////////////////////////////////////////////////////////////////////////
		// TIMER ENDS
		if (TimerEnd == 1) // If the condition indicating that the Tadb time has ended is met
		{
			TimerEnd = 0; // Disable the reception of the timer signal as it has ended
			transmit_sample_frame((uint8_t *) ":     TWAIT        ::      Ends      :", 44, 1);
			//                               :     GEnds       ::       Tadv         :// A message is transmitted indicating that the T adv Timer has ended
			proc4Count = 0;
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			// State change
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of the state
	}
	else if (stateControl == 5) {
		//LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t *) ":      End       ::........S-02........:", 44, 1); // state indicator
		// controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 5. CLIENT'S_PUBLICATION_PROCEDURE ********************************************************** //
/*
// Procedure Variables //
int iniRegistro = 0; // Indicates that another procedure should start
int proc5Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int topicIdComparador = -1; // Allows indicating that a topic ID is not valid
int QoSComparador = 0; // Prevents blocking when comparing the QoS FLAGS field
int selcecPUBACK = 0; // Selects the PUBACK to transmit

// State Function Declarations //
void initialState(void); // WAIT PUBLISH S0

void state_1(void); // RX_PUBREL S1

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // Wait PUBLISH
	{
		initialState();
	}
	else if (stateIndex == 1) // Wait PUBLISH
	{
		state_1();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source buffer to destination
		bmm_buffer_free(frame->buffer_header); // Free buffer data to avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x0C) // RX PUBLISH
		{
			enableReception = 0; // Disable reception to avoid interference in state change.
			successfulReceptionControl = 12; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x10) // RX PUBREL
		{
			enableReception = 0; // Disable reception to avoid interference in state change.
			successfulReceptionControl = 16; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // WAIT PUBLISH S0
	
	uint8_t State[44] = ":''WAIT_PUBLICATION''::        S-00        :";
	                   //:      GW_END        ::........S-00........:"
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a PUBLISH with different QoS values 0,1,2
		// RX PUBLISH
		if (successfulReceptionControl == 12) // The condition is activated when a PUBLISH is received
		{
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			topicIdComparador = 1; // invalid = 0, valid = 1
		}
		//----------------------------------Valid Topic ID------------------------------------//
		else if (topicIdComparador == 1)
		{
			topicIdComparador = -1;
			QoSComparador = 1; // allows comparing the QoS of the PUBLISH message
		}//RX PUBLISH QoS 0
		else if (QoSComparador == 1 && receivedFrame.MQTT_SN_message[2] == 0b00000000)
		{
			QoSComparador = 0;
			proc5Count = 1;
			controlLED(proc5Count); // Indicates that another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change of state
			stateControl = 4;
		}//RX PUBLISH QoS 1
		else if (QoSComparador == 1 && receivedFrame.MQTT_SN_message[2] == 0b00100000)
		{
			QoSComparador = 0;
			jump = 1;
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			selcecPUBACK = 2;
			enableTransmission = 11;
		}//TR PUBACK
		else if (enableTransmission == 11 && selcecPUBACK == 2) // Proceed to transmit a PUBACK as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			selcecPUBACK = 0;
			uint8_t ReturnCode1; ReturnCode1 = 0x00; // 0x00,0x01,0x03
			messageLength = functionPUBACK(PUBACK, ReturnCode1);
			transmit_sample_frame(PUBACK, messageLength, 0); // Message transmission
			messageLength = 0;
			iniRegistro = 2;
			controlLED(iniRegistro); // Indicates that another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change of state
			stateControl = 4;
		}//RX PUBLISH QoS 2
		else if (QoSComparador == 1 && receivedFrame.MQTT_SN_message[2] == 0b01000000)
		{
			QoSComparador = 0;
			jump = 2;
		}
		// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 15;
		}//TR PUBREC
		else if (enableTransmission == 15) // Proceed to transmit a PUBREC as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			messageLength = functionPUBREC(PUBREC); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBREC, messageLength, 0); // Message transmission
			messageLength = 0;
			LED_Toggle(LED2);
			LED_Toggle(LED0);
			
			// STATE CHANGE
			temporaryStateIndex = 1; // Change of state
			stateControl = 4;
		}
		
		//----------------------------------Invalid Topic ID------------------------------------//
		else if (topicIdComparador == 0)
		{
			topicIdComparador = -1;
			jump = 3; // Add delay for transmission
		}// DELAY
		else if (jump == 3)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			selcecPUBACK = 1;
			enableTransmission = 11;
		}//TR PUBACK
		else if (enableTransmission == 11 && selcecPUBACK == 1) // Proceed to transmit a PUBACK as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			selcecPUBACK = 0;
			uint8_t ReturnCodeRechazo2; ReturnCodeRechazo2 = 0x02; // returnCode: invalid topicID
			messageLength = functionPUBACK(PUBACK, ReturnCodeRechazo2);
			transmit_sample_frame(PUBACK, messageLength, 0); // Message transmission
			messageLength = 0;
			iniRegistro = 1;
			controlLED(iniRegistro); // Indicates that another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change of state
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      GW_END        ::........S-00........:", 44, 1); // state indicator
		                               //:      GW_END        ::........S-00........:"
		// controldeEstadoInicial=1; // This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; // Change of state
		temporaryStateIndex = -1; // Set to -1 to be able to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // RX_PUBREL S1
	
	uint8_t State[44] = ":'''''RX_PUBREL''''''::        S-01        :";
	                   //:      GW_END        ::........S-00........:"
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a PUBREL
		// RX PUBREL
		if (successfulReceptionControl == 16) // The condition is activated when a PUBREC is received
		{
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			jump = 1; // Add delay for transmission
		}// DELAY
		else if (jump == 1)
		{
			jump = 0;
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 14; // Proceed to send a PUBREL
		}
		else if (enableTransmission == 14) // Proceed to transmit a PUBCOMP as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a WILLTOPICREQ message is needed
			messageLength = functionPUBCOMP(PUBCOMP); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBCOMP, messageLength, 0); // Message transmission
			messageLength = 0;
			proc5Count = 3;
			controlLED(proc5Count); // Indicates how this procedure ends
			// State change
			temporaryStateIndex = 0; // Change of state
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      GW_END        ::........S-02........:", 44, 1); // state indicator
		                               //:      GW_END        ::........S-00........:"
		// controldeEstadoInicial=1; // This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; // Change of state
		temporaryStateIndex = -1; // Set to -1 to be able to use it later.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 6. PUBLICATION_WITH_QOS_LEVEL_-1 *********************************************************** //
/*
// Procedure Variables //
int brokerAnnouncement = 0;
int proc6Count = 0;

// Declaration of State Functions //
void initialState(void); // INACTIVE(QoS-1) S0

void state_1(void);     // WAIT PUBLISH(QoS-1) S1

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // INACTIVE(QoS-1) S0
	{
		initialState();
	}
	else if (stateIndex == 1) // WAIT PUBLISH(QoS-1) S1
	{
		state_1();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Allocate memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x0C) // RX PUBLISH
		{
			enableReception = 0; // Disable reception to avoid interference with state change.
			successfulReceptionControl = 12; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void){// INACTIVE(QoS-1) S0
	
	uint8_t State[44]=":''INACTIVE(QoS -1)''::       S-00         :";
	//                  :''''''INACTIVE''''''::       S-00         :
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500);// Waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // Message to indicate the current state
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until the broker and gateway
		// start their operation; otherwise, the state control variable
		// will remain the same value.
		///////////////////////////////////////////////////////////////////////////////////////
		// Broker ready to operate
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the connection establishment between the gateway and the server, a push button is used
		{
			delay_ms(200); // Delay for the button to work
			brokerAnnouncement = 1;
		}
		else if (brokerAnnouncement == 1)
		{
			brokerAnnouncement = 0;
			transmit_sample_frame((uint8_t*)":        broker      ::       Ready==1     :", 44, 1); // State indicator
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)":        GFIN        ::........S-00........:", 44, 1); // State indicator
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

void state_1(void){// WAIT PUBLISH(QoS-1) S1
	
	uint8_t State[44]=":''WAIT_PUBLISHQoS-1''::       S-00         :";
	//                  :'''''GW_ANNOUNCEMENT''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a PUBLISH
		// ------
		//////////////////////////////////////////////////////////////////////////
		// RX PUBLISH QoS-1
		if (successfulReceptionControl == 12) // The condition is activated when a PUBLISH is received
		{
			successfulReceptionControl = -1; // The required message has been received and no other messages are accepted
			proc6Count = 1;
			controlLED(proc6Count);
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		// Broker stops working
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the connection establishment between the gateway and the server, a push button is used
		{
			delay_ms(200); // Delay for the button to work
			brokerAnnouncement = 1;
		}
		else if (brokerAnnouncement == 1)
		{
			brokerAnnouncement = 0;
			transmit_sample_frame((uint8_t*)":        broker      ::       Ready==0     :", 44, 1); // State indicator
			controlLED(2);
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":         Ends         ::........S-00........:", 44, 1); // State indicator
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 7. CLIENT’S_TOPIC_SUBSCRIBE_OR_UN-SUBSCRIBE_PROCEDURE ************************************** //
/*
// Procedure Variables //
int proc7Count = 0;

// Declaration of State Functions //
void initialState(void); // WAITING SUBSCRIPTION or UNSUBSCRIBE S0

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // WAITING SUBSCRIPTION or UNSUBSCRIBE S0
	{
		initialState();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x12) // RX SUBSCRIBE
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 18; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x14) // RX UNSUBSCRIBE
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 20; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}

	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void){ // WAITING SUBSCRIPTION or UNSUBSCRIBE
	
	uint8_t State[44] = ":'''WAIT_SUBSCRIPTION''::       S-00         :";
	//                  :'''''GW_ANNOUNCEMENT''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Variable enabling the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a SUBSCRIBE or UNSUBSCRIBE
		//////////////////////////////////////////////////////////////////////////
		// RX SUBSCRIBE
		if (successfulReceptionControl == 18) // The condition is activated when a SUBSCRIBE is received
		{
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			jump = 1; // Add delay for transmission
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Waiting time to appreciate the response to a reception
			
			enableTransmission = 19;
		}// TR SUBACK
		else if (enableTransmission == 19) // Proceed to transmit a SUBACK as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only a SUBACK message is needed
			uint8_t ReturnCODE; ReturnCODE = 0x00; // 0x00; 0x01; 0x02; 0x03;//mod
			uint8_t flagsSK; flagsSK = 0b01100000; // 0b00000000; 0b00100000; 0b01000000; 0b01100000;
			messageLength = functionSUBACK(SUBACK, flagsSK, ReturnCODE);
			transmit_sample_frame(SUBACK, messageLength, 0); // Message transmission
			messageLength = 0;
			proc7Count = 1;
			controlLED(proc7Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAITING SUBSCRIPTION or UNSUBSCRIBE state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// RX UNSUBSCRIBE
		else if (successfulReceptionControl == 20) // The condition is activated when an UNSUBSCRIBE is received
		{
			successfulReceptionControl = -1; // Received the required message, no longer accept others
			jump = 2; // Add delay for transmission
		}// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Waiting time to appreciate the response to a reception
			
			enableTransmission = 21;
		}// TR UNSUBACK
		else if (enableTransmission == 21) // Proceed to transmit an UNSUBACK as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only an UNSUBACK message is needed
			messageLength = functionUNSUBACK(UNSUBACK);
			transmit_sample_frame(UNSUBACK, messageLength, 0); // Message transmission
			messageLength = 0;
			proc7Count = 2;
			controlLED(proc7Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAITING SUBSCRIPTION or UNSUBSCRIBE state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		//LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      End       ::........S-00........:", 44, 1); // State indicator
		//controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 8. GATEWAY'S_PUBLICATION_PROCEDURE ********************************************************* //
/*
// Procedure Variables //
int proc8Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int twait = 10; // int twait=300; Should be 5 min to restart this procedure
int registrationInitiated = 0; // Indicates that another procedure should start
int ackComparator = 0; // Prevents blockages when receiving PUBACK
int qos = -1; // Prevents blockages when comparing messages with QoS flags
int pendingPublications = 3; // Indicates that the gateway needs to make one or more publications
int pubPendingComparator = 1; // Aids in comparisons of the publication counter
int pulseCounter = 0; // Helps choose a message with a specific QoS
int receivedPulses = 0; //

// Declaration of State Functions //
void initialState(void); // WAIT S0

void state_1(void);     // ACTIVE PUBLICATION S1

void state_2(void);     // RX_ACK S2

void state_3(void);     // RX_PUBCOMP S3

void state_4(void);     // CONGESTION S4

void usr_wireless_app_task(void)
{
	if (stateIndex == 0)       // WAIT S0
	{
		initialState();
	}
	else if (stateIndex == 1)  // ACTIVE PUBLICATION S1
	{
		state_1();
	}
	else if (stateIndex == 2)  // RX_ACK S2
	{
		state_2();
	}
	else if (stateIndex == 3)  // RX_PUBCOMP S3
	{
		state_3();
	}
	else if (stateIndex == 4)  // CONGESTION S4
	{
		state_4();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
	{

		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserves memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copies information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoids overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		if (msgTypeMQTT_SN == 0x0D) // RX PUBACK
		{
			enableReception = 0; // Disables reception to avoid interference with state changes.
			successfulReceptionControl = 13; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x0F) // RX PUBREC
		{
			enableReception = 0; // Disables reception to avoid interference with state changes.
			successfulReceptionControl = 15; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x0E) // RX PUBCOMP
		{
			enableReception = 0; // Disables reception to avoid interference with state changes.
			successfulReceptionControl = 14; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue with its operation.
		}

	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // WAIT S0

	uint8_t State[44] = ":'''''''WAIT'''''''::       S-00         :";
	//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // RX_Connect state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it has pending publications >=1
		//////////////////////////////////////////////////////////////////////////
		//
		if (pendingPublications >= 1 && pubPendingComparator == 1)
		{
			pubPendingComparator = 0;
			delay_ms(2000); // waiting time
			jump = 1; // Switches to use a delay
		} // DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			transmit_sample_frame((uint8_t *)":    publications   ::    pending >=1   :", 44, 1);
			//nt8_t State[44]="             :'''ESP_SUBSCRIPTION''::       S-00         :";
			// STATE CHANGE
			pubPendingComparator = 1;
			temporaryStateIndex = 1; //
			stateControl = 4;
		}//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t *)":      End       ::........S-00........:", 44, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // ACTIVE PUBLICATION S1

	uint8_t State[44] = ":'''ACTIVE PUBLICATION''::       S-01         :";
	//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // RX_Connect state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until the publication data is ready
		// or receive a PUBACK for invalid topicID
		//////////////////////////////////////////////////////////////////////////
		// PUSH RX BUTTON UPPER LAYERS
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate a publication, a button is used
		{
			delay_ms(200); // Delay for the button to work
			pulseCounter++;
			if (pulseCounter == 1)
			{
				addTimer = 1; // When the first pulse arrives, the Timer configuration will continue
			}
		} // PULSE TIMER
		else if (addTimer == 1)
		{
			addTimer = 0; // The timer is only assigned once so the variable returns to 0
			// SET TIMER
			timerDurationTime = 3; // The timer lasts 5 seconds to collect other pulses
			start_timer1(); // The timer starts its operation
		} // The Tadv Timer finishes ListoNuevoWILLTOPIC = TRUE
		else if (TimerEnd == 1) // If the condition indicating that the Tadb time ends is met
		{
			TimerEnd = 0; // Disable the reception of the timer signal as it has ended
			receivedPulses = pulseCounter; // unnecessary pulses are avoided
			pulseCounter = 0;
		}
		//////////////////////////////////////////////////////////////////////////
		// Prepare PUBLISH (QoS 0)
		else if (receivedPulses == 1)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t *)":  publication list  ::     (QoS 0)==1     :", 44, 1);
			//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
			jump = 1; // Switches to use a delay
		} // DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(400); // waiting time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 0;
		} // TR PUBLISH Prepare PUBLISH (QoS 0)
		else if (enableTransmission == 12 && qos == 0) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS0"; uint8_t flagsP; flagsP = 0b00000000; //flagsP field review MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // message send
			messageLength = 0;
			proc8Count = 1; // indicates that this sequence is finished
			controlLED(proc8Count);
			pendingPublications = pendingPublications - 1; // indicates that the pending publication has been served
			// STATE CHANGE
			temporaryStateIndex = 1; // PUBLICATION
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// Prepare PUBLISH (QoS 1)
		else if (receivedPulses == 2)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t *)":  publication list  ::     (QoS 1)==1     :", 44, 1);
			//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
			jump = 2; // Switches to use a delay
		} // DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(400); // waiting time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 1;
		} // TR PUBLISH
		else if (enableTransmission == 12 && qos == 1) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS1"; uint8_t flagsP; flagsP = 0b00100000; //flagsP field review MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // message send
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 2; // move to RX_ACK state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// Prepare PUBLISH (QoS 2)
		else if (receivedPulses == 3)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t *)":  publication list  ::     (QoS 2)==1     :", 44, 1);
			//             nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
			jump = 3; // Switches to use a delay
		} // DELAY
		else if (jump == 3)
		{
			jump = 0; // Only one jump
			delay_ms(400); // waiting time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 2;
		} // TR PUBLISH
		else if (enableTransmission == 12 && qos == 2) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS2"; uint8_t flagsP; flagsP = 0b01000000; //flagsP field review MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // message send
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 2; // move to RX_ACK state
			stateControl = 4;
		}
		///////////////////////////////////////////////////////////////////
		// RX PUBACK
		else if (successfulReceptionControl == 13) // The condition is activated when a PUBACK is received
		{
			successfulReceptionControl = -1; // The required message has been received and no others are accepted
			ackComparator = 1; // Proceed to check the returnCode field
		} // Searches: returnCode 0x02 = rejection of invalid topic ID
		//////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x02)
		{
			ackComparator = 0;
			registrationInitiated = 1; // indicates that this sequence is finished
			controlAllLEDs(registrationInitiated);
			pendingPublications = pendingPublications - 1; // indicates that the pending publication has been served
			//STATE CHANGE
			temporaryStateIndex = 1; // returns to the ACTIVE PUBLICATION state
			stateControl = 4;
		}
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] != 0x02)
		{
			ackComparator = 0;
			//STATE CHANGE
			temporaryStateIndex = 1; // returns to the ACTIVE PUBLICATION state
			stateControl = 4;
		} // pendingPublications = 0
		//////////////////////////////////////////////////////////////////////////
		else if (pendingPublications == 0 && pubPendingComparator == 1)
		{
			pubPendingComparator = 0;
			delay_ms(3000); // waiting time
			jump = 6; // Switches to use a delay
		} // DELAY
		else if (jump == 6)
		{
			jump = 0; // Only one jump
			pubPendingComparator = 1;
			controlAllLEDs(1);
			transmit_sample_frame((uint8_t *)":    publications   ::    pending==0   :", 44, 1);
			// STATE CHANGE
			temporaryStateIndex = 0; // WAIT state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t *)":      End       ::........S-01........:", 44, 1); // State indicator
		//transmit_sample_frame((uint8_t*)":      Termina       ::........S-00........:",44,1);// state indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be used later.
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ****************************************************************************************************************************** //
void state_2(void) { // RX_ACK S2

	uint8_t State[44] = ":'''''''RX_ACK'''''''::       S-02         :";
	//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // time to wait to appreciate the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator RX_Connect
		enableReception = 1; // Variable that enables the function that receives messages.
		} else if (stateControl == 3) {

		// The node will remain in this state until it receives PUBACK or PUBREC
		//////////////////////////////////////////////////////////////////////////
		// RX PUBACK
		if (successfulReceptionControl == 13) { // The condition is activated when a PUBACK is received
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			ackComparator = 1; // Adds delay for transmission
		} // Comparator PUBACK ACCEPTED
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x00) {
			ackComparator = 0;
			proc8Count = 2; // indicates how the procedure has ended
			controlLED(proc8Count); //
			pendingPublications = pendingPublications - 1; // indicates that the pending publication has been served
			// State Change
			temporaryStateIndex = 1; // PUBLISH
			stateControl = 4;
		} // Comparator PUBACK invalid topicID
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x02) {
			ackComparator = 0;
			registrationInitiated = 1;
			controlAllLEDs(registrationInitiated); // indicates how the procedure has ended
			// State Change
			temporaryStateIndex = 1; // PUBLISH
			stateControl = 4;
		} // Comparator PUBACK not supported
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x03) {
			ackComparator = 0;
			controlAllLEDs(0); // indicates how the procedure has ended
			// State Change
			temporaryStateIndex = 1; // PUBLISH
			stateControl = 4;
		} // Comparator PUBACK congestion
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x01) {
			ackComparator = 0;
			// SET TIMER TWAIT
			timerDurationTime = twait; // The timer lasts 10 seconds for testing
			start_timer1(); // The timer starts its operation
			// State Change
			temporaryStateIndex = 4; // CONGESTION STATE
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// RX PUBREC
		if (successfulReceptionControl == 15) { // The condition is activated when a PUBREC is received
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Add transmission delay
		} // DELAY
		else if (jump == 1) {
			jump = 0;
			delay_ms(4000); // time to wait to appreciate the response to a reception
			enableTransmission = 16; // Proceed to send a PUBREL
			} else if (enableTransmission == 16) { // Proceed to transmit a PUBREL as indicated by the variable value
			enableTransmission = -1; // Transmission is disabled because only a message WILLTOPICREQ is needed
			messageLength = functionPUBREL(PUBREL); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBREL, messageLength	, 0); // message sending
			messageLength = 0;
			
			// State Change
			temporaryStateIndex = 3; // RX_PUBCOMP STATE
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // time to wait to appreciate the end of the state
		} else if (stateControl == 5) {
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t *) ":      End       ::........S-02........:", 44, 1); // State indicator
		//transmit_sample_frame((uint8_t*)":      Termina       ::........S-00........:",44,1);// state indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //
void state_3(void) { // RX_PUBCOMP STATE S3

	uint8_t State[44] = ":'''''RX_PUBCOMP'''''::        S-03        :";
	//nt8_t State[44]=":'''''''RX_ACK'''''''::        S-01        :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // time to wait to appreciate the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator RX_Connect
		enableReception = 1; // Variable that enables the function that receives messages.
		} else if (stateControl == 3) {
		//////////////////////////////////////////////////////////////////////////
		// The node will remain in this state until it receives a PUBCOMP message
		//////////////////////////////////////////////////////////////////////////
		// RX PUBCOMP
		if (successfulReceptionControl == 14) { // The condition is activated when a PUBCOMP is received
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			proc8Count = 3;
			controlLED(proc8Count);
			pendingPublications = pendingPublications - 1;
			temporaryStateIndex = 1; // Change to the PUBLISH state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // time to wait to appreciate the end of the state
		} else if (stateControl == 5) {
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t *) ":      End       ::........S-03........:", 44, 1); // State indicator
		//tnsmit_sample_frame((uint8_t*)":      Termina       ::........S-00........:",44,1);// state indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // Change of state to RX_TOPIC STATE
		temporaryStateIndex = -1; // It is set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //
void state_4(void) { // CONGESTION STATE S4

	uint8_t State[44] = ":''''CONGESTION''''::       S-04         :";
	//nt8_t State[44]=":'''ESP_SUBSCRIPTION''::       S-00         :";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // time to wait to appreciate the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // state indicator RX_Connect
		} else if (stateControl == 3) {
		// The node will remain in this state until the TWAIT time has passed
		//////////////////////////////////////////////////////////////////////////
		// TIMER FINISHES
		if (TimerEnd == 1) { // If the condition indicating that the time Tadb has ended is met
			TimerEnd = 0; // Disable the reception of the timer signal since it has ended
			transmit_sample_frame((uint8_t *) ":       TWAIT        ::      Finished      :", 44, 1);
			//                               :      End       ::........S-00........:",44
			proc8Count = 4;
			controlLED(proc8Count);
			// State Change
			temporaryStateIndex = 1; // Moves to the PUBLISH state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // time to wait to appreciate the end of the state
		} else if (stateControl == 5) {
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t *) ":      End       ::........S-04........:", 44, 1); // State indicator
		//tnsmit_sample_frame((uint8_t*)":      End       ::........S-00........:",44,1);// state indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // Change of state to RX_TOPIC STATE
		temporaryStateIndex = -1; // It is set to -1 to be used later.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 9. KEEP_ALIVE_AND_PING_PROCEDURE *********************************************************** //
/*
// Procedure Variables //
int duration = 20; // Duration established during the connection
int tkeepAlive = 0; // Duration of the TKA timer
int lostClient = 0; // Client is considered lost when the timer ends
bool random_1 = true; // Initiates the first timer
int proc9Count = 0;
int auxTimer = 0;
int messageAlert = 0;

// State Function Declarations //
void initialState(void); // WAIT PINGREQ S0

void state_1(void); // RX_PINGRESP S1

void usr_wireless_app_task(void)
{
	if (random_1 == true) // Assignment of the TKA timer
	{
		random_1 = false;
		// Assignment of retransmission times
		tkeepAlive = duration + (duration * 0.5);
		timerDurationTime = tkeepAlive; // The timer has a duration of 15 seconds*NADV for tests (TADV must have a duration of 15 min)
		start_timer1(); // The timer starts its operation
		// auxTimer=2;
	}
	if (stateIndex == 0) // WAIT PINGREQ S0
	{
		initialState();
	}
	else if (stateIndex == 1) // RX_PINGRESP S1
	{
		state_1(); //
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x16) // RX PINGREQ
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 22; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue its operation.
		}
		else if (msgTypeMQTT_SN == 0x17) // RX PINGRESP
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 23; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // WAIT PINGREQ S0

	uint8_t State[44] = ":'''WAIT_PINGREQ'''::       S-00         :";
	//                  :'''''GW_ANUNCIO''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives RX PINGREQ
		// or the value Monitor Client=TRUE from upper layers
		//////////////////////////////////////////////////////////////////////////
		// RX PINGREQ
		if (successfulReceptionControl == 22) // Activated when a PINGREQ is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			stop_timer1();
			jump = 1; // Delay added for transmission
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to appreciate the response to a reception
			enableTransmission = 23; // Proceed to send a PINGRESP
		}// TR PINGRESP
		else if (enableTransmission == 23) // Proceed to transmit a PINGRESP as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PINGRESP message is needed
			messageLength = functionPINGRESP(PINGRESP);
			transmit_sample_frame(PINGRESP, messageLength, 0); // Message sending
			messageLength = 0;
			tkeepAlive = duration + (duration * 0.5);
			timerDurationTime = tkeepAlive; // The timer has a duration of 30 seconds*NADV for tests (TADV must have a duration of 15 min)
			start_timer1(); // The timer restarts its operation
			proc9Count = 1;
			controlLED(proc9Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT PINGREQ state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// BUTTON listaRequest==1
		else if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate listaRequest==1 a button is used
		{
			delay_ms(200); // Delay for the button to work
			stop_timer1();
			messageAlert = 2; // Indicates that the condition has been met
		}
		// Alert Message: Monitor Client=TRUE
		else if (messageAlert == 2)
		{
			messageAlert = 0; // The alert is sent only once
			transmit_sample_frame((uint8_t*)":       list        ::   Request==1   :", 44, 1);
			//                               :      End       ::........S-00........:
			jump = 2;
		}
		// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Wait time to appreciate the response to a reception
			enableTransmission = 22; // Proceed to send a PINGREQ
		}// TX msg PINGREQ
		else if (enableTransmission == 22) // Proceed to transmit a PINGREQ as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PINGREQ message is needed
			messageLength = functionPINGREQ(PINGREQ);
			transmit_sample_frame(PINGREQ, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // RX_PINGRESP state
			stateControl = 4;
		}// TKA timer ends
		//////////////////////////////////////////////////////////////////////////
		else if (TimerEnd == 1)
		{
			TimerEnd = 0;
			transmit_sample_frame((uint8_t*)":    TKA:Ends    ::  client:Lost   :", 44, 1);
			//transmit_sample_frame((uint8_t*)":      client      ::      Lost=1     :", 44, 1);
			lostClient = 1;
			controlAllLEDs(lostClient);
			// STATE CHANGE
			temporaryStateIndex = 0; // WAIT PINGREQ state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      End       ::........S-00........:", 44, 1); // State indicator
		//controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // RX_PINGRESP S1

	uint8_t State[44] = ":'''''RX_PINGRESP''''::       S-01         :";
	//  :'''''GW_ANUNCIO''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives PINGRESP
		//////////////////////////////////////////////////////////////////////////
		// RX PINGRESP
		if (successfulReceptionControl == 23) // Activated when a PINGRESP is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			tkeepAlive = duration + (duration * 0.5);
			timerDurationTime = tkeepAlive; // The timer has a duration of 15 seconds*NADV for tests (TADV must have a duration of 15 min)
			start_timer1(); // The timer restarts its operation
			proc9Count = 2;
			controlLED(proc9Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // WAIT PINGREQ state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		//LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      End       ::........S-01........:", 44, 1); // State indicator
		//controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 10. CLIENT’S_DISCONNECT_PROCEDURE ********************************************************** //
/*
// Procedure Variables //
int proc10Count = 0; // Indicates how a message sequence ends in this procedure
int prepareAlertInfo = 0;

// State Function Declarations //
void initialState(void); // WAIT DISCONNECT S0

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // WAIT DISCONNECT S0
	{
		initialState();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x18) // RX DISCONNECT
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 24; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue with its operation.
		}

	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // WAIT DISCONNECT S0
	
	uint8_t State[44] = "''WAIT_DISCONNECT''::       S-00         :";
	//                  :'''''GW_ANNOUNCEMENT''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a DISCONNECT or if there is an Unknown Client
		//////////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		if (successfulReceptionControl == 24) // Activated when a DISCONNECT is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Delay added for transmission
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to appreciate the response to a reception
			
			enableTransmission = 24;
		}//TR DISCONNECT
		else if (enableTransmission == 24) // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			messageLength = functionDISCONNECT(DISCONNECT, 'd');
			transmit_sample_frame(DISCONNECT, messageLength, 0); // Message sending
			messageLength = 0;
			proc10Count = 1;
			controlLED(proc10Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT DISCONNECT state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		// Unknown Client == 1
		else if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate Unknown Client == TRUE
		{
			delay_ms(200); // Delay for button to work
			prepareAlertInfo = 1;
		}
		else if (prepareAlertInfo == 1)
		{
			prepareAlertInfo = 0;
			transmit_sample_frame((uint8_t*)":    client    ::   Unknown==1   :", 44, 1);
			jump = 2;
		}
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(200); // Wait time to send DISCONNECT
			enableTransmission = 24; // Proceed to send DISCONNECT
		}//TR DISCONNECT
		else if (enableTransmission == 24) // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			messageLength = functionDISCONNECT(DISCONNECT, 'd');
			transmit_sample_frame(DISCONNECT, messageLength, 0); // Message sending
			messageLength = 0;
			proc10Count = 2;
			controlLED(proc10Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT DISCONNECT state
			stateControl = 4;
		}//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		//LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":      End       ::........S-00........:", 44, 1); // State indicator
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 11. CLIENT’S_RETRANSMISSION_PROCEDURE ****************************************************** //
/*

//
//        To test this procedure, it is not necessary to encode any state on the gateway side.  
//
// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 12. SUPPORT_OF_SLEEPING_CLIENTS ************************************************************ //
/*
// Procedure Variables //
int pendingPublications = -1; // Indicates if there are pending publication messages
int durationS = 0; // Should wait for a DISCONNECT indicating sleep duration
int clientStatus = 0; // Client Sleeping (clientStatus=1), Client Awake (clientStatus=2), Client Lost (clientStatus=3)
int proc12Count = 0; // Indicates how a message sequence ends in this procedure
int prepareWarningInfo = 0; // Used to send warnings when required
int disconnectCounter = 0; // Helps avoid blockages when sending a disconnect

// State Function Declarations //
void initialState(void); // WAIT SUPERVISION S0

void state_1(void); // PUBLICATION S1

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // WAIT SUPERVISION S0
	{
		initialState();
	}
	if (stateIndex == 1) // PUBLICATION S1
	{
		state_1();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1)
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x18) // RX DISCONNECT
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 24; // This variable allows the node to exit the reception loop
			// where it is located and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x16) // RX PINGREQ
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 22; // This variable allows the node to exit the reception loop
			// where it is located and allows the state to continue with its operation.
		}

	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // WAIT SUPERVISION S0
	
	uint8_t State[44] = "''WAIT_SUPERVISION''::      S-00        :";
	//                    ''GW_ANNOUNCEMENT'':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a message, DISCONNECT 18,
		// DISCONNECT(sleepDuration) 18-24, PINGREQ 16-22, or the Timer Finalize
		//////////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		if (successfulReceptionControl == 24 && receivedFrame.MQTT_SN_message[0] == 0x02) // The condition is activated when a CONNECT message 04 is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			stop_timer1(); // Timers are stopped
			jump = 2; // Add delay due to transmission
		}// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Waiting time to appreciate the response to a reception
			enableTransmission = 24;
			disconnectCounter = 1;
		}// TX DISCONNECT confirmation (State CL=DISCONNECTED unsupervised)
		else if (enableTransmission == 24 && disconnectCounter == 1) // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			disconnectCounter = 0;
			messageLength = functionDISCONNECT(DISCONNECT,'d'); // (A time should be added for the node to sleep)
			transmit_sample_frame(DISCONNECT, messageLength, 0); // Message sending
			messageLength = 0;
			proc12Count = 1;
			controlAllLEDs(proc12Count); // Indicates how a message sequence has been processed
			//transmit_sample_frame((uint8_t*)":    State CL=    ::     DISCONNECTED     :",44,1); // Not necessary
			// State change
			temporaryStateIndex = 0; // WAIT SUPERVISION
			stateControl = 4;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////
		// DISCONNECT (sleepDuration)
		else if (successfulReceptionControl == 24 && receivedFrame.MQTT_SN_message[0] == 0x04) // The condition is activated when a CONNECT message 04 is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			stop_timer1(); // Timers are stopped
			jump = 3; // Add delay due to transmission
		}// DELAY
		else if (jump == 3)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Jump to avoid TX problems
			prepareWarningInfo = 3;
		}
		else if (prepareWarningInfo == 3) // Indicates the state of the Client = SLEEPING
		{
			prepareWarningInfo = 0;
			transmit_sample_frame((uint8_t*)":  Client State:  ::       SLEEPING     :",44,1);
			addTimer = 1;
		}
		else if (addTimer == 1)
		{
			addTimer = 0; // The timer is assigned only once, so the variable returns to 0
			int Duration = 20; // 20 s for testing; should be: Duration = receivedFrame.MQTT_SN_message[4];
			durationS = Duration + (0.5 * Duration); // Duration should be 50% greater for times less than 1 min
			timerDurationTime = durationS; //
			start_timer1(); // Timer starts its operation
			jump = 5;
		}
		else if (jump == 5)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Waiting time to appreciate the response to a reception
			enableTransmission = 24;
			disconnectCounter = 2;
		}// TX DISCONNECT confirmation (State CL=DISCONNECTED unsupervised)
		else if (enableTransmission == 24 && disconnectCounter == 2) // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			disconnectCounter = 0;
			messageLength = functionDISCONNECT(DISCONNECT,'d'); // (A time should be added for the node to sleep)
			transmit_sample_frame(DISCONNECT, messageLength, 0); // Message sending
			messageLength = 0;
			clientStatus = 1;
			controlLED(clientStatus); // Indicates how a message sequence has been processed
			// State change
			temporaryStateIndex = 0; // WAIT SUPERVISION
			stateControl = 4;
		}
		///////////////////////////////////////////////////////////////////////////////////////////
		// RX PINGREQ
		else if (successfulReceptionControl == 22) // The condition is activated when a PINGREQ message is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			stop_timer1(); // Timers are stopped
			jump = 4; // Add delay due to transmission
		}// DELAY
		else if (jump == 4)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Waiting time to appreciate the response to a reception
			prepareWarningInfo = 4;
		}
		else if (prepareWarningInfo == 4) // Indicates the state of the Client = AWAKE
		{
			prepareWarningInfo = 0;
			transmit_sample_frame((uint8_t*)":  Client State:  ::       AWAKE     :",44,1);
			//                               :   State CL=   ::     ACTIVE     :
			clientStatus = 2;
			controlLED(clientStatus); // Indicates how a message sequence has been processed
			// State change
			pendingPublications = 0; // Set to 0 for later use
			temporaryStateIndex = 1; // PUBLICATION STATE
			stateControl = 4;
		}
		/////////////////////////////////////////////////////////////////////////////////////
		// Timer Ts Ends
		else if (TimerEnd == 1) // If the condition indicating that the time has ended is met
		{
			TimerEnd = 0; // Disable the reception of the timer signal since it has ended
			transmit_sample_frame((uint8_t*)":     TIMER     ::     Ts:Ends    :",44,1);
			//                               :   State CL=   ::     ACTIVE     :
			jump = 7; // Moves to use a delay
		}
		else if (jump == 7)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Waiting time to appreciate the response to a reception
			prepareWarningInfo = 6; //
		}
		else if (prepareWarningInfo == 6) // Indicates the state of the Client = LOST
		{
			prepareWarningInfo = 0;
			transmit_sample_frame((uint8_t*)":  Client State:  ::       LOST     :",44,1);
			//                               :   State CL=   ::     ACTIVE     :
			clientStatus = 3;
			controlLED(clientStatus); // Indicates how a message sequence has been processed
			// State change
			temporaryStateIndex = 0; // WAIT SUPERVISION
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		//LED_Off(LED1);
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":    End     ::........S-00........:",44,1); // State indicator
		//controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

void state_1(void) { // PUBLICATION S1
	
	uint8_t State[44] = "''''PUBLICATION''''::      S-01        :";
	//                    '''''GW_ANOUNCEMENT''''':
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 44, 1); // State indicator
		enableReception = 1; // Enable function to receive messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state if:
		// If it has pending publications (Should execute a Publication procedure),
		// If it has no pending publications, it sends a PINGRESP message
		//////////////////////////////////////////////////////////////////////////
		// There are pending publications
		if (pendingPublications == 0) // The condition is activated when a PINGRESP message is received
		{
			pendingPublications = -1;
			delay_ms(5000); // Waiting time to appreciate TX
			prepareWarningInfo = 4;
		}
		else if (prepareWarningInfo == 4) // Indicates the state of the Client = SLEEPING
		{
			prepareWarningInfo = 0;
			transmit_sample_frame((uint8_t*)":   Publications:   ::   Pending==0   :",44,1);
			jump = 4;
		}
		else if (jump == 4)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Jump to avoid TX problems
			enableTransmission = 23;
		}
		// TX: PINGRESP
		else if (enableTransmission == 23) // Proceed to transmit a TX: PINGRESP as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PINGRESP message is needed
			messageLength = functionPINGRESP(PINGRESP);
			transmit_sample_frame(PINGRESP, messageLength, 0); // Message sending
			messageLength = 0;
			jump = 1;
		}
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Jump to avoid TX problems
			prepareWarningInfo = 3;
		}
		else if (prepareWarningInfo == 3) // Indicates the state of the Client = SLEEPING
		{
			prepareWarningInfo = 0;
			transmit_sample_frame((uint8_t*)":  Client State:  ::       SLEEPING      :",44,1);
			addTimer = 1;
		}// Timer Ts
		else if (addTimer == 1)
		{
			addTimer = 0; // The timer is assigned only once, so the variable returns to 0
			int Duration = 20; // 20 s for testing; should be: Duration = receivedFrame.MQTT_SN_message[4];
			durationS = Duration + (0.5 * Duration); // Duration should be 50% greater for times less than 1 min
			timerDurationTime = durationS; //
			start_timer1(); // Timer starts its operation
			clientStatus = 1;
			controlLED(clientStatus); // Indicates how a message sequence has been processed
			// State change
			temporaryStateIndex = 0; // WAIT SUPERVISION
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)":   End    ::........S-01........:",44,1); // State indicator
		//controldeEstadoInicial=1;// This variable must be set to 1 so that the initial state can operate again.
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// Aditional Funtion//
// ****************************************************************************************************************************** //
void controlLED(int led_on){
	if (led_on==0)
	{
		LED_Off(LED0);
		LED_Off(LED1);
		LED_Off(LED2);
	}
	else if (led_on==1)
	{
		LED_On(LED0);
		LED_Off(LED1);
		LED_Off(LED2);
	}
	else if (led_on==2)
	{
		LED_Off(LED0);
		LED_On(LED1);
		LED_Off(LED2);
	}
	else if (led_on==3)
	{
		LED_Off(LED0);
		LED_Off(LED1);
		LED_On(LED2);
	}
	else if (led_on==4)
	{
		LED_On(LED0);
		LED_On(LED1);
		LED_Off(LED2);
	}
}
void controlAllLEDs(int leds_on){
	if (leds_on==0)
	{
		LED_Off(LED0);
		LED_Off(LED1);
		LED_Off(LED2);
	}
	else if (leds_on==1)
	{
		LED_On(LED0);
		LED_On(LED1);
		LED_On(LED2);
	}
}

