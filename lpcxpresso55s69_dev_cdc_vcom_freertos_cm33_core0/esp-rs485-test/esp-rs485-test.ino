/*
 * ESP32-S3-WROOM-1 RS485 Echo Test
 * 
 * Hardware Setup:
 * - RS485 Transceiver: THVD1450
 * - UART Interface: Serial1
 * - Connect ESP32-S3 pins to the THVD1450 as defined below.
 * - The THVD1450's RE_N and DE pins should be tied together and driven by RS485_DE_PIN.
 */

#include <Arduino.h>

// Define ESP32-S3 pins (Change these to match your actual hardware wiring)
#define RS485_RX_PIN  4  // Connect to THVD1450 R (RO) pin
#define RS485_TX_PIN  5  // Connect to THVD1450 D (DI) pin
#define RS485_DE_PIN  6  // Connect to tied THVD1450 DE & RE_N pins

// Use Hardware Serial 1
HardwareSerial RS485(1);

void setup() {
  // Initialize standard USB serial for debugging/monitoring
  Serial.begin(9600);
  
  // Initialize the DE/RE control pin
  pinMode(RS485_DE_PIN, OUTPUT);
  // Set LOW immediately to enter Receive Mode
  digitalWrite(RS485_DE_PIN, LOW); 

  // Initialize Serial1 for RS485 communication
  // Baud rate: 115200, 8N1 standard
  RS485.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  Serial.println("\n--- ESP32-S3 RS485 Echo Test Ready ---");
  Serial.println("Listening for queries from LPC55S69...");
}

void loop() {
  //RS485.print("ACK");
  // Check if data is available from the RS485 bus
  if (RS485.available()) {
    // Read the incoming message until a newline character
    String incomingMsg = RS485.readStringUntil('\n');
    
    // Trim any trailing carriage returns (\r) or whitespace
    incomingMsg.trim();

    if (incomingMsg.length() > 0) {
      Serial.print("Received: ");
      Serial.println(incomingMsg);

      /* ================= TURNAROUND SEQUENCE ================= */
      
      // 1. Give the LPC55S69 a brief moment (e.g. 5ms) to ensure its 
      //    own transceiver has switched back to Receive mode.
      delay(5);

      // 2. Set DE High (Transmit Mode)
      digitalWrite(RS485_DE_PIN, HIGH);

      // 3. Send the response back. We append a newline (\n) because 
      //    the LPC55S69's query command expects it to signify end of message.
      // RS485.println("AAAAA");
      RS485.println(incomingMsg);
      // 4. BLOCK until all bytes have physically left the TX shift register!
      //    This is the equivalent of waiting for kUSART_TxIdleFlag on the LPC.
      RS485.flush();

      // 5. Set DE Low (Receive Mode)
      digitalWrite(RS485_DE_PIN, LOW);
      
      /* ======================================================= */
      
      Serial.println("Response sent.");
   }
  }
}
