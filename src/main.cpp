#include <Arduino.h>
#include <BleKeyboard.h>

//#include "CustomBLE.h"
#include "ButtonState.cpp"
#include "ComboController.cpp"
#include "ArtseyIO.cpp"
//#define USE_NIMBLE

#define BATTERY_SENSE 190
#define BATTERY_PIN 35
#define BATTERY_STATE_REFRESH 1000

const int adcMultiplier = 790;
int nextBatteryRefresh = 0;

int buttonMap[] = {27, 26, 25, 33, 16, 17, 18, 19};

ButtonState buttonArray[8] = {27, 26, 25, 33, 16, 17, 18, 19};
ComboController controller(8);
BleKeyboard bleKeyboard;

void printCombo(unsigned long combo){
	Serial.print("Combo: ");
	char temp[9];
	for(int i = 0; i < 8; i++){
		temp[7-i] = combo & 1 ? '1' : '0';
		combo >>= 1;
	}
	temp[8] = '\0';
	Serial.print(temp);
	Serial.print(" ");
	Serial.print("not done"); // finish artseyio map
	Serial.println();
}

float batteryPercent(float voltage){
	float level;
	if (voltage > 3.975f)
		level = (voltage - 2.920f) * 0.8f;
	else if (voltage > 3.678f)
		level = (voltage - 3.300f) * 1.25f;
	else if (voltage > 3.489f)
		level = (voltage - 3.400f) * 1.7f;
	else if (voltage > 3.360f)
		level = (voltage - 3.300f) * 0.8f;
	else
		level = (voltage - 3.200f) * 0.3f;

	level = (level - 0.05f) / 0.95f; // Cut off the last 5% (3.36V)

	if (level > 1)
		level = 1;
	else if (level < 0)
		level = 0;
	return level;
}

const int mapSize = sizeof(ArtseyMap) / sizeof(ArtseyMap[0]);
extern uint8_t getArtseyIOInput(uint8_t combo){
    for(int i = 0; i < mapSize; i++){
        uint8_t temp = ArtseyMap[i][1];
        if(ArtseyMap[i][0] == combo){
            return ArtseyMap[i][1];
        }
    }
    return '\0';
}


void setup() {
	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);

	Serial.begin(115200);
	Serial.println("Starting BLE work!");
	bleKeyboard.setName("Hz-4 Keyboard");
	bleKeyboard.begin();
	//xTaskCreate(bluetoothTask, "bluetooth", 20000, NULL, 5, NULL);
	for(int i = 0; i < 8; i++) {
		buttonArray[i].initialize();
	}
	Serial.println("Everything good here");
	printCombo(129);
	digitalWrite(2, LOW);
}

bool lastPress = false;
uint8_t lastCombo = 0;
uint8_t currentCombo = 0;
void loop() {
  /*for(int i = 0; i < 8; i++){
    buttonArray[i].read();
  }*/
  //if(isBleConnected){
	if(bleKeyboard.isConnected()){

		controller.update();
		currentCombo = controller.getCombo();
		if(currentCombo != lastCombo){
			Serial.print("Simulate release ");
			Serial.println((char)getArtseyIOInput(lastCombo));
			bleKeyboard.release(getArtseyIOInput(lastCombo));
			//release();
			if(currentCombo != 0){
				Serial.print("Simulate press ");
				Serial.println((char)getArtseyIOInput(currentCombo));
				bleKeyboard.press(getArtseyIOInput(currentCombo));
				//press(getArtseyIOInput(currentCombo));

				float temp = analogRead(BATTERY_PIN);
				Serial.print("Analog reading: ");
				Serial.println(temp);
			}
			lastCombo = currentCombo;
		}
		
		for(int i = 0; i < 8; i++){
			if(buttonArray[i].read()){
			controller.changeInput(i, buttonArray[i].isPressed());
			printCombo(controller.getCombo());
			}
		} 

		if(millis() > nextBatteryRefresh){
			nextBatteryRefresh = millis() + BATTERY_STATE_REFRESH;
			//hid->setBatteryLevel(batteryPercent(analogRead(BATTERY_PIN / adcMultiplier)));
			bleKeyboard.setBatteryLevel(batteryPercent(analogRead(BATTERY_PIN / adcMultiplier)));
		}
	}
}


