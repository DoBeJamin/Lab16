

#include "oled-wing-adafruit.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

OledWingAdafruit display;

//setup to advertise for a UART connection with a phone over BLE
const size_t UART_TX_BUF_SIZE = 20;
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) ;
const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);

String BLE_output;
int ble_int;

bool disconect = false;

void setup() {

	display.setup();
	format_display();
	display.print(" ");
	display.display();

	RGB.control(true);

	BLE.on();
	BLE.addCharacteristic(txCharacteristic);
	BLE.addCharacteristic(rxCharacteristic);

	BleAdvertisingData data;
	data.appendServiceUUID(serviceUuid);
	BLE.advertise(&data);
	BLE.setDeviceName("Benjamin");

	Serial.begin(9600);

}

void loop() {

	display.loop();

	if (disconect) {
		BLE.disconnect();
		disconect = false;
		RGB.color(255,255,0);
		delay(5000);
	}

	if (BLE.connected()) {
		//sends message to phone
		RGB.color(0,0,255);
		uint8_t txBuf[UART_TX_BUF_SIZE];
		String message = "connected to phone";
		message.toCharArray((char *)txBuf, message.length() + 1);
		txCharacteristic.setValue(txBuf, message.length() + 1);
	} else {
		RGB.color(255,255,0);
	}

}





void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
	//runs whenever data is recieved over BLE
    for (size_t i = 0; i < len; i++) {
		BLE_output = String(data[i]);
		ble_int = BLE_output.toInt();
		Serial.print(ble_int);
		if (ble_int == 48) { //asci value for 0
			disconect = true;
		} else if (ble_int == 49) { //asci value for 1
			format_display();
			display.print("phone button pressed");
			display.display();
		}
    }
}

void format_display () {
  //sets up basic formatting for when using the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
}