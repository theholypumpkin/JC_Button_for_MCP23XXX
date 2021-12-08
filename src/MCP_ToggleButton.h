// Arduino ToogleButton Library
// https://github.com/JChristensen/JC_Button
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef JC_MCP_TOGGLEBUTTON_H_INCLUDED
#define JC_MCP_TOGGLEBUTTON_H_INCLUDED

#include <Arduino.h>
#include "MCP_Button.h"
#include "MCP_ToggleButton.h"

// a derived class for a "push-on, push-off" (toggle) type button.
// initial state can be given, default is off (false).
class MCP_ToggleButton : public MCP_Button
{
protected:
    // constructor is similar to MCP_Button, but includes the initial state for the toggle.
    MCP_ToggleButton(uint8_t pin, bool initialState = false, uint32_t dbTime = 25, uint8_t puEnable = true, uint8_t invert = true)
        : MCP_Button(pin, dbTime, puEnable, invert), m_toggleState(initialState) {}

    // read the button and return its state.
    // should be called frequently.
    bool read();

    // has the state changed?
    bool changed();

    // return the current state
    bool toggleState();

private:
    bool m_toggleState;
    bool m_changed;
};