#ifndef JC_MCP23X17_BUTTON_H_INCLUDED
#define JC_MCP23X17_BUTTON_H_INCLUDED

#include <Arduino.h>
#include <MCP23XXX_Button.h>
#include <Adafruit_MCP23X17.h>
#include <LinkedList.h>
#include <ArxContainer.h>

class MCP23X17_Button {
  public:
    // MCP_Button(pin, dbTime, puEnable, invert) instantiates a button object.
    //
    // Required parameter:
    // pin      The Arduino pin the button is connected to
    //
    // Optional parameters:
    // dbTime   Debounce time in milliseconds (default 25ms)
    // puEnable true to enable the AVR internal pullup resistor (default true)
    // invert   true to interpret a low logic level as pressed (default true)
    MCP23X17_Button(Adafruit_MCP23X17& mcp,
                    uint8_t mcp_pin,
                    uint32_t mcp_dbTime = 25,
                    uint8_t mcp_puEnable = true,
                    uint8_t mcp_invert = true
                   )
    MCP23XXX_Button(mcp_pin, mcp_dbTime, mcp_puEnable, mcp_invert),
    : mcp_register(mcp)
    {
      m_id = m_gbl_id++;
      allButtons[m_id] = this;
    }

    // Initialize a MCP_Button object and the pin it's connected to
    void begin();

    // Returns the current debounced button state, true for pressed,
    // false for released. Call this function frequently to ensure
    // the sketch is responsive to user input.
    bool read();

    //TODO this method ist jank and will be improved in the future
    static void readAll(LinkedList<Adafruit_MCP23X17> *list);

    static std::map<uint8_t, MCP23X17_Button*>& getAllButtons();

  private:
    Adafruit_MCP23X17& mcp_register;

    //TODO both static parameters map should take MCP23XXX_Buttons as Objects and be implemented in the MCP23XXX_Button class not here
    static std::map<uint8_t, MCP23X17_Button*> allButtons;
    static uint8_t m_gbl_id; // a golbal id for each object created

    uint8_t m_id;          // the id of the object is automatically assigned by the constructor
    
};
#endif
