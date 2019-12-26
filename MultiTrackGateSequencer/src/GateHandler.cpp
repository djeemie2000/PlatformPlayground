#include "GateHandler.h"

DigitalOutGateHandler::DigitalOutGateHandler(DigitalOut& out) 
: m_Out(out)
{}

void DigitalOutGateHandler::On() 
{ 
    m_Out = 1;
}

void DigitalOutGateHandler::Off(){
    m_Out = 0;
}
