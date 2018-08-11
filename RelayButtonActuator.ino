/**
 * 
 * DESCRIPTION
 * Sketch for a switch where you can control light or something 
 * else from an HA controller.
 * This node also works as a repeater for other nodes
 *
 */ 

// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Enabled repeater feature for this node
#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>

#define RELAY_PIN  4  // Arduino Digital I/O pin number for relay 
#define BUTTON_PIN  3  // Arduino Digital I/O pin number for button 
#define CHILD_ID 1   // Id of the sensor child
#define RELAY_ON 1
#define RELAY_OFF 0

Bounce debouncer = Bounce(); 
int oldValue=0;
bool state;

MyMessage msg(CHILD_ID,V_LIGHT);

void setup()  
{  
  // Make sure relays are off when starting up
  digitalWrite(RELAY_PIN, RELAY_OFF);
  // Then set relay pins in output mode
  pinMode(RELAY_PIN, OUTPUT);   

  // Set relay to last known state (using eeprom storage) 
  state = loadState(CHILD_ID);
  digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
}

void presentation()  {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Relay", "1.1");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_LIGHT);
}

/*
*  Example on how to asynchronously check for new messages from gw
*/
void loop() 
{
  debouncer.update();
  // Get the update value
  int value = debouncer.read();
  if (value != oldValue && value==0) {
      send(msg.set(state?false:true), true); // Send new state and request ack back
  }
  oldValue = value;
} 

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_LIGHT) {
     // Change relay state
     state = message.getBool();
     digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     saveState(CHILD_ID, state);

     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
