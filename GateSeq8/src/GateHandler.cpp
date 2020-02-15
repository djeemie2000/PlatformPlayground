#include "GateHandler.h"

DigitalOutGateHandler::DigitalOutGateHandler(uint8_t outPin) 
: m_OutPin(outPin)
{}

void DigitalOutGateHandler::On() 
{ 
    digitalWrite(m_OutPin, HIGH);
}

void DigitalOutGateHandler::Off(){
    digitalWrite(m_OutPin, LOW);
}
