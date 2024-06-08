#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>
#define PAUSE               300
#define FREQUENCY           866.3       // for Europe

//Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
#define BANDWIDTH           250.0
#define SPREADING_FACTOR    9
#define TRANSMIT_POWER      20 //max 22, min -9

  String rxdata;
  volatile bool rxFlag = false;
  long counter = 0;
  uint64_t last_tx = 0;
  uint64_t tx_time;
  uint64_t minimum_pause;
  const unsigned int MAX_MESSAGE_LENGTH = 12;
  static char message[MAX_MESSAGE_LENGTH];
  static char deviceIdentifier[5] = "002%";
  static char finalMess[MAX_MESSAGE_LENGTH+5];
  
  void setup() {
    heltec_setup();
    both.println("Radio init");
    RADIOLIB_OR_HALT(radio.begin());
    radio.setDio1Action(rx);
    both.printf("Frequency: %.2f MHz\n", FREQUENCY);
    RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
    both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
    RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
    both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
    RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
    both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
    RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    Serial.begin(9600);
  }

  void loop() {
    heltec_loop();
    readFromSerial();
    sendMessageReadFromSerial();
    listenForIncomingMessage();
  }

  void rx() {
    rxFlag = true;
  }

  void readFromSerial() {
    while (Serial.available() > 0)
    {
     static unsigned int message_pos = 0;
     char inByte = Serial.read();
     if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
     {
       message[message_pos] = inByte;
       message_pos++;
     }
     else
     {
       message[message_pos] = '\0';
       Serial.println(message);
       message_pos = 0;
     }
    }
  }

  void sendMessageReadFromSerial() {
    if (strlen(message) > 0) {
        both.printf("TX [%s] ", message);
        radio.clearDio1Action();
        heltec_led(50); // 50% brightness is plenty for this LED
        tx_time = millis();

        strcpy(finalMess, deviceIdentifier);
        strcat(finalMess, message);

        RADIOLIB(radio.transmit(finalMess));
        tx_time = millis() - tx_time;
        heltec_led(0);
        if (_radiolib_status == RADIOLIB_ERR_NONE) {
          both.printf("OK (%i ms)\n", (int)tx_time);
        } else {
          both.printf("fail (%i)\n", _radiolib_status);
        }
        minimum_pause = tx_time * 100;
        last_tx = millis();
        radio.setDio1Action(rx);
        RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
        memset(message, 0, sizeof(message));
        memset(finalMess, 0, sizeof(finalMess));
      }
  }

  void listenForIncomingMessage() {
    if (rxFlag) {
      rxFlag = false;
      radio.readData(rxdata);
      if (_radiolib_status == RADIOLIB_ERR_NONE) {
        both.printf("RX [%s]\n", rxdata.c_str());
        both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
        both.printf("  SNR: %.2f dB\n", radio.getSNR());
      }
      RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    }
  }
