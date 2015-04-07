#ifndef CONSOLE_MANAGER__H
#define CONSOLE_MANAGER__H

#include "manager.h"

class ConsoleManager : public DecoratorManager
{
public:
    ConsoleManager(AbstractManager *inner);
    ~ConsoleManager();

    void key_pressed(SDLKey key, SDLMod mod);

    void draw(Surface *dst) const;
private:
    void parse_input(string input);
    
    Console *m_console;
};

#endif
