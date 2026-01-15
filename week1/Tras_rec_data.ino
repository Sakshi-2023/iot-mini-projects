#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "LoRaWan_APP.h"

// ================= OLED PINS (HELTEC V3) =================
#define OLED_SDA   17
#define OLED_SCL   18
#define OLED_RST   21
#define Vext       36

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0, OLED_RST, OLED_SCL, OLED_SDA
);

// ================= LORA CONFIG =================
#define RF_FREQUENCY           915000000   // change to 865000000 for India
#define TX_OUTPUT_POWER        14
#define LORA_BANDWIDTH         0
#define LORA_SPREADING_FACTOR  7
#define LORA_CODINGRATE        1
#define LORA_PREAMBLE_LENGTH   8
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON   false
#define RX_TIMEOUT_VALUE       1000
#define BUFFER_SIZE            64

// ================= VARIABLES =================
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

bool lora_idle = true;
float localValue = 0.0;
float remoteValue = 0.0;

unsigned long lastSend = 0;
const unsigned long sendInterval = 4000;

static RadioEvents_t RadioEvents;

// ================= POWER OLED =================
void VextON() {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

// ================= CALLBACKS =================
void OnTxDone(void) {
  lora_idle = true;
  Radio.Rx(0);
}

void OnTxTimeout(void) {
  lora_idle = true;
  Radio.Rx(0);
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';

  Serial.printf("Received: %s | RSSI: %d\n", rxpacket, rssi);

  String data = String(rxpacket);
  int idx = data.indexOf(':');
  if (idx != -1) {
    remoteValue = data.substring(idx + 1).toFloat();
  }

  lora_idle = true;
  Radio.Rx(0);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

  // OLED
  VextON();
  delay(100);
  Wire.begin(OLED_SDA, OLED_SCL, 400000);

  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);

  oled.begin();
  oled.setFont(u8g2_font_ncenB08_tr);
  oled.clearBuffer();
  oled.drawStr(0, 15, "LoRa Two-Way");
  oled.sendBuffer();

  // LoRa
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxDone = OnRxDone;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetTxConfig(
    MODEM_LORA, TX_OUTPUT_POWER, 0,
    LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
    LORA_CODINGRATE, LORA_PREAMBLE_LENGTH,
    LORA_FIX_LENGTH_PAYLOAD_ON, true,
    0, 0, LORA_IQ_INVERSION_ON, 3000
  );

  Radio.SetRxConfig(
  MODEM_LORA,
  LORA_BANDWIDTH,
  LORA_SPREADING_FACTOR,
  LORA_CODINGRATE,
  0,
  LORA_PREAMBLE_LENGTH,
  LORA_FIX_LENGTH_PAYLOAD_ON,
  0,
  true,
  0,
  0,
  LORA_IQ_INVERSION_ON,
  true,
  RX_TIMEOUT_VALUE
);


  Radio.Rx(0);
}

// ================= LOOP =================
void loop() {

  if (millis() - lastSend > sendInterval && lora_idle) {
    lastSend = millis();

    localValue = random(100, 999) / 10.0;
    sprintf(txpacket, "Value:%.2f", localValue);

    Serial.printf("Sending: %s\n", txpacket);
    Radio.Send((uint8_t *)txpacket, strlen(txpacket));
    lora_idle = false;
  }

  // OLED DISPLAY
  oled.clearBuffer();
  oled.drawStr(0, 12, "LoRa Two-Way");
  oled.drawStr(0, 30, ("My Val: " + String(localValue, 2)).c_str());
  oled.drawStr(0, 50, ("RX Val: " + String(remoteValue, 2)).c_str());
  oled.sendBuffer();

  Radio.IrqProcess();
  delay(100);
}
