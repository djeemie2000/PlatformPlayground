#include <Arduino.h>

class OrderedList
{
public:
    static const int Capacity = 8;//for testing/development

    OrderedList() : m_Size(0), m_Begin(0), m_Head(0)
    {}

    int Size() const { return m_Size;   }
    void Clear()
    {
        m_Size = 0;
        m_Begin = 0;
        m_Head = 0;
    }

    bool Add(int key, char value)
    {       
        if(0 == m_Size)
        {
            if(Item* inserted = AllocateItem(key, value))
            {
                m_Begin = inserted;
                m_Head = inserted;
                return true;
            }
        }
        else if(m_Head == 0 && key<m_Begin->Key)
        {
            // insert into begin == head
            if(Item* inserted = AllocateItem(key, value))
            {
                inserted->Next = m_Begin;
                m_Head = inserted;
                m_Begin = inserted;
                return true;
            }
        }
        else 
        {
            // TODO check inserted wrt head -> can insert at head?
            if(m_Head->Key<=key && (0 == m_Head->Next || key<=m_Head->Next->Key))
            {
                if(Item* inserted = AllocateItem(key, value))
                {
                    inserted->Next = m_Head->Next;
                    m_Head->Next = inserted;
                    m_Head = inserted;
                    return true;
                }
            }
            // if not, move head untill can insert at head???
        }

        return false;
    }

    bool HeadAt(int key) const 
    {
        return m_Head && m_Head->Key == key;
    }

    int HeadKey() const
    {
        return m_Head ? m_Head->Key : -1;
    }

    char HeadValue() const
    {
        // TODO default value for Value type
        return m_Head ? m_Head->Value : '0';
    }

    bool ResetHead()
    {
        m_Head = 0;
        return true;
    }

    bool MoveHeadToNext()
    {
        if(m_Head && m_Head->Next)
        {
            m_Head = m_Head->Next;
            return true;
        }
        if(m_Head == 0 && m_Begin != 0)
        {
            m_Head = m_Begin;
            return true;
        }
        return false;
    }

    // move head to next if next key equals key
    bool MoveHeadToNextIfNextKeyEquals(int key)
    {
        if(m_Head && m_Head->Next && m_Head->Next->Key == key)
        {
            m_Head = m_Head->Next;
            return true;
        }
        if(m_Head == 0 && m_Begin && m_Begin->Key == key)
        {
            m_Head = m_Begin;
            return true;
        }
        return false;
    }

private:
    struct Item
    {
        char Value;//TODO replace by other value type
        int Key;
        //Item* Prev;//???
        Item* Next;
    };


    Item* AllocateItem(int key, char value)
    {
        if(m_Size < Capacity)
        {
             // 'allocate' item and 'constuct' item ~ key,value
            Item* inserted = m_Items+m_Size;
            ++m_Size;
            inserted->Next = 0;
            inserted->Key = key;
            inserted->Value = value;
            return inserted;
        }

        return 0;
    }

    Item m_Items[Capacity];
    int m_Size;
    Item* m_Begin;
    
    Item* m_Head;
};
