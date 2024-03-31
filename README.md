# FSM-MQTT-SN_Over_IEEE_802.15.4
Simulations and Code for RCB256RFR2 Nodes of Finite State Machines

INSTRUCTIONS FOR NODES 

To run each finite state machine (FSM) encoded in the RCB25RFR2 nodes, you must install Atmel Studio 7 development environment 
on your PC and download each of the projects from the repository.Within each project in the repository, you will find the code 
that allows the nodes to handle each FSM of their respective MQTT-SN procedure, at the following path: 
Project Name \ Solution Name \ src \ usr_wireless.c (for example: Gateway\Gateway\src\ usr_wireless.c). 
You can access this file using Atmel Studio 7. Inside the usr_wireless.c file, you can see that each procedure has its respective 
denomination and they are independent of each other. The necessary code has been added to comment and uncomment each of the 
procedures. Only one procedure can be executed at a time; the others must remain commented. To uncomment a procedure, follow 
these steps:

• Find the respective denomination of the required procedure.

• On the line following the denomination, you will only find the characters "/*", locate it at the beginning of the line.

• Add the character '/' at the beginning of the aforementioned line; the procedure will be uncommented automatically.

• Verify that the other procedures are commented.

• Verify that the client node and gateway have the same procedure.

Once the above steps have been executed and if no compilation errors have been found, it is now possible to enter the code into 
each of the RCB25RFR2 nodes using Atmel Studio 7 IDE.


INSTRUCTIONS FOR SIMULATIONS

-Open JFLAP-7.0_With_Source.

-Load any of the .JFF files.

-Once the JFF file is open, load its respective input sequence from the .txt files.

-Execute the respective simulation step by step.