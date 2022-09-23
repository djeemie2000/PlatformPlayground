#pragma once
#include <Arduino.h>

class AnalogButtonIn2
{
public:
     AnalogButtonIn2() 
     : m_Button1(false)
     , m_Button2(false)
     {}

     void Begin(uint8_t analogInPin)
     {
         m_AnalogInPin = analogInPin;
     }

     void update()
     {
         m_PrevButton1 = m_Button1;
         m_PrevButton2 = m_Button2;

         // value [0, 8[
         int value = analogRead(m_AnalogInPin) >> 7;
         if(value<1)
         {
             m_Button1 = false;
             m_Button2 = false;
         }
         else if(3<value)
         {
             m_Button1 = true;
             m_Button2 = false;
         }
         else if(5<value)
         {
             m_Button1 = false;
             m_Button2 = true;
         }
         else
         {
             m_Button1 = true;
             m_Button2 = true;             
         }
     }

     bool Get1() const {return m_Button1;}
     bool Get2() const {return m_Button2;}
     bool IsClicked1() const { return m_Button1 && !m_PrevButton1;}
     bool IsClicked2() const { return m_Button2 && !m_PrevButton2;}

private:
     uint8_t m_AnalogInPin;
     bool m_Button1;
     bool m_PrevButton1;
     bool m_Button2;
     bool m_PrevButton2;
};
