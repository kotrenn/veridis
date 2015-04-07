#ifndef ACTOR_SELECTOR__H
#define ACTOR_SELECTOR__H

#include "actor.h"

class ActorSelector
{
public:
    ActorSelector(int font_size, vector2d pos, vector2d size);
    ~ActorSelector();
    
    void mouse_event(SDL_MouseButtonEvent event);

    void draw(Surface *dst) const;

    string get_selection() const;
private:
    TTF_Font *m_font;
    int m_font_size;
    vector2d m_pos;
    vector2d m_size;
    vector<string> m_actors;
    int m_cur_actor;
    int m_first_actor;
};

#endif
