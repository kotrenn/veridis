#ifndef EDITOR_MANAGER__H
#define EDITOR_MANAGER__H

#define EM_TILE 0
#define EM_ACTOR 1

#include "manager.h"
#include "actorselector.h"

class EditorManager : public DecoratorManager
{
public:
    EditorManager(AbstractManager *inner);
    ~EditorManager();

    void key_pressed(SDLKey key, SDLMod mod);
    void mouse_event(SDL_MouseButtonEvent event);

    void update();
    void draw(Surface *dst) const;
private:
    bool m_editing;
    int m_state;
    Surface *m_sheet;
    int m_camera_w;
    int m_camera_h;
    int m_tile_x;
    int m_tile_y;
    ActorSelector *m_selector;
    TTF_Font *m_font;
    int m_font_size;
};

#endif
