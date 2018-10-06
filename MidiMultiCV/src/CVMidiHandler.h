#ifndef CVMIDIHANDLER_H_INCLUDE
#define CVMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"
#include "VoltageOut.h"

class CVMidiHandler : public MidiHandler
{
public:
  CVMidiHandler(VoltageOut& cvPitch, VoltageOut& cvVelocity, DigitalOut& gateOut) 
  : m_cvPitch(cvPitch)
  , m_cvVelocity(cvVelocity)
  , m_gateOut(gateOut)
  {}

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
    int cv = MidiNote + 12*m_OctaveOffset;
    m_cvPitch.WriteVoltage(cv/12.0f);
    m_cvVelocity.WriteVoltage(Velocity/40.0f);//[0,128[ to [0.0,3.2[
    m_gateOut = 1;
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_gateOut = 0;
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   // learn
   if(m_Controller==128)
   {
     m_Controller = Controller;
   }
   if(m_Controller == Controller)
   {
      //assume all CC is voltage offset change
      m_OctaveOffset = (Value>>3);//[0,15]
      m_OctaveOffset -= 8;//[-8, +7]
   }
 }

private:
  VoltageOut& m_cvPitch;
  VoltageOut& m_cvVelocity;
  DigitalOut& m_gateOut;
  // octave offset
  uint8_t m_Controller{128};
  int m_OctaveOffset{-4};
};

#endif /* end of include guard: CVMIDIHANDLER_H_INCLUDE */
