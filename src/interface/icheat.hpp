#pragma once
#include "ifeature.hpp"

template<typename T>
class ICheat : public IFeature<T>
{
protected:
    bool m_bEnabled = false;     

public:

    // Deactivates the cheat
    virtual void Disable()
    {
        m_bEnabled = false;
    }

    // Activates the cheat
    virtual void Enable()
    {
        m_bEnabled = true;
    }

    // Returns the current stae of the cheat
    virtual const bool GetState() final
    {
        return m_bEnabled;
    }

    // Sets the cheat state
    virtual void SetState(const bool state) final
    {
        if (m_bEnabled != state)
        {
            Toggle();
        }
    }

    // Toggles the cheat & returns it's toggled state 
    virtual bool Toggle() final
    {
        m_bEnabled = !m_bEnabled;
        if (m_bEnabled)
        {
            Enable();
        }
        else
        {
            Disable();
        }
        
        return m_bEnabled;
    }
};