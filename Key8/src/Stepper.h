#pragma once

class Stepper
{
public:
    Stepper();

    void Reset();
    void Advance(int length);
    int GetStep() const;

private:
    int m_Step;
};
