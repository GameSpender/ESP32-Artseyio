#include <Arduino.h>
#define INPUT_DEBOUNCE 50

#define BUTTON_STATE_DEBUG  //comment out to stop Serial.prints

class ButtonState {
private:
    bool currentState, lastState;
    int inputDebounce;
public:
    const int pin;
    ButtonState(int Pin) : pin(Pin), currentState(true), lastState(true){}
    bool initialize(){
        pinMode(pin, INPUT_PULLUP);
        return true;
    }

    bool read(){
        bool reading = digitalRead(pin);
        if(reading != lastState){
            inputDebounce = millis() + INPUT_DEBOUNCE;
        }
        bool changed = false;
        if(millis() > inputDebounce){
            if(reading != currentState){
                currentState = reading;
                changed = true;
                #ifdef BUTTON_STATE_DEBUG
                if(reading == 0){    
                    Serial.print("B:");
                    Serial.print(pin);
                    Serial.print(" pressed ");
                    Serial.println(millis());

                } else{
                    Serial.print("B:");
                    Serial.print(pin);
                    Serial.print(" released ");
                    Serial.println(millis());                                 
                }
                #endif                
            }
        }
        lastState = reading;
        return changed;
    }
    bool isPressed(){
        return !currentState;
    }
};