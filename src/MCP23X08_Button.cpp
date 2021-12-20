#include "MCP23X08_Button.h"

uint8_t MCP23X08_Button::m_gbl_id = 0; // The definition for the static global ID counter
std::map<uint8_t, MCP23X08_Button*> MCP23X08_Button::allButtons = {{}};

/*----------------------------------------------------------------------*
  / initialize a MCP_Button object and the pin it's connected to.             *
  /-----------------------------------------------------------------------*/
void MCP23X08_Button::begin()
{
  mcp_register.pinMode(m_pin, m_puEnable ? INPUT_PULLUP : INPUT);
  m_state = mcp_register.digitalRead(m_pin);
  if (m_invert)
    m_state = !m_state;
  m_time = millis();
  m_lastState = m_state;
  m_changed = false;
  m_lastChange = m_time;
}

/*----------------------------------------------------------------------*
  / returns the state of the button, true if pressed, false if released.  *
  / does debouncing, captures and maintains times, previous state, etc.   *
  /-----------------------------------------------------------------------*/
bool MCP23X08_Button::read()
{
  uint32_t ms = millis();
  bool pinVal = mcp_register.digitalRead(m_pin);
  if (m_invert)
    pinVal = !pinVal;
  if (ms - m_lastChange < m_dbTime)
    m_changed = false;
  else
  {
    m_lastState = m_state;
    m_state = pinVal;
    m_changed = (m_state != m_lastState);
    if (m_changed)
      m_lastChange = ms;
  }
  m_time = ms;
  return m_state;
}

//TODO this solution is a bit janky and not very felixible
void MCP23X08_Button::readAll(Adafruit_MCP23X08& mcp, Adafruit_MCP23X08& mcp2)
{
  /* A 32 bit variable holding each button state of both shift register
     if the bit is 1 and m_invert = true (the default) means the button
     is not pressed if it is 0 the button is pressed
     if m_invert = false its the other way around
     both register values are bitwise connected
     if no button is pressed the decimal value should be (2^32)-1
  */
  uint32_t pin_values = mcp2.readGPIOAB() << 16 | mcp.readGPIOAB();
  uint32_t bitmask = 1; // think in binary 0000...1 only the lsb is 1
  
  std::map<uint8_t, MCP23X08_Button*> allButtons = getAllButtons();

  for (const auto& b : allButtons)
  {
    MCP23X08_Button* btn = b.second; // get the Button obj in the map
    uint32_t ms = millis();
    /* This will always yield 0 or 1 because only 1 bit is ever set.
     * ie. 1111 & 0001 = 0001 which can be interpreted as TRUE and 
     * hence the button is NOT pressed but when we have
     * 1110 & 0001 = 0000 we get 0 which can be interpreted as FALSE 
     * hence the button is pressed (under default conditions)
     */
    bool pinVal = pin_values & bitmask;
    bitmask = bitmask << 1; // move bitmask to next place to test next button
    if (btn -> m_invert)
      pinVal = !pinVal; // if we invert i.e pull down resistor
    if (ms - btn -> m_lastChange < btn -> m_dbTime)
      btn -> m_changed = false;
    else
    {
      btn -> m_lastState = btn -> m_state;
      btn -> m_state = pinVal;
      btn -> m_changed = (btn -> m_state != btn -> m_lastState);
      if (btn -> m_changed)
        btn -> m_lastChange = ms;
    }
    btn -> m_time = ms;
  }
}

std::map<uint8_t, MCP23X08_Button*>& MCP23X08_Button::getAllButtons()
{
  return allButtons;
}