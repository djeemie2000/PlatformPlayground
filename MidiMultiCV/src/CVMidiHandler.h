#ifndef CVMIDIHANDLER_H_INCLUDE
#define CVMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"
#include "VoltageOut.h"

class CVMidiHandler : public MidiHandler
{
public:
  CVMidiHandler(VoltageOut& cvPitch, DigitalOut& gateOut) : m_cvPitch(cvPitch), m_gateOut(gateOut)
  {}

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
    int cv = MidiNote + 12*m_OctaveOffset;
    m_cvPitch.write(cv/12.0f);
    m_gateOut = 1;
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_gateOut = 0;
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   //assume all CC is voltage offset change
   m_OctaveOffset = (Value>>3);//[0,15]
   m_OctaveOffset -= 8;
 }

private:
  VoltageOut& m_cvPitch;
  DigitalOut& m_gateOut;
  //TODO voltage out for velocity
  int m_OctaveOffset{0};
};

#endif /* end of include guard: CVMIDIHANDLER_H_INCLUDE */
