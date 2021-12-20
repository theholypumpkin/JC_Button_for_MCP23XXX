// Arduino MCP_Button Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "MCP23X08_Button.h"
#include "MCP23X08_ToggleButton.h"

bool MCP23X08_ToggleButton::read()
{
    MCP23X08_Button::read();
    if (wasPressed())
    {
        m_toggleState = !m_toggleState;
        m_changed = true;
    }
    else
    {
        m_changed = false;
    }
    return m_toggleState;
}
// has the state changed?
bool MCP23X08_ToggleButton::changed()
{
    return m_changed;
}

// return the current state
bool MCP23X08_ToggleButton::toggleState()
{
    return m_toggleState;
}
