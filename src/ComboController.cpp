#include <Arduino.h>

#define INPUT_DECAY 50
#define INPUT_NUMBER_MAX 30

// #define COMBO_CONTROLLER_DEBUG



class ComboController{
private:
    const int inputNum;
    int inputDecay;
    bool readyNextInput;
    unsigned long combo;
    bool inputs[INPUT_NUMBER_MAX];

    void setCurrentCombo(){
        #ifdef COMBO_CONTROLLER_DEBUG
            Serial.print("Combo: ");
        #endif
        combo += inputs[0];
        for(int i = 1; i < inputNum; i++){
            combo = combo << 1;
            combo += inputs[i];

            #ifdef COMBO_CONTROLLER_DEBUG
                Serial.print(inputs[i]);
            #endif                
        }
        #ifdef COMBO_CONTROLLER_DEBUG
            Serial.println();
        #endif
    }
    void setCurrentCombo(unsigned long selectedCombo){
        combo = selectedCombo;
    }
public:
    ComboController(int inputNum) : inputNum(inputNum > 0 ? inputNum : 1){}

    const bool allReleased(){
        bool temp = true;
        for(int i = 0; i < inputNum; i++){
            if(inputs[i] != 0) 
                temp = false;
        }
        return temp;
    }

    void update(){
        if(millis() > inputDecay){
            setCurrentCombo(0);
            if(allReleased()){
                readyNextInput = true;
            }
        }
    }
    void changeInput(int input, bool value){
        if(input < 0 || input >= inputNum){
            return;
        }
        /*if(millis() > inputTimeframe){
            inputTimeframe = millis() + INPUT_COMBO_TIMEFRAME;
        }*/
        /*if(inputs[input] == 1 && value == 0){
            if(millis() > inputTimeframe){
                setCurrentCombo();
                inputTimeframe = millis() + INPUT_COMBO_TIMEFRAME;
            }
        }*/
        if(inputs[input] == 1 && value == 0){
            if(readyNextInput){
                setCurrentCombo();
                readyNextInput = false;  
                inputDecay = millis() + INPUT_DECAY;
            }
        }
        inputs[input] = value;
    }

    unsigned long getCombo(){
        return combo;
    }
};