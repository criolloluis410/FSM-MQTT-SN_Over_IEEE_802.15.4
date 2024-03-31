/* ********************************************************************************************************************************
ESCUELA POLITECNICA NACIONAL
ELECTRICAL AND ELECTRONIC ENGINEERING FACULTY
 													
TOPIC: DEVELOPMENT OF THE FINITE STATE MACHINE OF THE MQTT-SN PROTOCOL
       FOR OPERATION OVER IEEE 802.15.4 IN LINEAR TOPOLOGIES
***********************************************************************************************************************************/
// Libraries //
// *********************************************************************************************************************************
#include "usr_wireless.h"
#include "wireless_config.h"
#include "periodic_timer.h"
#include "Messages_MQTT_SN.h" // Allows the use of MQTT-SN messages(MSG).
// ********************************************************************************************************************************
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
//int TimerEnd = 0; // Variable to indicate that the timer time has ended
int maxTimer = 0; // Maximum number of times the timer is restarted
int jump = 0; // It is used to assign a Delay after using the Transmit_sample_frame() function
int auxTimer = 0;
//*********************************************************************************************************************************

// ********************************************************** CLIENT NODE ****************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 1. GATEWAY_ADVERTISEMENT_AND_DISCOVERY_PROCEDURE ******************************************* //
/*

// Procedure variables //
int tsearchgw = 5; // Random time range
int NADV = 2; // Maximum number of times the node waits for announcement messages
int durationRx = 25; // ADVERTISE message duration field (int durationRx = 900;)
// variables to control timer times and the number of retries
int tini = 0;
int trtx = 0;
int updatedGwTable = 0;
int NTadv = 0;
bool random1 = true;

// State Function Declarations //
void initialState(void); // DISCOVERY

void state_1(void); // RX_GWINFO

// Function where state functions are executed //
void usr_wireless_app_task(void)
{
	if (random1 == true) // Assignment of timer TSEARCHGW
	{
		random1 = false;
		// Assignment of retransmission times
		tini = 5; // tini=rand() % (tsearchgw+1);
		trtx = tini;
		timerDurationTime = tini; // The timer has a duration of 15 seconds*NADV for testing (TADV should have a duration of 15min)
		start_timer1(); // The timer starts its operation
		auxTimer = 2;
	}
	else if (stateIndex == 0) // DISCOVERY
	{
		initialState();
	}
	else if (stateIndex == 1) // RX_GWINFO
	{
		state_1();
	}
}

// Function to receive MQTT-SN messages over IEEE802.15.4 frames //
void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
	{

		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source buffer to destination
		bmm_buffer_free(frame->buffer_header); // Free buffer data to avoid overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x00) // RX ADVERTISE
		{
			enableReception = 0; // Disable reception to avoid interference in state change.
			successfulReceptionControl = 0; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x02) // RX GWINFO
		{
			enableReception = 0; // Disable reception to avoid interference in state change.
			successfulReceptionControl = 2; // This variable allows the node to exit the reception loop
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
void initialState(void) // DISCOVERY S0
{
	uint8_t State[48] = "|======DISCOVERY=======||        S-00          |";
	//|        S-00          |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Variable that enables the function that receives messages
	}
	else if (stateControl == 3)
	{   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// The node will remain in this state until it receives an ADVERTISE message,
		// the NADVTADV timer finishes, or the TSEARCHGW timer finishes.
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// RX ADVERTISE //////////////////////////////////////////////////////////////////////////////////////////////////////// //
		if (successfulReceptionControl == 0)
		{
			successfulReceptionControl = -1; // Reception is disabled to avoid blocking
			stop_timer1(); // The timer is stopped when an ADVERTISE arrives to restart it.
			updatedGwTable = 2;
			controlLED(updatedGwTable); // Indicates how a GW table should be updated
			addTimer = 1; // Move to add Timer
		}
		else if (addTimer == 1) // Adding NADVTADV Timer
		{
			addTimer = 0; // The timer is only assigned once so the variable returns to 0
			NTadv = NADV * durationRx; // Assigning the duration of the NADVTADV timer
			timerDurationTime = NTadv; // The timer has a duration of 15 seconds*NADV for testing (TADV should have a duration of 15min)
			start_timer1(); // The timer restarts its operation
			// STATE CHANGE
			auxTimer = 1;
			temporaryStateIndex = 0; // Variable that stores the next state
			stateControl = 4; // Exit state
		}
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// NADVTADV Timer Ends //////////////////////////////////////////////////////////////////////////////////////////////// //
		else if (TimerEnd == 1 && auxTimer == 1) // If the condition indicating that Nadv*Tadv time has ended is met
		{
			TimerEnd = 0; auxTimer = 0; // Disable reception of the timer signal since it has ended
			transmit_sample_frame((uint8_t*)"|     Nadv*Tadv     ||       Finish      |", 42, 1); // Message indicating the end of Nadv*Tadv
			updatedGwTable = 1; // |      Finish     |
			controlLED(updatedGwTable); // Indicates how a GW table should be updated
			//No new timers will be added until a new ADVERTISE is received
			temporaryStateIndex = 0; // Variable that stores the next state
			stateControl = 4; // Exit state
		}
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// TSEARCHGW Timer Ends //////////////////////////////////////////////////////////////////////////////////////////////// //
		else if (TimerEnd == 1 && auxTimer == 2) // If the client does not want to wait for an ADVERTISE, a timer is activated, when it ends, a SEARCHGW will be sent
		{
			TimerEnd = 0; auxTimer = 0;
			transmit_sample_frame((uint8_t*)"|    T_SEARCHGW     ||       Finish      |", 42, 1); // Message indicating the end of Nadv*Tadv
			delay_ms(200); // Delay for button to work
			addTimer = 2; // Move to add a second Timer
		}
		else if (addTimer == 2) // TIMER TRTX
		{
			addTimer = 0; // The timer is only assigned once so the variable returns to 0
			trtx = 2 * trtx;
			timerDurationTime = trtx; // The timer has a duration of 5 seconds for testing (should be random)
			start_timer1(); // The timer starts its operation
			enableTransmission = 1; // Move to transmit SEARCHGW
		}   // TX SEARCHGW
		else if (enableTransmission == 1) // Proceed to transmit a SEARCHGW as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only a SEARCHGW message is needed
			messageLength = funcionSEARCHGW(SEARCHGW); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(SEARCHGW, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // Variable that stores the next state
			stateControl = 4; // Exit state
		}
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		transmit_sample_frame((uint8_t*)"|       CL_END         ||---------S-00---------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}
// ============================================================================================================================= //
void state_1(void) // RX_GWINFO
{
	uint8_t State[48] = "|======RX_GWINFO=======||        S-01          |";
	                   //|       CL_END         |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1;
	}
	else if (stateControl == 3)
	{
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// The node will remain in this state until a GWINFO message arrives
		// or the TRTX timer ends.
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// RX GWINFO /////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		if (successfulReceptionControl == 2) // The condition is activated when a GWINFO is received
		{
			successfulReceptionControl = 0; // Reception is disabled to avoid blocking
			stop_timer1(); // Timer is stopped to avoid inconvenience
			updatedGwTable = 2; // GW table update
			controlLED(updatedGwTable); // Indicates how a GW table should be updated
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
		// TRTX Timer Ends ///////////////////////////////////////////////////////////////////////////////////////////// //
		else if (TimerEnd == 1) // If the condition indicating that the time Tretry has ended is met
		{
			TimerEnd = 0; // Disable reception of the timer signal since it has ended
			transmit_sample_frame((uint8_t*)"|       T_RTX       ||       Finish      |", 42, 1);
			           //:    GFinaliza    : :       Tadv        :// A message is transmitted indicating that the Tadv Timer has ended
			jump = 1; // Move to use a delay
		}
		else if (jump == 1) // A delay is added to be able to transmit another message afterwards
		{
			jump = 0;
			delay_ms(200); // Delay
			enableTransmission = 1;
		}
		//RTX SEARCHGW
		else if (enableTransmission == 1) // Once the signal from the timer is received, a MSG SEARCHGW with aT = 0 is transmitted
		{
			enableTransmission = -1; // Transmission is disabled because only TX a SEARCHGW message is needed
			messageLength = funcionSEARCHGW(SEARCHGW); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(SEARCHGW, messageLength, 0); // Message sending
			messageLength = 0;
			addTimer = 3; // Move to restart the timer
		}
		else if (addTimer == 3) // The timer is restarted
		{
			addTimer = 0;
			trtx = 2 * trtx;
			timerDurationTime = trtx; // The timer has a duration of 10*n seconds for testing (should increase exponentially)
			start_timer1();
			// EXIT STATE
			temporaryStateIndex = 1; // State change
			stateControl = 4; // Exit the loop
		}
		// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)"|       CL_END         ||---------S-01---------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}

// ============================================================================================================================= //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 2. CLIENT’S_CONNECTION_SETUP *************************************************************** //
/*
// Procedure Variables //
int proc2Count = 0; // Indicates how this procedure ends
int twait = 10; // Should be 5 min to reset this procedure
int conditional = 0; // Prevents the blocking of will flag comparators

// State Function Declarations //
void initialState(void); // ESTABLISH CONNECTION S0

void state_1(void); // RX_TOPIC_REQ S1

void state_2(void); // RX_MSG_REQ S2

void state_3(void); // RX_CONNACK S3

void state_4(void); // DECONGESTION S4

// Function to receive MQTT-SN messages over IEEE802.15.4 frames //
void usr_wireless_app_task(void) {
	if (stateIndex == 0) // CONFIGURE CONNECT S0
	{
		initialState();
	} else if (stateIndex == 1) // RX_TOPIC_REQ S1
	{
		state_1();
	} else if (stateIndex == 2) // RX_MSG_REQ S2
	{
		state_2();
	} else if (stateIndex == 3) // RX_CONNACK S3
	{
		state_3();
	} else if (stateIndex == 4) // DECONGESTION S4
	{
		state_4();
	}
}

// Function to receive MQTT-SN messages over IEEE802.15.4 frames //
void usr_frame_received_cb(frame_info_t *frame) {
	if (enableReception == 1) // State index
	{

		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserves memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copies information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Frees buffer data, prevents overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x06) // RX WILLTOPICREQ
		{
			enableReception = 0; // Disables reception to avoid interference in state change.
			successfulReceptionControl = 6; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		} else if (msgTypeMQTT_SN == 0x08) // RX WILLMSGREQ
		{
			enableReception = 0; // Disables reception to avoid interference in state change.
			successfulReceptionControl = 8; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		} else if (msgTypeMQTT_SN == 0x05) // RX CONNACK
		{
			enableReception = 0; // Disables reception to avoid interference in state change.
			successfulReceptionControl = 5; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{
}

// State Programming Functions //
// ============================================================================================================================= //
void initialState(void) { // ESTABLISH_CONNECTION S0
	uint8_t State[48] = "|=ESTABLISH_CONNECTION=||       CL S-00        |";
                       //|        CL_END        ||------ CL S-04 -------|
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // Wait time to observe the start of the state
		} 
	else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Variable that enables the function to receive messages.
		} 
	else if (stateControl == 3) {
		// Node will remain in this state until deciding the Will flag
		// Will flag == 0 or Will flag == 1
		// CONFIGURING WILL FLAG
		uint8_t userFlags = 0b00001000; // TRUE 0b00001000 or FALSE 0b00000000
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To indicate that the client has configured WILL FLAG, a button will be used
		{
			delay_ms(200); // Delay for button operation
			uint8_t flagIni[48] = "|         Will         ||       flag==_        |"; // Flag indicator
			if (userFlags == 0b00001000) {
				flagIni[38] = '1'; // Flag indicator
				} 
			else if (userFlags == 0b00000000) {
				flagIni[38] = '0'; // Flag indicator
				}
			transmit_sample_frame(flagIni, 48, 1); // Flag indicator
			jump = 1;
		} 
		else if (jump == 1) // Jump
		{
			jump = 0;
			delay_ms(200);
			enableTransmission = 4; // Move to transmit CONNECT
		}
		// TX CONNECT WILLFLAG=1 0b00001000
		else if (enableTransmission == 4 && userFlags == 0b00001000) // Proceed to transmit a CONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a GWINFO message is needed
			uint8_t clientID[] = "idcl0";
			uint8_t flags;
			flags = userFlags;
			messageLength = funcionCONNECT(CONNECT, flags, clientID, (sizeof(clientID)) - 1); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(CONNECT, messageLength, 0); // Message transmission
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		// TX CONNECT WILLFLAG=0 0b00000000
		else if (enableTransmission == 4 && userFlags == 0b00000000) // Proceed to transmit a CONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a GWINFO message is needed
			uint8_t clientID[] = "idcl0";
			uint8_t flags;
			flags = userFlags;
			messageLength = funcionCONNECT(CONNECT, flags, clientID, (sizeof(clientID)) - 1); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(CONNECT, messageLength, 0); // Message transmission
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 3; // State change
			stateControl = 4;
		}
	} 
	else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // Wait time to observe the end of the state
	}
	else if (stateControl == 5) {
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-00 -------|", 48, 1);
        // |         CFIN         |
        stateControl = 1; // This control variable is set to one so that the following states can start their operation.
        stateIndex = temporaryStateIndex; // State change
        temporaryStateIndex = -1; // Set to -1 to be used later.
     }
}

// ============================================================================================================================= //
void state_1(void) { // State RX_TOPIC_REQ S1

	uint8_t State[48] = "|=====RX_TOPIC_REQ=====||       CL S-01        |";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // Wait time to observe the start of the state
	} 
	else if (stateControl == 2) {
		 stateControl = 3;
		 transmit_sample_frame(State, 48, 1); // State indicator
		 enableReception = 1; // Variable that enables the function to receive messages.
		 } 
	else if (stateControl == 3) {
		  // Node will remain in this state until it receives a WILLTOPICREQ message
		  // RX WILLTOPICREQ
		  if (successfulReceptionControl == 6) // Condition is activated when a WILLTOPICREQ is received
		  {
			  successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			  jump = 1; // Add transmission delay
		   } // DELAY
		        else if (jump == 1) {
			        jump = 0;
			        delay_ms(4000); // Wait time to observe the response to a reception
			        enableTransmission = 7; // Proceed to send a WILLTOPIC
		        } else if (enableTransmission == 7) // Proceed to transmit a WILLTOPIC as indicated by the variable value
		        {
			        enableTransmission = -1; // Transmission is disabled because only a WILLTOPICREQ message is needed
			        uint8_t willTopic[] = "willTop";
			        uint8_t willTopicFlags;
			        willTopicFlags = 0b01010000;
			        messageLength = funcionWILLTOPIC(WILLTOPIC, willTopicFlags, willTopic, (sizeof(willTopic)) - 1); // Invocation of the function that creates the message to be sent, returns the message length
			        transmit_sample_frame(WILLTOPIC, messageLength, 0); // Message transmission
			        messageLength = 0;
			        // STATE CHANGE
			        temporaryStateIndex = 2; // State change
			        stateControl = 4;
		        }
	} 
	else if (stateControl == 4) {
		     stateControl = 5;
		     delay_ms(1500); // Wait time to observe the end of the state
	}
	else if (stateControl == 5) {
		      stateControl = 1;
		      transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-01 -------|", 48, 1); // State indicator
		      stateIndex = temporaryStateIndex; // State change
		      temporaryStateIndex = -1; // Set to -1 to be used later.
	 }
}
// ============================================================================================================================= //
void state_2(void) { // RX_MSG_REQ S2

	uint8_t State[48] = "|======RX_MSG_REQ======||       CL S-02        |";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // Wait time to observe the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Variable that enables the function to receive messages.
		} else if (stateControl == 3) {
		// Node will remain in this state until it receives a WILLMSGREQ message
		// RX WILLMSGREQ
		if (successfulReceptionControl == 8) // Condition is activated when a WILLMSGREQ is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Add transmission delay
		} // DELAY
		else if (jump == 1) {
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to observe the response to a reception
			enableTransmission = 9; // Proceed to send a WILLMSG
		} // TX WILLMSG
		else if (enableTransmission == 9) // Proceed to transmit a WILLMSG as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a message is needed
			uint8_t willMsg[] = "willmsgcl";
			messageLength = funcionWILLMSG(WILLMSG, willMsg, (sizeof(willMsg)) - 1); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(WILLMSG, messageLength, 0); // Message transmission
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 3; // State change
			stateControl = 4;
		}
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // Wait time to observe the end of the state
		} else if (stateControl == 5) {
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-02 -------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ============================================================================================================================= //
void state_3(void) { // RX_CONNACK S3

	uint8_t State[48] = "|======RX_CONNACK======||       CL S-03        |";
	                   //|        CL_END        ||------ CL S-04 -------|
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // Wait time to observe the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Variable that enables the function to receive messages.
		} else if (stateControl == 3) {
		// Node will remain in this state until it receives a CONNACK message
		// RX CONNACK
		if (successfulReceptionControl == 5) // Condition is activated when a CONNACK is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Add transmission delay
		} // DELAY
		else if (jump == 1) {
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to observe the response to a reception
			conditional = 1; // Proceed to compare the received ReturnCode field
		} else if (conditional == 1 && receivedFrame.MQTT_SN_message[2] == 0x00) // Accept the received CONNACK message
		{
			conditional = 0;
			proc2Count = 1;
			controlLED(proc2Count); // Indicates a successful connection.
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to ESTABLISH_CONNECTION state
			stateControl = 4;
		}
		// Handle other conditions...
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // Wait time to observe the end of the state
		} else if (stateControl == 5) {
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-03 -------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; // Change to initial state
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ============================================================================================================================= //
void state_4(void) { // CONGESTION S4

	uint8_t State[48] = "|=====CONGESTION=======||       CL S-04        |";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // Wait time to observe the start of the state
		} else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		} else if (stateControl == 3) {
		// Node will remain in this state until TWAIT ends
		// TWAIT Ends
		if (TimerEnd == 1) // Condition is activated when the timer ends
		{
			TimerEnd = 0; // Prevents blocking
			transmit_sample_frame((uint8_t*)"|         TWAIT:       ||         Finish       |", 48, 1); // State indicator
			proc2Count = 2;
			controlLED(proc2Count); // Indicates that the connection procedure should be repeated.
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to ESTABLISH_CONNECTION state
			stateControl = 4;
		}
		} else if (stateControl == 4) {
		stateControl = 5;
		delay_ms(1500); // Wait time to observe the end of the state
		} else if (stateControl == 5) {
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-04 -------|", 48, 1); // State indicator
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
int willUpdateCounter = 0; // Variable to handle multiple pulses
int willComparator = 0; // Variable to handle multiple pulses

// Declaration of State Functions //
void initialState(void); // ACTIVE UPDATE WILL DATA S0
void state_1(void);     // RX_TOPIC_RESP S1
void state_2(void);     // RX_MSG_RESP S2

void usr_wireless_app_task(void) {
	if (stateIndex == 0) // ACTIVE UPDATE WILL DATA S0
	{
		initialState();
	}
	else if (stateIndex == 1)  // RX_TOPIC_RESP S1
	{
		state_1();
	}
	else if (stateIndex == 2)  // RX_MSG_RESP S2
	{
		state_2();
	}
}

void usr_frame_received_cb(frame_info_t *frame) {
	if (enableReception == 1) {
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // Reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data to avoid overlap
		
		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		
		if (msgTypeMQTT_SN == 0x1B) // RX WILLTOPICRESP
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 27; // This variable allows the node to exit the reception loop
		}
		else if (msgTypeMQTT_SN == 0x1D) // RX WILLMSGRESP
		{
			enableReception = 0; // Disable reception to avoid interference in state changes.
			successfulReceptionControl = 29; // This variable allows the node to exit the reception loop
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame) {
	// Callback function for transmitted frames
}

void initialState(void) {// ACTIVE UPDATE WILL DATA S0
	uint8_t state[48] = "|===ACT_DATOS_WILL_====||       CL S-00        |";
	if (stateControl == 1) {
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2) {
		stateControl = 3;
		transmit_sample_frame(state, 48, 1); // state indicator RX_Connect
		enableReception = 1; // Enable the function that receives messages.
	}
	else if (stateControl == 3) {
		// Remain in this state until choosing the update to send
		//////////////////////////////////////////////////////////////////////
		// BUTTON RX Signals
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the WILL data update, a button is used
		{
			delay_ms(200); // Delay for button operation
			willUpdateCounter++;
			if (willUpdateCounter == 1)
			{
				addTimer = 1; // When the first pulse is received, timer configuration will continue
			}
		}// PULSE TIMER
		else if (addTimer == 1)
		{
			addTimer = 0; // Timer is assigned only once so the variable returns to 0
			timerDurationTime = 2; // Timer duration is set to 3 seconds for testing
			start_timer1(); // Timer starts operation
		}// Timer Tadv Ends NewWILLTOPIC = TRUE
		else if (TimerEnd == 1) // If the condition indicating that Tadb time has ended is met
		{
			TimerEnd = 0; // Disable reception of timer signal as it has ended
			willComparator = willUpdateCounter; // Avoid unnecessary pulses
			willUpdateCounter = 0;
		}
		/////////////////////////////////////////////////////////////////
		// NEW WILLTOPIC
		else if (willComparator == 1)
		{
			willComparator = 0;
			transmit_sample_frame((uint8_t*)"|      update     ||     WILLTOPIC==1     |", 48, 1);
			//                    |------ CL S-00 -------||------ CL S-00 -------|
			// A message indicating Timer T adv termination is transmitted
			jump = 1; // Proceed to use a delay
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 26; // Proceed to send a WILLTOPICUPD
		}// TR WILLTOPICUPD
		else if (enableTransmission == 26) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only one message is needed
			// TX msg WILLTOPICUPD
			uint8_t willTopic[] = "newWllTOP";
			uint8_t flagsWTU = 0b01110000; // Flags field, check MQTT-SN documentation
			messageLength = funcionWILLTOPICUPD(WILLTOPICUPD, flagsWTU, willTopic, (sizeof(willTopic)) - 1);
			transmit_sample_frame(WILLTOPICUPD, messageLength, 0); // message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		/////////////////////////////////////////////////////////////////
		// NEW WILLMSG
		else if (willComparator == 2)
		{
			willComparator = 0;
			transmit_sample_frame((uint8_t*)"|      update     ||      WILLMSG==1      |", 48, 1);
			jump = 2; // Proceed to use a delay
		}// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // wait time to appreciate the response to a reception
			enableTransmission = 28; // Proceed to send a WILLMSG
		}// TR WILLMSG
		else if (enableTransmission == 28) // Proceed to transmit a WILLMSG as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only one message WILLMSG is needed
			// TX msg WILLMSG
			uint8_t willMSG[] = "newWlMSG";
			messageLength = funcionWILLMSGUPD(WILLMSGUPD, willMSG, (sizeof(willMSG)) - 1);
			transmit_sample_frame(WILLMSGUPD, messageLength, 0); // message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 2; // State change
			stateControl = 4;
		}
		/////////////////////////////////////////////////////////////////
	}
    else if (stateControl == 4) {
	    //LED_Off(LED0);
	    stateControl = 5;
	    delay_ms(1500); // time to appreciate the end of the state
    }
    else if (stateControl == 5) {
	    stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
	    transmit_sample_frame((uint8_t*)"|       Finish       ||------ CL S-00 -------|", 48, 1); // State indicator
	    //|==CONFIGURE_CONNECT==||       CL S-00        |
	    stateControl = 1; // This control variable is set to one so that the following states can start their operation.
	    stateIndex = temporaryStateIndex; // State change
	    temporaryStateIndex = -1; // It is set to -1 to be used later.
    }
    }

    // ********************************************************************************** //
    void state_1(void) {// RX_TOPIC_RESP S1

	    uint8_t state[48] = "|=====RX_TOPIC_RESP====||       CL S-01        |";
	    //|        S-02          |
	    if (stateControl == 1) {
		    stateControl = 2;
		    delay_ms(1500); // time to appreciate the start of state
	    }
	    else if (stateControl == 2) {
		    stateControl = 3;
		    transmit_sample_frame(state, 48, 1); // State indicator
		    enableReception = 1; // Enable the function that receives messages.
	    }
	    else if (stateControl == 3) {
		    //////////////////////////////////////////////////////////////////////
		    //The node will remain in this state until it receives a WILLTOPICRESP message
		    //////////////////////////////////////////////////////////////////////
		    // RX WILLTOPICRESP
		    if (successfulReceptionControl == 27) // The condition is activated when a WILLTOPICRESP is received
		    {
			    successfulReceptionControl = -1; // Received the required message, other messages are no longer accepted
			    proc3Count = 1;
			    controlLED(proc3Count);
			    temporaryStateIndex = 0; // State change
			    stateControl = 4;
		    }
		    //////////////////////////////////////////////////////////////////////
	    }
	    else if (stateControl == 4) {
		    stateControl = 5;
		    delay_ms(1500); // time to appreciate the end of the state
	    }
	    else if (stateControl == 5) {
		    stateControl = 1; // This control variable is set to zero as it is not used in the initial state.
		    transmit_sample_frame((uint8_t*)"|        Finish       ||------ CL S-02 -------|", 48, 1); // State indicator
		    temporaryStateIndex = stateIndex; // State change
		    stateIndex = -1; // It is set to -1 to be used later.
	    }
    }

    // ********************************************************************************** //
    void state_2(void) {// RX_MSG_RESP S2

	    uint8_t state[48] = "|======RX_MSG_RESP=====||       CL S-02        |";
	    //|        S-02          |
	    if (stateControl == 1) {
		    stateControl = 2;
		    delay_ms(1500); // time to appreciate the start of state
	    }
	    else if (stateControl == 2) {
		    stateControl = 3;
		    transmit_sample_frame(state, 48, 1); // State indicator
		    enableReception = 1; // Enable the function that receives messages.
	    }
	    else if (stateControl == 3) {
		    //////////////////////////////////////////////////////////////////////
		    //The node will remain in this state until it receives a WILLMSGRESP message
		    //////////////////////////////////////////////////////////////////////
		    // RX WILLMSGRESP
		    if (successfulReceptionControl == 29) // The condition is activated when a WILLMSGRESP is received
		    {
			    successfulReceptionControl = -1; // Received the required message, other messages are no longer accepted
			    proc3Count = 2;
			    controlLED(proc3Count);
			    temporaryStateIndex = 0; // State change
			    stateControl = 4;
		    }
		    //////////////////////////////////////////////////////////////////////
	    }
	    else if (stateControl == 4) {
		    stateControl = 5;
		    delay_ms(1500); // time to appreciate the end of the state
	    }
	    else if (stateControl == 5) {
		    stateControl = 1; // This control variable is set to zero as it is not used in the initial state.
		    transmit_sample_frame((uint8_t*)"|        Finish       ||------ CL S-02 -------|", 48, 1); // State indicator
		    temporaryStateIndex = stateIndex; // State change
		    stateIndex = -1; // It is set to -1 to be used later.
	    }
    }

// ********************************************************************************** //

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 4. PROCEDURE_FOR_REGISTERING_TOPIC_NAMES  ************************************************** //
/*
// Procedure Variables //
int proc4Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int twait = 10; // twait=300; Should be 5 min to restart this procedure
int messageAlert = 0; // Used to send alert messages
int ACKComparator = 0; // Prevents blocking in the REGACK comparator

// Declaration of State Functions //
void initialState(void); // WAIT REGISTER S0

void state_1(void); // RX_REGACK(from Gateway) S1

void congestionState(void); // CONGESTION S2

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // WAIT REGISTER S0
	{
		initialState();
	}
	else if (stateIndex == 1)
	{
		state_1(); // RX_REGACK(from Gateway) S1
	}
	else if (stateIndex == 2)
	{
		congestionState(); // CONGESTION S2
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // allocate memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // copy information from source (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		if (msgTypeMQTT_SN == 0x0A) // RX REGISTER
		{
			enableReception = 0; // Disable reception to avoid interference with state change.
			successfulReceptionControl = 10; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
		else if (msgTypeMQTT_SN == 0x0B) // RX REGACK
		{
			enableReception = 0; // Disable reception to avoid interference with state change.
			successfulReceptionControl = 11; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue with its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{
}

// ****************************************************************************************************************************** //
void initialState(void) // WAIT REGISTER S0
{
	uint8_t State[48] = "|=====WAIT_REGISTER=====||       CL S-00        |";
	//                  |         CFIN         ||                      |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // RX_Connect state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a REGISTER from the Gateway
		// or when this client requires to register a topic
		// RX REGISTER
		if (successfulReceptionControl == 10) // The condition is activated when a REGISTER is received
		{
			successfulReceptionControl = -1; // The required message has been received and no other messages are accepted
			jump = 1; // Delay is added due to transmission
		}
		// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // waiting time to appreciate the response to a reception
			enableTransmission = 11;
		}
		// TR REGACK
		else if (enableTransmission == 11) // Proceed to transmit a REGACK as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			// TX msg REGACK
			uint8_t ReturnCODE;
			ReturnCODE = 0x00; // 0x00; 0x01; 0x02; 0x03;
			messageLength = funcionREGACK(REGACK, ReturnCODE); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(REGACK, messageLength, 0); // message sending
			messageLength = 0;
			proc4Count = 1;
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// It is indicated that it has been received: readyRegister==1 this indication should come from higher layers
		// BUTTON RX UPPER LAYERS
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the RX readyRegister==1, a button is used
		{
			delay_ms(200); // Delay for the button to work
			messageAlert = 2; // indicate that the condition has been met
		}
		// Alert message: readyRegister==1
		else if (messageAlert == 2)
		{
			messageAlert = 0; // the alert is sent only once
			transmit_sample_frame((uint8_t*)"|         Ready         ||   readyRegister==1   |", 48, 1);
			//                           |         CFIN         ||                      |
			jump = 2;
		}
		// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(400); // waiting time
			enableTransmission = 10; // Proceed to send a REGISTER
		}
		// TR REGISTER
		else if (enableTransmission == 10) // Proceed to transmit a REGISTER as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only a REGISTER message is needed
			uint8_t topicNAME[] = "topicNameCl";
			messageLength = funcionREGISTER(REGISTER, topicNAME, (sizeof(topicNAME)) - 1);
			transmit_sample_frame(REGISTER, messageLength, 0); // message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Ends        ||------ CL S-00 -------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //

void state_1(void) // RX_REGACK(from Gateway) S1
{
	uint8_t State[48] = "|=====RX_REGACK_GW=====||       CL S-01        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // RX_Connect state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives REGACK from the gateway
		// RX REGACK
		if (successfulReceptionControl == 11) // The condition is activated when a REGACK is received
		{
			successfulReceptionControl = -1; // The required message has been received and no other messages are accepted
			ACKComparator = 1; //
		}
		// Comparator
		else if (ACKComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x00)
		{
			ACKComparator = 0;
			proc4Count = 2;
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			temporaryStateIndex = 0; // returns to the WAIT REGISTER state
			stateControl = 4;
		}
		else if (ACKComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x03)
		{
			proc4Count = 0;
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			temporaryStateIndex = 0; // returns to the WAIT REGISTER state
			stateControl = 4;
		}
		else if (ACKComparator == 1 && receivedFrame.MQTT_SN_message[6] == 0x01)
		{
			ACKComparator = 0;
			// SET TIMER TWAIT
			timerDurationTime = twait; // The timer has a duration of 10 seconds for testing
			start_timer1(); // The timer starts its operation
			// STATE CHANGE
			temporaryStateIndex = 2; // moves to the CONGESTION state
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Ends        ||------ CL S-01 -------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}
// ****************************************************************************************************************************** //

void congestionState(void) // CONGESTION S2
{
	uint8_t State[48] = "|=====CONGESTION====||       CL S-02        |";
	//                  |         CFIN         ||                      |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // RX_Connect state indicator
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until TWAIT time passes
		// TIMER FINISHES
		if (TimerEnd == 1) // If the condition indicating that the Tadb time has ended is met
		{
			TimerEnd = 0; // Deactivate reception of the timer signal since it has ended
			transmit_sample_frame((uint8_t*)"|        TWAIT         ||       Ends       |", 48, 1);
			//                  |         CFIN         ||                      |
			proc4Count = 3;
			controlLED(proc4Count); // Indicates how a sequence of messages has been processed
			// State change
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Ends        ||------ CL S-02 -------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to be used later.
	}
}

// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 5. CLIENT'S_PUBLICATION_PROCEDURE ********************************************************** //
/*
// Procedure Variables //
int proc5Count = 0; // Indicates how a sequence of messages sent in this procedure ends
int twait = 10; // twait=300; Should be 5 minutes to restart this procedure
int iniRegister = 0; // Indicates that another procedure should start
int comparatorACK = 0; // Prevents blocking when receiving PUBACK
int qos = -1; // Prevents blocking when comparing messages with QoS flags
int pulseCounter = 0; // Helps choose a message with a specific QoS
int receivedPulses = 0;

// Declaration of State Functions //
void stateInitial(void); // ACTIVE PUBLISH S0
void state_1(void);      // RX_ACK S1
void state_2(void);      // RX_PUBCOMP S2
void state_3(void);      // DECONGESTION S3

void usr_wireless_app_task(void)
{
	if (stateIndex == 0)       // ACTIVE PUBLISH S0
	{
		stateInitial();
	}
	else if (stateIndex == 1)  // RX_ACK S1
	{
		state_1();
	}
	else if (stateIndex == 2)  // RX_PUBCOMP S2
	{
		state_2();
	}
	else if (stateIndex == 3)  // DECONGESTION S3
	{
		state_3();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
	{
		// Clear received frame memory space
		memset(&receivedFrame, 0, sizeof(receivedFrame));
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // Copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data to avoid overlap

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		if (msgTypeMQTT_SN == 0x0D) // RX PUBACK
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 13; // This variable allows the node to exit the reception loop
		}
		else if (msgTypeMQTT_SN == 0x0F) // RX PUBREC
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 15; // This variable allows the node to exit the reception loop
		}
		else if (msgTypeMQTT_SN == 0x0E) // RX PUBCOMP
		{
			enableReception = 0; // Disable reception to avoid interference in state change
			successfulReceptionControl = 14; // This variable allows the node to exit the reception loop
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{
	// Transmit frame callback function
}

// ****************************************************************************************************************************** //
void stateInitial(void) // ACTIVE PUBLISH S0
{
	uint8_t State[48] = "|===ACT_PUBLICATION====||       CL S-00        |";
	                   //|        CL_END        ||------ CL S-04 -------|

	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		enableReception = 1; // Enable message receiving function
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until publication data is ready
		// or an invalid topicID PUBACK is received
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// UPPER LAYERS RX BUTTON
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate a publication, a button is used
		{
			delay_ms(200); // Delay for button to work
			pulseCounter++;
			if (pulseCounter == 1)
			{
				addTimer = 1; // When the first pulse arrives, continue with Timer configuration
			}
		}
		// PULSE TIMER
		else if (addTimer == 1)
		{
			addTimer = 0; // The timer is assigned only once, so the variable returns to 0
			// SET TIMER
			timerDurationTime = 3; // The timer has a duration of 5 seconds to collect other pulses
			start_timer1(); // Timer starts operation
		}
		// Timer Tadv Ends Ready NewWILLTOPIC=TRUE
		else if (TimerEnd == 1) // If the condition indicating that the time Tadb has ended is met
		{
			TimerEnd = 0; // Disable reception of the timer signal since it has ended
			receivedPulses = pulseCounter; // avoid unnecessary pulses
			pulseCounter = 0;
		}
		// Preparing PUBLISH (QoS 0)
		else if (receivedPulses == 1)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t*)"|  publicationReady=1  ||       (QoS_0)        |", 48, 1);
			                               //|        CL_END        ||------ CL S-04 -------|
			jump = 1; // Move to use a delay
		}
		// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Delay time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 0;
		}
		// TR PUBLISH Prepare PUBLISH (QoS 0)
		else if (enableTransmission == 12 && qos == 0) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS_0"; uint8_t flagsP; flagsP = 0b00000000; //flagsP field check MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message sending
			messageLength = 0;
			proc5Count = 1; // Indicates that this sequence is finished
			controlLED(proc5Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// Preparing PUBLISH (QoS 1)
		else if (receivedPulses == 2)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t*)"|  publicationReady=1  ||       (QoS_1)        |", 48, 1);
			                               //|  publicationReady=1  ||       (QoS_0)        |
			jump = 2; // Move to use a delay
		}
		// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Delay time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 1;
		}
		// TR PUBLISH
		else if (enableTransmission == 12 && qos == 1) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS1"; uint8_t flagsP; flagsP = 0b00100000; //flagsP field check MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		// Preparing PUBLISH (QoS 2)
		else if (receivedPulses == 3)
		{
			receivedPulses = 0;
			transmit_sample_frame((uint8_t*)"|  publicationReady=1  ||       (QoS_2)        |", 48, 1);
			                               //|  publicationReady=1  ||       (QoS_0)        |
			jump = 3; // Move to use a delay
		}
		// DELAY
		else if (jump == 3)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Delay time to appreciate the response to a reception
			enableTransmission = 12; // Proceed to send
			qos = 2;
		}
		// TR PUBLISH
		else if (enableTransmission == 12 && qos == 2) // Proceed to transmit a WILLTOPICUPD as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			qos = -1;
			uint8_t DATA[] = "Payload-QoS2"; uint8_t flagsP; flagsP = 0b01000000; //flagsP field check MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // State change
			stateControl = 4;
		}
		// RX PUBACK
		if (successfulReceptionControl == 13) // The condition is activated when a PUBACK is received
		{
			successfulReceptionControl = -1; // Received the required message, no others are accepted
			comparatorACK = 1; // Check the returnCode field
		}
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] == 0x02)
		{
			comparatorACK = 0;
			iniRegister = 1; // Indicates that this sequence is finished
			controlAllLEDs(iniRegister);
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] != 0x02)
		{
			comparatorACK = 0;
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-00 -------|", 48, 1); // State indicator
		                               //|        CL_END        ||------ CL S-04 -------|
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) // RX_ACK S1
{
	uint8_t State[48] = "|========RX_ACK========||       CL S-01        |";
	                   //|        CL_END        ||------ CL S-04 -------|

	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		enableReception = 1; // Enable message receiving function
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until PUBACK or PUBREC is received
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// RX PUBACK
		if (successfulReceptionControl == 13) // The condition is activated when a PUBACK is received
		{
			successfulReceptionControl = -1; // Received the required message, no others are accepted
			comparatorACK = 1; // Add delay for transmission
		}
		// Comparador PUBACK ACCEPTED
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] == 0x00)
		{
			comparatorACK = 0;
			proc5Count = 2; // Indicates how the procedure ended
			controlAllLEDs(proc5Count); //
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// Comparador PUBACK invalid topicID
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] == 0x02)
		{
			comparatorACK = 0;
			iniRegister = 1;
			controlAllLEDs(iniRegister); // Indicates how the procedure ended
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// Comparador PUBACK not supported
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] == 0x03)
		{
			comparatorACK = 0;
			controlAllLEDs(0); // Indicates how the procedure ended
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		// Comparador PUBACK congestion
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		else if (comparatorACK == 1 && receivedFrame.MQTT_SN_message[6] == 0x01)
		{
			comparatorACK = 0;
			// SET TIMER TWAIT
			timerDurationTime = twait; // The timer has a duration of 10 seconds for testing
			start_timer1(); // The timer starts its operation
			// STATE CHANGE
			temporaryStateIndex = 3; // State change
			stateControl = 4;
		}
		// RX PUBREC
		if (successfulReceptionControl == 15) // The condition is activated when a PUBREC is received
		{
			successfulReceptionControl = -1; // Received the required message, no others are accepted
			jump = 1; // Add delay for transmission
		}
		else if (jump == 1)
		{
			jump = 0;
			delay_ms(4000); // Delay time to appreciate the response to a reception
			enableTransmission = 16; // Proceed to send PUBREL
		}
		else if (enableTransmission == 16) // Proceed to transmit a PUBREL as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a WILLTOPICREQ message is needed
			messageLength = functionPUBREL(PUBREL); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBREL, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 2; // State change
			stateControl = 4;
		}
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-01 -------|", 48, 1); // State indicator
		                               //|        CL_END        ||------ CL S-04 -------|
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_2(void) // RX_PUBCOMP STATE S2
{
	uint8_t State[48] = "|======RX_PUBCOMP======||       CL S-02        |";
	                   //|        CL_END        ||------ CL S-04 -------|

	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		enableReception = 1; // Enable message receiving function
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until a PUBCOMP message is received
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// RX PUBCOMP
		if (successfulReceptionControl == 14) // The condition is activated when a PUBCOMP is received
		{
			successfulReceptionControl = -1; // Received the required message, no others are accepted
			proc5Count = 3;
			controlAllLEDs(proc5Count);
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-02 -------|", 48, 1); // State indicator
		                               //|        CL_END        ||------ CL S-04 -------|
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_3(void) // DECONGESTION STATE S3
{
	uint8_t State[48] = "|=====DECONGESTION=====||       CL S-03        |";
	                   //|        CL_END        ||------ CL S-04 -------|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until TWAIT time passes
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//TIMER FINISH
		if (TimerEnd == 1) // If the condition indicating that time Tadb has ended is met
		{
			TimerEnd = 0; // Disable reception of the timer signal as it has ended
			transmit_sample_frame((uint8_t*)"|        T_WAIT:       ||        Finish        |", 48, 1); // State indicator
			                               //|12345678CL_END        ||12345678T_WAIT:       |
			stateControl = 4;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|        CL_END        ||------ CL S-03 -------|", 48, 1); // State indicator
		                               //|12345678CL_END        ||12345678T_WAIT:       |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change
		temporaryStateIndex = -1; // Set to -1 to use it later.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 6. PUBLICATION_WITH_QOS_LEVEL_-1 *********************************************************** //
/*
// Procedure Variables //
int proc6Count = 0;
int prepareData = 0;

// Declaration of State Functions //
void initialState(void); // ACTIVE PUBLISHING (QoS-1)S0

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // ACTIVE PUBLISHING (QoS-1)S0
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

	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

// ****************************************************************************************************************************** //

void initialState(void){ // ACTIVE PUBLISHING (QoS-1)S0
	
	uint8_t State[48] = "|====ACTIVE_PUBLISHING===||       CL S-00        |";
	//                  |         CFIN         ||===UPDATE_WILL===|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Waiting time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		//enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it has the publishing data ready with QoS-1 == TRUE
		//////////////////////////////////////////////////////////////////////////
		// UPPER LAYERS RX BUTTON publishing data
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the RX publishing data QoS-1 == TRUE
		{
			delay_ms(200); // Delay for the button to work
			prepareData = 1;
		}
		else if (prepareData == 1)
		{
			prepareData = 0;
			transmit_sample_frame((uint8_t*)"|   publishingDataReady   ||      (QoS-1)==1      |", 48, 1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			jump = 1;
		}
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Waiting time to send the PUBLISH
			enableTransmission = 12; // Proceed to send a PUBLISH
		} // TR PUBLISH QoS -1
		else if (enableTransmission == 12) // Proceed to transmit a PUBLISH as indicated by the value of the variable
		{
			enableTransmission = -1; // Transmission is disabled because only one PUBLISH message is needed
			uint8_t DATA[] = "PayloadQoS-1"; uint8_t flagsP; flagsP = 0b10100011; // Flags field check MQTT-SN documentation QoS -1
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message transmission
			messageLength = 0;
			proc6Count = 1;
			controlLED(proc6Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // State change
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		//LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // Waiting time to appreciate the end of state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
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
int proc7Count = 0; // Indicates how a message sequence ends in this procedure
int twait = 10; // twait = 300; It should be 5 minutes to restart this procedure
// Variables to control pulses
int pulseCounter = 0;
int pulseComparator = 0;
int ackComparator = 0;

// Declaration of State Functions //
void initialState(void); // ACTIVE SUBSCRIPTION or UNSUBSCRIBE S0

void state_1(void); // RX_SUBACK S1

void state_2(void); // DECONGESTION S2

void state_3(void); // RX_UNSUBACK S3

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // ACTIVE SUBSCRIPTION or UNSUBSCRIBE S0
	{
		initialState();
	}
	else if (stateIndex == 1) // RX_SUBACK S1
	{
		state_1();
	}
	else if (stateIndex == 2) // DECONGESTION S2
	{
		state_2();
	}
	else if (stateIndex == 3) // RX_UNSUBACK S3
	{
		state_3();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State Index
	{
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // reserve memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Delete buffer data, avoid overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		if (msgTypeMQTT_SN == 0x13) // RX SUBACK
		{
			enableReception = 0; // Disable reception to avoid state change interference.
			successfulReceptionControl = 19; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
		else if (msgTypeMQTT_SN == 0x15) // RX UNSUBACK
		{
			enableReception = 0; // Disable reception to avoid state change interference.
			successfulReceptionControl = 21; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

// ****************************************************************************************************************************** //

void initialState(void) // ACTIVE SUBSCRIPTION or UNSUBSCRIBE S0
{
	uint8_t State[48] = "|===ACT_SUBSCRIPTION===||       CL S-00        |";
	// | CFIN || === UPDATE_WILL === |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the state start
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator RX_Connect
		// enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until the client
		// requires subscribing or unsubscribing
		//////////////////////////////////////////////////////////////////////////
		// Button to indicate that it is ready to Subscribe (1 pulse) or Unsubscribe (2 pulses)
		//////////////////////////////////////////////////////////////////////////
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) //
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
			timerDurationTime = 5; // The timer has a duration of 5 seconds for testing
			start_timer1(); // The timer starts its operation
		} // Timer Ends
		else if (TimerEnd == 1) // If the condition indicating that the time has ended is met
		{
			TimerEnd = 0; // Deactivate the timer signal reception since it ended
			pulseComparator = pulseCounter; // unnecessary pulses are avoided
			pulseCounter = 0;
		}
		// Subscription list == 1 (1 pulse)
		else if (pulseComparator == 1)
		{
			pulseComparator = 0;
			transmit_sample_frame((uint8_t*)"|         list         ||   Subscription==1   |", 48, 1);
			// |------ CL S-00 -------||------ CL S-00 -------|
			jump = 1; // Move to use a delay
		} // DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(200); // Waiting time to send SUBSCRIBE
			enableReception = 18; // Proceed to send a SUBSCRIBE
			
		} // SUBSCRIBE
		else if (enableReception == 18) // Proceed to transmit a SUBSCRIBE as indicated by the variable value
		{
			enableReception = -1; // Transmission is disabled because only a SUBSCRIBE message is needed
			uint8_t topicNameOrID[] = "TopNameSUB"; uint8_t SUBFlags; SUBFlags = 0b11100011; // Field SUBFlags check MQTT-SN documentation
			messageLength = funcionSUBSCRIBE(SUBSCRIBE, SUBFlags, topicNameOrID, (sizeof(topicNameOrID)) - 1);
			transmit_sample_frame(SUBSCRIBE, messageLength, 0); // Message send
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // RX_SUBACK
			stateControl = 4;
		}
		// Unsubscribe == 1 (2 pulses)
		else if (pulseComparator == 2)
		{
			pulseComparator = 0;
			transmit_sample_frame((uint8_t*)"|           Unsubscribe  ||      Baja==1       |", 48, 1);
			// | Ready New || WILL_TOPIC = TRUE | // A message indicating that the T adv Timer has ended is transmitted
			jump = 2; // Move to use a delay
		} // DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(200); // Waiting time to send UNSUBSCRIBE
			enableReception = 20; // Proceed to send an UNSUBSCRIBE
		} // UNSUBSCRIBE
		else if (enableReception == 20) // Proceed to transmit an UNSUBSCRIBE as indicated by the variable value
		{
			enableReception = -1; // Transmission is disabled because only an UNSUBSCRIBE message is needed
			uint8_t topicNameOrID[] = "TopicNamEoID"; uint8_t UNSUBFlags; UNSUBFlags = 0b11100011; // Field UNSUBFlags check MQTT-SN documentation
			messageLength = funcionUNSUBSCRIBE(UNSUBSCRIBE, UNSUBFlags, topicNameOrID, (sizeof(topicNameOrID)) - 1);
			transmit_sample_frame(UNSUBSCRIBE, messageLength, 0); // Message send
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 3; // state change to RX_UNSUBACK
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
		transmit_sample_frame((uint8_t*)"|         Ends         ||------ CL S-00 -------|", 48, 1); // State indicator
		// | == CONFIGURE_CONNECT == || CL S-00 |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}
// ****************************************************************************************************************************** //

void state_1(void) // RX_SUBACK S1
{
	uint8_t State[48] = "|=======RX_SUBACK======||       CL S-01        |";
	// | CFIN || === UPDATE_WILL === |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator RX_Connect
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a SUBACK
		//////////////////////////////////////////////////////////////////////////
		// RX SUBACK
		if (successfulReceptionControl == 19) // The condition is activated when a SUBACK is received
		{
			successfulReceptionControl = -1; // Received the required message, other SUBACKs are not accepted
			ackComparator = 1; // Transmission delay is added
		} // Comparator Accepted = 0x00
		//////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[7] == 0x00)
		{
			ackComparator = 0;
			proc7Count = 1;
			controlLED(proc7Count); // Accepted = 0x00
			temporaryStateIndex = 0; // ACTIVE SUBSCRIPTION or UNSUBSCRIBE
			stateControl = 4;
		} // Comparator rejection
		//////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[7] == 0x02)
		{
			ackComparator = 0;
			controlLED(0);
			temporaryStateIndex = 0; // ACTIVE SUBSCRIPTION or UNSUBSCRIBE
			stateControl = 4;
		} // Comparator rejection
		//////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[7] == 0x03)
		{
			ackComparator = 0;
			controlLED(0);
			temporaryStateIndex = 0; // ACTIVE SUBSCRIPTION or UNSUBSCRIBE
			stateControl = 4;
		} // congestion rejection
		//////////////////////////////////////////////////////////////////////////
		else if (ackComparator == 1 && receivedFrame.MQTT_SN_message[7] == 0x01)
		{
			ackComparator = 0;
			// SET TIMER TWAIT
			timerDurationTime = twait; // The timer has a duration of 10 seconds for testing
			start_timer1(); // The timer starts its operation
			// STATE CHANGE
			temporaryStateIndex = 2; // moves to the DECONGESTION state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		// LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|         Ends         ||------ CL S-01 -------|", 48, 1); // State indicator
		// | == CONFIGURE_CONNECT == || CL S-00 |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}
// ****************************************************************************************************************************** //

void state_2(void) // DECONGESTION S2
{
	uint8_t State[48] = "|=====DECONGESTION=====||       CL S-02        |";
	// | CFIN || |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator RX_Connect
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until TWAIT time has passed
		//////////////////////////////////////////////////////////////////////////
		// TIMER ENDS
		if (TimerEnd == 1) // If the condition indicating that the T adb time has ended is met
		{
			TimerEnd = 0; // Deactivate the reception of the timer signal since it ended
			transmit_sample_frame((uint8_t*)"|        TWAIT         ||       Ends       |", 48, 1);
			// | CFIN || |
			proc7Count = 3;
			controlLED(proc7Count); // Indicates how a message sequence has been processed
			// State change
			temporaryStateIndex = 0; // Moves to the ACTIVE SUBSCRIPTION or UNSUBSCRIBE state
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
		transmit_sample_frame((uint8_t*)"|         Ends         ||------ CL S-02 -------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}
// ****************************************************************************************************************************** //
void state_3(void) // RX_UNSUBACK S3
{
	uint8_t State[48] = "|======RX_UNSUBACK=====||       CL S-03        |";
	// | CFIN || === UPDATE_WILL === |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // waiting time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator
		enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		//////////////////////////////////////////////////////////////////////////
		// RX UNSUBACK
		if (successfulReceptionControl == 21) // The condition is activated when a UNSUBACK is received
		{
			successfulReceptionControl = -1; // Received the required message, other UNSUBACKs are not accepted
			proc7Count = 2;
			controlLED(proc7Count); // Indicates how a message sequence has been processed
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to ACTIVE SUBSCRIPTION or UNSUBSCRIBE state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		// LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // waiting time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|         Ends         ||------ CL S-03 -------|", 48, 1); // State indicator
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 to be able to use it later.
	}
}

// ****************************************************************************************************************************** //

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 8. GATEWAY'S_PUBLICATION_PROCEDURE ********************************************************* //
/*
// Procedure Variables //
int startRegistration = 0; // Indicates another procedure should start
int proc8Count = 0; // Indicates how a message sequence in this procedure ends
int topicIdComparator = -1; // Indicates an invalid topic ID
int QoSComparator = 0; // Prevents blocks when comparing QoS FLAGS
int selectPUBACK = 0; // Selects the PUBACK to transmit

// State Function Declarations //
void initialState(void); // WAIT PUBLICATION S0

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
		memset(&receivedFrame, 0, sizeof(receivedFrame)); // allocate memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // clear buffer data to prevent overlap

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];

		if (msgTypeMQTT_SN == 0x0C) // RX PUBLISH
		{
			enableReception = 0; // Disable reception to avoid interference with state changes
			successfulReceptionControl = 12; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue its operation.
		}
		else if (msgTypeMQTT_SN == 0x10) // RX PUBREL
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 16; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{
	// Callback function for transmitted frame events
}

void initialState(void) { // ACTIVE WAIT PUBLICATION S0

	uint8_t State[48] = "|====ACTIVE_WAIT_PUBLISH====||       CL S-00        |";
	// uint8_t State[48] = "|===ACTIVE_SUBSCRIPTION====||       CL S-00        |";
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the state start
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a PUBLISH with different QoS values 0,1,2
		//////////////////////////////////////////////////////////////////////////
		// RX PUBLISH
		if (successfulReceptionControl == 12) // Activated when a PUBLISH is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			topicIdComparator = 1; // invalid = 0, valid = 1
		}
		//-------------------------------Valid Topic ID----------------------------------------------------//
		else if (topicIdComparator == 1)
		{
			topicIdComparator = -1;
			QoSComparator = 1; // Allows comparing QoS of PUBLISH msg
		}// RX PUBLISH QoS 0
		else if (QoSComparator == 1 && receivedFrame.MQTT_SN_message[2] == 0b00000000)
		{
			QoSComparator = 0;
			proc8Count = 1;
			controlLED(proc8Count); // Indicates another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT PUBLICATION state
			stateControl = 4;
		}// RX PUBLISH QoS 1
		else if (QoSComparator == 1 && receivedFrame.MQTT_SN_message[2] == 0b00100000)
		{
			QoSComparator = 0;
			jump = 1;
		}// DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to appreciate the response to a reception
			selectPUBACK = 2;
			enableTransmission = 11;
		}// TR PUBACK
		else if (enableTransmission == 11 && selectPUBACK == 2) // Proceed to transmit a PUBACK as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			selectPUBACK = 0;
			uint8_t ReturnCode1; ReturnCode1 = 0x00; // 0x00,0x01,0x03
			messageLength = funcionPUBACK(PUBACK, ReturnCode1);
			transmit_sample_frame(PUBACK, messageLength, 0); // Message sending
			messageLength = 0;
			proc8Count = 2;
			controlLED(proc8Count); // Indicates another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT PUBLICATION state
			stateControl = 4;
		}// RX PUBLISH QoS 2
		else if (QoSComparator == 1 && receivedFrame.MQTT_SN_message[2] == 0b01000000)
		{
			QoSComparator = 0;
			jump = 2;
		}// DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to appreciate the response to a reception
			enableTransmission = 15;
		}// TR PUBREC
		else if (enableTransmission == 15) // Proceed to transmit a PUBREC as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBREC message is needed
			messageLength = funcionPUBREC(PUBREC); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBREC, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // Change to RX_PUBREL state
			stateControl = 4;
		}
		//--------------------------------------------------------------------------------------------//
		//------------------------------------Invalid Topic ID------------------------------------------//
		else if (topicIdComparator == 0)
		{
			topicIdComparator = -1;
			jump = 3; // Delay added for transmission
		}// DELAY
		else if (jump == 3)
		{
			jump = 0; // Only one jump
			delay_ms(4000); // Wait time to appreciate the response to a reception
			selectPUBACK = 1;
			enableTransmission = 11;
		}// TR PUBACK
		else if (enableTransmission == 11 && selectPUBACK == 1) // Proceed to transmit a PUBACK as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a REGACK message is needed
			selectPUBACK = 0;
			uint8_t ReturnCodeReject2; ReturnCodeReject2 = 0x02; // returnCode: invalid topicID
			messageLength = funcionPUBACK(PUBACK, ReturnCodeReject2);
			transmit_sample_frame(PUBACK, messageLength, 0); // Message sending
			messageLength = 0;
			startRegistration = 1;
			controlAllLEDs(startRegistration); // Indicates another process should start
			// STATE CHANGE
			temporaryStateIndex = 0; // Change to WAIT PUBLICATION state
			stateControl = 4;
		}
		//-------------------------------------------------------------------------------------------//
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

// ****************************************************************************************************************************** //

void state_1(void) { // RX_PUBREL S1

	uint8_t State[48] = "|=======RX_PUBREL======||       CL S-01        |";
	// uint8_t State[48] = "|===ACTIVE_SUBSCRIPTION===||       CL S-00        |";
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a PUBREL
		//////////////////////////////////////////////////////////////////////////
		// RX PUBREL
		if (successfulReceptionControl == 16) // Activated when a PUBREC is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			jump = 1; // Delay added for transmission
		}// DELAY
		else if (jump == 1)
		{
			jump = 0;
			delay_ms(4000); // Wait time to appreciate the response to a reception
			enableTransmission = 14; // Proceed to send a PUBREL
		}
		else if (enableTransmission == 14) // Proceed to transmit a PUBCOMP as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a WILLTOPICREQ message is needed
			messageLength = funcionPUBCOMP(PUBCOMP); // Invocation of the function that creates the message to be sent, returns the message length
			transmit_sample_frame(PUBCOMP, messageLength, 0); // Message sending
			messageLength = 0;
			proc8Count = 3;
			controlLED(proc8Count); // Indicates how this procedure ends
			// State Change
			temporaryStateIndex = 0; // ACTIVE WAIT PUBLICATION state
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
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-01 -------|", 48, 1); // State indicator
		// transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
		
	}
}
// ****************************************************************************************************************************** //
//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 9. KEEP_ALIVE_AND_PING_PROCEDURE *********************************************************** //
/*
// Procedure Variables //
int duration = 20; // Duration established during the connection
int tkeepAlive = 0; // Duration of the TKA timer
int lostClient = 0; // Client is considered lost when the timer ends
bool random_2 = true; // Initiates the first timer
int proc9Count = 0;
int messageAlert = 0;

// State Function Declarations //
void initialState(void); // WAIT PINGREQ S0

void state_1(void); // RX_PINGRESP S1

void usr_wireless_app_task(void)
{
	if (random_2 == true) // Assignment of the TKA timer
	{
		random_2 = false;
		// Assignment of retransmission times
		tkeepAlive = duration;
		timerDurationTime = tkeepAlive; // The timer has a duration of 15 seconds*NADV for tests (TADV must have a duration of 15 min)
		start_timer1(); // The timer starts its operation
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

	uint8_t State[48] = "|==WAIT_PINGREQ==||       CL S-00        |";
	//                  |         CFIN         ||===ACTUALIZAR_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
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
			delay_ms(400); // Wait time to appreciate the response to a reception
			enableTransmission = 22; // Proceed to send a PINGREQ
		}// TX msg PINGREQ
		else if (enableTransmission == 22) // Proceed to transmit a PINGREQ as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PINGREQ message is needed
			messageLength = funcionPINGREQ(PINGREQ);
			transmit_sample_frame(PINGREQ, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // RX_PINGRESP state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (TimerEnd == 1)
		{
			TimerEnd = 0;
			transmit_sample_frame((uint8_t*)"|    TKA:Ends    ||  client:Lost   |", 48, 1);
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
		transmit_sample_frame((uint8_t*)"|      End       ||------ CL S-00 -------|", 48, 1); // State indicator
		stateIndex = temporaryStateIndex; //
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // RX_PINGRESP S1

	uint8_t State[48] = "|==RX_PINGRESP==||       CL S-01        |";
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of the state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
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
			tkeepAlive = duration;
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
		stateControl = 1;
		transmit_sample_frame((uint8_t*)"|      End       ||------ CL S-01 -------|", 48, 1); // State indicator
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
int prepareAlert = 0;
int proc10Count = 0; // Indicates how a message sequence ends in this procedure

// State Function Declarations //
void initialState(void); // ACTIVE DISCONNECTION S0
void state_1(void);     // RX_DISCONNECT S1
void state_2(void);     // DISCONNECTED S2
void state_3(void);     // ESTABLISH CONNECTION S3

void usr_wireless_app_task(void)
{
	if (stateIndex == 0)       // ACTIVE DISCONNECTION S0
	{
		initialState();
	}
	else if (stateIndex == 1)  // RX_DISCONNECT S1
	{
		state_1();
	}
	else if (stateIndex == 2)  // DISCONNECTED S2
	{
		state_2();
	}
	else if (stateIndex == 3)  // ESTABLISH CONNECTION S3
	{
		state_3();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
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

// ****************************************************************************************************************************** //
void initialState(void) { // ACTIVE DISCONNECTION S0
	
	uint8_t State[48] = "|====ACTIVE_DISCONNECTION===||       CL S-00        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it wants to Close Connection or receives a disconnection
		//////////////////////////////////////////////////////////////////////////
		// Button to Close Connection == TRUE
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate Close Connection == TRUE
		{
			delay_ms(200); // Delay for button to work
			prepareAlert = 1;
		}
		else if (prepareAlert == 1)
		{
			prepareAlert = 0;
			transmit_sample_frame((uint8_t*)"|        close        ||    Connection==TRUE    |", 48, 1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			jump = 1;
		}
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(200); // Wait time to send DISCONNECT
			enableTransmission = 24; // Proceed to send a DISCONNECT
		}//TR DISCONNECT
		else if (enableTransmission == 24) // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			messageLength = funcionDISCONNECT(DISCONNECT, 'd');
			transmit_sample_frame(DISCONNECT, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // Change to RX_DISCONNECT state
			stateControl = 4;
		}//////////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		else if (successfulReceptionControl == 24) // Activated when a DISCONNECT is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			prepareAlert = 2;
			delay_ms(1500);
		}
		else if (prepareAlert == 2)
		{
			prepareAlert = 0;
			transmit_sample_frame((uint8_t*)"|   startNewConnection  ||       Connection       |", 48, 1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			proc10Count = 2;
			controlLED(proc10Count);
			// STATE CHANGE
			temporaryStateIndex = 3; // Change to ESTABLISH CONNECTION state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		//LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // Change of state to STATE RX_TOPIC
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) { // RX_DISCONNECT S1
	
	uint8_t State[48] = "|=====RX_DISCONNECT====||       CL S-01        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it receives a DISCONNECT
		//////////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		if (successfulReceptionControl == 24) // Activated when a DISCONNECT is received
		{
			successfulReceptionControl = -1; // Received the required message, no other messages are accepted
			proc10Count = 1;
			controlLED(proc10Count);
			// STATE CHANGE
			temporaryStateIndex = 2; // Change to DISCONNECTED state S2
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		//LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-01 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateIndex = temporaryStateIndex; // Change of state to STATE RX_TOPIC
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_2(void) { // DISCONNECTED S2
	
	uint8_t State[48] = "|=====DISCONNECTED=====||       CL S-02        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until a new Connection is started
		//////////////////////////////////////////////////////////////////////////
		// Button to indicate New Connection == 1
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate Close Connection == TRUE
		{
			delay_ms(200); // Delay for button to work
			prepareAlert = 1;
		}
		else if (prepareAlert == 1)
		{
			prepareAlert = 0;
			transmit_sample_frame((uint8_t*)"|       start        ||     Connection==1     |", 48, 1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			proc10Count = 4;
			controlLED(proc10Count);
			// STATE CHANGE
			temporaryStateIndex = 3; // Change to ESTABLISH CONNECTION state S2
			stateControl = 4;
		}
		////////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-02 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // Change of state to STATE RX_TOPIC
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_3(void) { // ESTABLISH CONNECTION S3
	
	uint8_t State[48] = "|=====ESTABLISH_CONNECTION=====||       CL S-03        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
	}
	else if (stateControl == 3)
	{
		// A Connection procedure must be started
		//////////////////////////////////////////////////////////////////////////
		delay_ms(5000);
		stateControl = 4;
		temporaryStateIndex = 3;
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-03 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // Change of state to STATE RX_TOPIC
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 11. CLIENT’S_RETRANSMISSION_PROCEDURE ****************************************************** //
/*
// Procedure Variables //
int Nmax = 3; // Maximum number of retries (N-1)
int tretry = 10; // Retransmission time
int proc11Count = 0; // Indicates how a message sequence ends in this procedure
int Nretry = 0; // Retry counter
int prepareAlert = 0;

// State Function Declarations //
void initialState(void); // ACTIVE S0
void state_1(void);     // RX_ACK S1
void state_2(void);     // ESTABLISH CONNECTION S2

void usr_wireless_app_task(void)
{
	if (stateIndex == 0)       // ACTIVE S0
	{
		initialState();
	}
	else if (stateIndex == 1)  // RX_ACK S1
	{
		state_1();
	}
	else if (stateIndex == 2)  // ESTABLISH CONNECTION S2
	{
		state_2();
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
		
		if (msgTypeMQTT_SN == 0x1D) // RX PUBACK
		{
			enableReception = 0; // Disable reception to avoid interference with state changes.
			successfulReceptionControl = 13; // This variable allows the node to exit the reception loop
			// in which it is located and allows the state to continue with its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{

}

void initialState(void) { // ACTIVE S0
	
	uint8_t State[48] = "|=========ACTIVE=========||       CL S-00        |";
	//|         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
		enableReception = 1; // Enable function to receive messages
	}
	else if (stateControl == 3)
	{
		//////////////////////////////////////////////////////////////////////
		// BUTTON readyMsgUnicast == 1
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate Unicast Message sending
		{
			delay_ms(200); // Delay for button to work
			prepareAlert = 1;
		}
		else if (prepareAlert == 1)
		{
			prepareAlert = 0;
			transmit_sample_frame((uint8_t*)"|       readyMsg       ||      Unicast==1      |", 48, 1);
			// TIMER TRETRY
			timerDurationTime = tretry; // The timer has a duration of 10 seconds for testing (should be 10-15 sec)
			start_timer1(); // Timer starts its operation
			jump = 1;
		}
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(400); // Time to wait for a response to a reception
			enableTransmission = 12; // Proceed to send a Unicast Message
		}//TR PUBLISH
		else if (enableTransmission == 12) // Proceed to transmit a PUBLISH as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			uint8_t DATA[] = "Payload-QoS1";
			uint8_t flagsP;
			flagsP = 0b00100000; // flagsP field, check MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message sending
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // Change to RX_ACK state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		//LED_Off(LED0);
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateIndex = temporaryStateIndex; // Change of state to RX_TOPIC
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// *********************************************************************************** //
void state_1(void) { // RX_ACK S1
	
	uint8_t State[48] = "|=========RX_ACK=========||        S-01          |";
	//                                          |        S-01          |
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator
		enableReception = 1;
	}
	else if (stateControl == 3)
	{
		//////////////////////////////////////////////////////////////////////
		// RX: ACK (e.g., RX: PUBACK)
		if (successfulReceptionControl == 13) // Activated when a PUBACK is received
		{
			successfulReceptionControl = 0; // Received the required message, no other messages are accepted
			stop_timer1();
			proc11Count = 2;
			controlLED(proc11Count);
			// STATE CHANGE
			temporaryStateIndex = 0; // Return to the PREPARING UNICAST MSG state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////
		// Finish TRETRY Timer
		else if (TimerEnd == 1) // If the condition indicating that the Tretry time ends is met
		{
			TimerEnd = 0; // Deactivate receiving the timer signal since it has ended
			transmit_sample_frame((uint8_t*)"|        TRETRY        ||      :Finish       |", 48, 1);
			jump = 1; // Move to use a delay
		}
		else if (jump == 1) // Add a delay to be able to transmit another message later
		{
			jump = 0;
			delay_ms(200); // Delay
			enableTransmission = 12; // Proceed to send a Unicast Message
		}// RTX: Unicast PUBLISH Message
		else if (enableTransmission == 12) // Proceed to transmit a PUBLISH as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a PUBLISH message is needed
			uint8_t DATA[] = "Payload-QoS1";
			uint8_t flagsP;
			flagsP = 0b00100000; // flagsP field, check MQTT-SN documentation
			messageLength = functionPUBLISH(PUBLISH, flagsP, DATA, (sizeof(DATA)) - 1);
			transmit_sample_frame(PUBLISH, messageLength, 0); // Message sending
			messageLength = 0;
			addTimer = 2;
		}// Restart TRETRY Timer
		else if (addTimer == 2)
		{
			addTimer = 0; // The timer is assigned only once, so the variable returns to 0
			Nretry = Nretry + 1; // Retry counter increases by one
			// SET TRETRY Timer
			timerDurationTime = tretry; // The timer has a duration of 10 seconds for testing (should be 10-15 sec)
			start_timer1(); // Timer starts its operation
			// STATE CHANGE
			temporaryStateIndex = 1; // RX_ACK
			stateControl = 4;
		}// Nretry == Nmax
		///////////////////////////////////////////////////////////////////////
		else if (Nretry == Nmax)
		{
			Nretry = 0;
			delay_ms(200);
			prepareAlert = 2;
		}
		else if (prepareAlert == 2)
		{
			prepareAlert = 0;
			transmit_sample_frame((uint8_t*)"|        Nretry        ||      Reached       |", 48, 1);
			stop_timer1();
			proc11Count = 1;
			controlLED(proc11Count);
			// STATE CHANGE
			temporaryStateIndex = 2; // ESTABLISH CONNECTION
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1;
		transmit_sample_frame((uint8_t*)"|       End        ||---------S-01---------|", 48, 1); // State indicator
		//|=========S-01=========|
		stateIndex = temporaryStateIndex; // Change of state to RX
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}
// ***************************************************************************************************************************** //
void state_2(void) { // ESTABLISH CONNECTION S2
	
	uint8_t State[48] = "|=====ESTABLISH_CONNECTION=====||       CL S-02        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // Wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // State indicator RX_Connect
	}
	else if (stateControl == 3)
	{
		// A Connection procedure must be started
		//////////////////////////////////////////////////////////////////////
		delay_ms(5000);
		stateControl = 4;
		temporaryStateIndex = 3;
		//////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // Wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-02 -------|", 48, 1); // State indicator
		//|==CONFIGURE_CONNECTION==||       CL S-00        |
		stateIndex = temporaryStateIndex; // Change of state to ESTABLISHED CONNECTION
		temporaryStateIndex = -1; // Set to -1 for later use.
	}
}

//*/
// ****************************************************************************************************************************** //

// ------------------------------------------------------------------------------------------------------------------------------ //
// ********************************* 12. SUPPORT_OF_SLEEPING_CLIENTS ************************************************************ //
/*
// Procedure variables //
int durationS = 20; // 20 seconds for testing, The sleep duration should be the same as DISCONNECT to send
int proc12Count = 0; // Indicates how a sequence of messages sent in this procedure ends

int pulseCounter = 0; // Allows using multiple pulses in a state
int pulseComparator = 0;
char disconnectType = 'n'; // Allows adding the duration field to a DISCONNECT
int warning = 0;

// Declaration of State Functions //
void initialState(void); // ACTIVE S0

void state_1(void); // CONFIRM SLEEP S1

void state_2(void); // ASLEEP S2

void state_3(void); // AWAKE S3

void state_4(void); // RX_DISCONNECT S4

void state_5(void); // DISCONNECTED S5

void state_6(void); // ESTABLISH CONNECTION S6

void usr_wireless_app_task(void)
{
	if (stateIndex == 0) // ACTIVE S0
	{
		initialState();
	}
	else if (stateIndex == 1) // CONFIRM SLEEP S1
	{
		state_1();
	}
	else if (stateIndex == 2) // ASLEEP S2
	{
		state_2();
	}
	else if (stateIndex == 3) // AWAKE S3
	{
		state_3();
	}
	else if (stateIndex == 4) // RX_DISCONNECT S4
	{
		state_4();
	}
	else if (stateIndex == 5) // DISCONNECTED S5
	{
		state_5();
	}
	else if (stateIndex == 6) // ESTABLISH CONNECTION S6
	{
		state_6();
	}
}

void usr_frame_received_cb(frame_info_t *frame)
{
	if (enableReception == 1) // State index
	{

		memset(&receivedFrame, 0, sizeof(receivedFrame)); // allocate memory space
		memcpy(&receivedFrame, frame->mpdu, sizeof(receivedFrame)); // copy information from source memory (buffer) to destination
		bmm_buffer_free(frame->buffer_header); // Clear buffer data, avoid overlap.

		uint8_t msgTypeMQTT_SN;
		msgTypeMQTT_SN = receivedFrame.MQTT_SN_message[1];
		if (msgTypeMQTT_SN == 0x18) // RX DISCONNECT
		{
			enableReception = 0; // Disables reception to avoid interference in state change.
			successfulReceptionControl = 24; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
		else if (msgTypeMQTT_SN == 0x17) // RX PINGRESP
		{
			enableReception = 0; // Disables reception to avoid interference in state change.
			successfulReceptionControl = 23; // This variable allows the node to exit the reception loop
			// it is in and allows the state to continue its operation.
		}
	}
}

void usr_frame_transmitted_cb(retval_t status, frame_info_t *frame)
{
}

// ****************************************************************************************************************************** //
void initialState(void) // ACTIVE S0
{

	uint8_t State[48] = "|========ACTIVE========||       CL S-00        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator RX_Connect
		//enableReception = 1; // Variable that enables the function that receives messages.
	}
	else if (stateControl == 3)
	{
		// The node will remain in this state until it wants to Save Energy (1 Pulse)
		// or closingConnection is required (2 pulses)
		//////////////////////////////////////////////////////////////////////
		// HIGHER LAYERS RX BUTTON
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) // To recreate the RX of HIGHER LAYERS
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
			timerDurationTime = 3; // The timer has a duration of 3 seconds for testing
			start_timer1(); // The timer starts its operation
		} // Timer ends
		else if (TimerEnd == 1) // If the condition indicating that the time has ended is met
		{
			TimerEnd = 0; // Disable the timer signal reception since it has ended
			pulseComparator = pulseCounter; // unnecessary pulses are avoided
			pulseCounter = 0;
		}
		//////////////////////////////////////////////////////////////////////
		// Save Energy == 1 (1 Pulse)
		else if (pulseComparator == 1)
		{
			pulseComparator = 0;
			transmit_sample_frame((uint8_t*)"|        save       ||      Energy==1      |", 48, 1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			jump = 1; // Move to using a delay
		} // DELAY
		else if (jump == 1)
		{
			jump = 0; // Only one jump
			delay_ms(200); // wait time to send DISCONNECT
			disconnectType = 's';
			enableTransmission = 24; // Proceed to send DISCONNECT
		} // TR DISCONNECT
		else if (enableTransmission == 24 && disconnectType == 's') // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			disconnectType = 'n';
			messageLength = funcionDISCONNECT(DISCONNECT, 's'); // (A time should be added for the node to sleep)
			transmit_sample_frame(DISCONNECT, messageLength, 0); // message sent
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 1; // CONFIRM SLEEP state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////
		// closeConnection == 1 (2nd pulse)
		else if (pulseComparator == 2)
		{
			pulseComparator = 0;
			transmit_sample_frame((uint8_t*)"|         close       ||       Connection==1    |", 48, 1);
			//                               |       NewList     ||    WILL_TOPIC=TRUE   |// A message is transmitted indicating that T adv Timer has ended
			jump = 2; // Move to using a delay
		} // DELAY
		else if (jump == 2)
		{
			jump = 0; // Only one jump
			delay_ms(200); // wait time to send DISCONNECT
			disconnectType = 'd';
			enableTransmission = 24; // Proceed to send DISCONNECT
		} // TR DISCONNECT
		else if (enableTransmission == 24 && disconnectType == 'd') // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission = -1; // Transmission is disabled because only a DISCONNECT message is needed
			disconnectType = 'n';
			messageLength = funcionDISCONNECT(DISCONNECT, 'd'); // (A time should be added for the node to sleep)
			transmit_sample_frame(DISCONNECT, messageLength, 0); // message sent
			messageLength = 0;
			// STATE CHANGE
			temporaryStateIndex = 4; // RX_DISCONNECT state
			stateControl = 4;
		}
		//////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-00 -------|", 48, 1); // State Indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 so that it can be used later.
	}
}
// ****************************************************************************************************************************** //
void state_1(void) // CONFIRM SLEEP S1
{

	uint8_t State[48] = "|====CONFIRM_SLEEP===||       CL S-01        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl == 1)
	{
		stateControl = 2;
		delay_ms(1500); // wait time to appreciate the start of state
	}
	else if (stateControl == 2)
	{
		stateControl = 3;
		transmit_sample_frame(State, 48, 1); // state indicator
		enableReception = 1;
	}
	else if (stateControl == 3)
	{
		// The node remains in this state until a DISCONNECT is received and then a timer starts
		////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		if (successfulReceptionControl == 24 && receivedFrame.MQTT_SN_message[0] == 0x02) // The condition is activated when a CONNECT 04 Message is received
		{
			successfulReceptionControl = -1; // Received the required message no other accepted
			addTimer = 1;
		} // Add Ts TIMER
		else if (addTimer == 1)
		{
			addTimer = 0; // The timer is only assigned once so the variable returns to 0
			timerDurationTime = durationS; // The timer has a duration of 20 seconds for testing (should be <> 1 min)
			start_timer1(); // The timer starts its operation
			// STATE CHANGE
			temporaryStateIndex = 2; // ASLEEP State
			stateControl = 4;
		}
		////////////////////////////////////////////////////////////////////
	}
	else if (stateControl == 4)
	{
		stateControl = 5;
		delay_ms(1500); // wait time to appreciate the end of the state
	}
	else if (stateControl == 5)
	{
		stateControl = 0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-01 -------|", 48, 1); // State Indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl = 1; // This control variable is set to one so that the following states can start their operation.
		stateIndex = temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex = -1; // It is set to -1 so that it can be used later.
	}
}

// ****************************************************************************************************************************** //
void state_2(void){// ASLEEP S2
	
	uint8_t State[48]="|========ASLEEP=======||       CL S-02        |";
	//                  |         CFIN         ||===UPDATE_WILL====|
	if (stateControl==1)
	{
		stateControl=2;
		delay_ms(1500);// wait time to appreciate the start of state
	}
	else if (stateControl==2)
	{
		stateControl=3;
		transmit_sample_frame(State,48,1);// state indicator RX_Connect
		enableReception=1;// Variable that enables the function that receives messages.
	}
	else if (stateControl==3)
	{
		// The node will remain in this state until it wants to Save Energy (1 Pulse)
		// , closingConnection is required (2 pulses), or the sleep timer Ts ends
		//////////////////////////////////////////////////////////////////////
		// RX HIGHER LAYERS BUTTON
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))// To recreate the RX of HIGHER LAYERS
		{
			delay_ms(200); // Delay for the button to work
			pulseCounter++;
			if (pulseCounter==1)
			{
				addTimer=2; // When the first pulse arrives, the Timer configuration will continue
			}
		}// PULSE TIMER
		else if (addTimer==2)
		{
			addTimer=0;// The timer is only assigned once so the variable returns to 0
			stop_timer1();
			timerDurationTime=3;// The timer has a duration of 3 seconds for testing
			start_timer1();// The timer starts its operation
		}// Timer ends
		else if (TimerEnd==1 && pulseCounter>0)// If the condition indicating that the time has ended is met
		{
			TimerEnd=0;// Disable the timer signal reception since it has ended
			pulseComparator=pulseCounter;// unnecessary pulses are avoided
			pulseCounter=0;
		}
		//////////////////////////////////////////////////////////////////////
		// saveEnergy==0 (1 Pulse)
		else if (pulseComparator==1)
		{
			pulseComparator=0;
			transmit_sample_frame((uint8_t*)"|        save       ||      Energy==0      |",48,1);
			//                               |------ CL S-00 -------||------ CL S-00 -------|
			proc12Count=1;
			controlLED(proc12Count); // Indicates how a sequence of messages has been processed
			// STATE CHANGE
			temporaryStateIndex=6;// ESTABLISH CONNECTION state
			stateControl=4;
		}
		//////////////////////////////////////////////////////////////////////
		// closeConnection==1 (2nd pulse)
		else if (pulseComparator==2)
		{
			pulseComparator=0;
			transmit_sample_frame((uint8_t*)"|        close      ||     Connection==1    |",48,1);
			//                               |       NewList     ||    WILL_TOPIC=TRUE   |// A message is transmitted indicating that T adv Timer has ended
			jump=1;// Move to using a delay
		}// DELAY
		else if (jump==1)
		{
			jump=0; // Only one jump
			delay_ms(200);// wait time to send DISCONNECT
			disconnectType='d';
			enableTransmission=24;// Proceed to send DISCONNECT
		}// TR DISCONNECT
		else if (enableTransmission==24&& disconnectType=='d') // Proceed to transmit a DISCONNECT as indicated by the variable value
		{
			enableTransmission=-1;// Transmission is disabled because only a DISCONNECT message is needed
			disconnectType='n';
			messageLength=funcionDISCONNECT(DISCONNECT,'d');// (A time should be added for the node to sleep)
			transmit_sample_frame(DISCONNECT,messageLength,0);// message sent
			messageLength=0;
			// STATE CHANGE
			temporaryStateIndex=4;// RX_DISCONNECT state
			stateControl=4;
		}
		//////////////////////////////////////////////////////////////////////
		// Timer Ts ends
		else if (TimerEnd==1 )// If the condition indicating that the time Ts has ended is met
		{
			TimerEnd=0;// Disable the timer signal reception since it has ended
			transmit_sample_frame((uint8_t*)"|      TIMER_sleep     ||      Ts:End     |",48,1);
			//                               |       NewList     ||    WILL_TOPIC=TRUE   |
			jump=2;// Move to using a delay
		}
		else if (jump==2)
		{
			jump=0; // Only one jump
			delay_ms(400);// wait time to appreciate the response to a reception
			enableTransmission=22;// Proceed to send a PINGREQ Message
		}// TX PINGREQ
		else if (enableTransmission==22) // Proceed to transmit a PINGREQ as indicated by the variable value
		{
			enableTransmission=-1;// Transmission is disabled because only a PINGREQ message is needed
			messageLength=funcionPINGREQ(PINGREQ);
			transmit_sample_frame(PINGREQ,messageLength,0);// message sent
			messageLength=0;
			// STATE CHANGE
			temporaryStateIndex=3;// Change to AWAKE state
			stateControl=4;
		}
		//////////////////////////////////////////////////////////////////////
	}
	else if (stateControl==4)
	{
		stateControl=5;
		delay_ms(1500);// wait time to appreciate the end of the state
	}
	else if (stateControl==5)
	{
		stateControl=0; // This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       End        ||------ CL S-02 -------|",48,1);// State Indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl=1; // This control variable is set to one so that the following states can start their operation.
		stateIndex=temporaryStateIndex; // State change to STATE RX_TOPIC
		temporaryStateIndex=-1;// It is set to -1 so that it can be used later.
	}
}
// ****************************************************************************************************************************** //
void state_3(void){//AWAKE STATE S3
	
	uint8_t State[48]="|=======AWAKE======||       CL S-03        |";
	//                  |         END         ||===UPDATE_WILL====|
	if (stateControl==1)
	{
		stateControl=2;
		delay_ms(1500);// wait time to appreciate the start of the state
	}
	else if (stateControl==2)
	{
		stateControl=3;
		transmit_sample_frame(State,48,1);// state indicator RX_Connect
		//addTimer=1;// Variable enabling the function to receive messages.
	}
	else if (stateControl==3)
	{
		//The node will remain in this state until it receives a PINGRESP
		//////////////////////////////////////////////////////////////////////////
		// RX PINGRESP
		if (successfulReceptionControl==23)// Condition activated when a PINGRESP is received
		{
			successfulReceptionControl=0;// Reception is disabled to avoid blocking
			addTimer=1;
		}// add TIMER Ts
		else if (addTimer==1)
		{
			addTimer=0;// The timer is only assigned once so the variable returns to 0
			timerDurationTime=durationS;// The timer has a duration of 20 seconds for testing (should be <> 1 min)
			start_timer1(); // The timer starts its operation
			// STATE CHANGE
			temporaryStateIndex=2;// Moves to SLEEP state
			stateControl=4;
		}
	}
	else if (stateControl==4)
	{
		stateControl=5;
		delay_ms(1500);// wait time to appreciate the end of the state
	}
	else if (stateControl==5)
	{
		stateControl=0; //This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Finished        ||------ CL S-03 -------|",48,1);//State indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl=1; //This control variable is set to one so that the following states can start their operation.
		stateIndex=temporaryStateIndex; //State change to STATE RX_TOPIC
		temporaryStateIndex=-1;//Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_4(void){//Waiting for RX_DISCONNECT
	
	uint8_t State[48]="|=====RX_DISCONNECT====||       CL S-04        |";
	//                  |         END         ||===UPDATE_WILL====|
	if (stateControl==1)
	{
		stateControl=2;
		delay_ms(1500);// wait time to appreciate the start of the state
	}
	else if (stateControl==2)
	{
		stateControl=3;
		transmit_sample_frame(State,48,1);// state indicator
		enableReception=1;
	}
	else if (stateControl==3)
	{

		//The node remains in this state until it receives a DISCONNECT
		//////////////////////////////////////////////////////////////////////////////////////
		// RX DISCONNECT
		if (successfulReceptionControl==24 && receivedFrame.MQTT_SN_message[0]==0x02)// Condition activated when a CONNECT message 04 is received
		{
			successfulReceptionControl=-1;// The required message has been received and no others are accepted
			// STATE CHANGE
			temporaryStateIndex=5;// Moves to DISCONNECTED state
			stateControl=4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl==4)
	{
		stateControl=5;
		delay_ms(1500);// wait time to appreciate the end of the state
	}
	else if (stateControl==5)
	{
		stateControl=0; //This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Finished        ||------ CL S-04 -------|",48,1);//State indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl=1; //This control variable is set to one so that the following states can start their operation.
		stateIndex=temporaryStateIndex; //State change to STATE RX_TOPIC
		temporaryStateIndex=-1;//Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_5(void){//Waiting for DISCONNECTED
	
	uint8_t State[48]="|=====DISCONNECTED=====||       CL S-05        |";
	//                  |         END         ||===UPDATE_WILL====|
	if (stateControl==1)
	{
		stateControl=2;
		delay_ms(1500);// wait time to appreciate the start of the state
	}
	else if (stateControl==2)
	{
		stateControl=3;
		transmit_sample_frame(State,48,1);// state indicator
	}
	else if (stateControl==3)
	{
		//The node will remain in this state until a new connection is initiated (1 Pulse)
		//////////////////////////////////////////////////////////////////////////
		// BUTTON iniConnection==1
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
		{
			delay_ms(200); //Delay for the button to work
			warning=1;
		}
		else if (warning==1)
		{
			warning=0;
			transmit_sample_frame((uint8_t*)"|        Starting       ||     Connection==1      |",48,1);//State indicator
			//                              "|       Finished        ||------ CL S-05 -------|",48,1);//State indicator
			proc12Count=2;
			controlLED(proc12Count); // Indicates how a sequence of messages has been processed
			// STATE CHANGE
			temporaryStateIndex=6;// Moves to ESTABLISH CONNECTION state
			stateControl=4;
			
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl==4)
	{
		stateControl=5;
		delay_ms(1500);// wait time to appreciate the end of the state
	}
	else if (stateControl==5)
	{
		stateControl=0; //This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Finished        ||------ CL S-05 -------|",48,1);//State indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl=1; //This control variable is set to one so that the following states can start their operation.
		stateIndex=temporaryStateIndex; //State change to STATE RX_TOPIC
		temporaryStateIndex=-1;//Set to -1 for later use.
	}
}
// ****************************************************************************************************************************** //
void state_6(void){// ESTABLISH CONNECTION//First state of another procedure
	
	uint8_t State[48]="|=====ESTABLISH_CONNECTION=====||       CL S-06        |";
	//                  |         END         ||===UPDATE_WILL====|
	if (stateControl==1)
	{
		stateControl=2;
		delay_ms(1500);// wait time to appreciate the start of the state
	}
	else if (stateControl==2)
	{
		stateControl=3;
		transmit_sample_frame(State,48,1);// state indicator
	}
	else if (stateControl==3)
	{
		//The node will remain in this state until a new connection is initiated (1 Pulse)
		//////////////////////////////////////////////////////////////////////////
		// BUTTON iniConnection==1
		if (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0))
		{
			delay_ms(200); //Delay for the button to work
			controlAllLEDs(1); // Indicates how a sequence of messages has been processed
			// STATE CHANGE
			temporaryStateIndex=0;// Moves to ACTIVE state
			stateControl=4;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else if (stateControl==4)
	{
		stateControl=5;
		delay_ms(1500);// wait time to appreciate the end of the state
	}
	else if (stateControl==5)
	{
		stateControl=0; //This control variable is set to zero because it is only used in the initial state.
		transmit_sample_frame((uint8_t*)"|       Finished        ||------ CL S-06 -------|",48,1);//State indicator
		//|==CONFIGURE_CONNECT==||       CL S-00        |
		stateControl=1; //This control variable is set to one so that the following states can start their operation.
		stateIndex=temporaryStateIndex; //State change to STATE RX_TOPIC
		temporaryStateIndex=-1;//Set to -1 for later use.
	}
}

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

