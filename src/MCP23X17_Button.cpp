#include "MCP23X17_Button.h"

uint8_t MCP23X17_Button::m_gbl_id = 0; // The definition for the static global ID counter
std::map<uint8_t, MCP23X17_Button*> MCP23X17_Button::allButtons = {{}};

/*----------------------------------------------------------------------*
  / initialize a MCP_Button object and the pin it's connected to.             *
  /-----------------------------------------------------------------------*/
void MCP23X17_Button::begin()
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
bool MCP23X17_Button::read()
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

void MCP23X17_Button::readAll(LinkedList<Adafruit_MCP23X17> *list)
{
  /*TODO implement a uint128_t to have up to 8 registers which would
   * be the same as the maximum posible i2c addresses for the registers
   * 2^3 = 8. While SPI doesn't has this limit I won't bother with this
   */
  uint64_t pin_values = 0;
  uint64_t bitmask = 1; // think in binary 0000...1 only the lsb is 1
  
  if (list -> size() < 5) // Only execute function if we have 4 or less shift registers
  {
    for (const auto& mcp : *list) // derefence list
    {
      /* inportant to note this reverses the order of the shift registers. i.e the first shift
       * register in the list occupies the last 16 bits of pin_values i.e bit 15 to 0
       * while the second shift register in the list will occupy bits 31 to 16 and so on.
       */
      pin_values = pin_values << 16 | mcp.readGPIOAB();
    }
    
    std::map<uint8_t, MCP23X17_Button*> allButtons = getAllButtons();

    for (const auto& b : allButtons)
    {
      MCP23X17_Button* btn = b.second; // get the Button obj in the map
      uint32_t ms = millis();

      /* This will always yield 0 or 1 because only 1 bit is ever set.
       * ie. 1111 & 0001 = 0001 which can be interpreted as TRUE and 
       * hence the button is NOT pressed but when we have
       * 1110 & 0001 = 0000 we get 0 which can be interpreted as FALSE 
       * hence the button is pressed (under default conditions)
       */
      /* NOTE IMPORTANT THIS WILL ONLY WORK, WHEN 1st pin in the 1st shift register war also the
       * first instance of an MCP23X17_Button created. The second pin the second object, and so on
       * following that the first pin of the second shift register was the 17th object created.
       * i.e created all objects in order
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
}

std::map<uint8_t, MCP23X17_Button*>& MCP23X17_Button::getAllButtons()
{
  return allButtons;
}