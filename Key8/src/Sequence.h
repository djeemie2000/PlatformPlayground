#pragma once

class  Sequence
{
public:
    static const int Capacity = 128;//?? other value ??

    Sequence();

    void Clear();
    void Append(int note, int gate);

    int Length() const;
    int Note(int step) const;
    int Gate(int step) const;

private:
    int m_Note[Capacity];
    int m_Gate[Capacity];
    int m_Length;
};
