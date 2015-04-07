#ifndef CONSOLE__H
#define CONSOLE__H

#include "surface.h"

class Console
{
public:
    Console(int max_lines);
    ~Console();

    void key_pressed(SDLKey key, SDLMod mod);
    void new_line();
    void add_line(const char *str);

    void draw(Surface *dst) const;
    
    bool is_visible() const { return m_visible; }
    string get_input() const { return m_input; }
    void set_visible(bool visible) { m_visible = visible; }
private:
    unsigned int m_max_lines;
    list<string> m_lines;
    string m_input;
    bool m_visible;
    TTF_Font *m_font;
    int m_font_size;
};

#endif
