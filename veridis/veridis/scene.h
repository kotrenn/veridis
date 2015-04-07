#ifndef SCENE__H
#define SCENE__H

#include "common.h"
#include "surface.h"

class Scene
{
public:
    Scene();
    ~Scene();
    
    void update();
    void draw(Surface *dst) const;

    int get_x() const { return m_x; }
    int get_y() const { return m_y; }
    int get_w() const { return m_w; }
    int get_h() const { return m_h; }
    int get_view_w() const { return m_view_w; }
    int get_view_h() const { return m_view_h; }
    const vector2d get_pos() const;
    const vector2d get_size() const;
    void set_x(int x) { m_x = x; }
    void set_y(int y) { m_y = y; }
    void set_pos(const vector2d &pos);
private:
    list<Surface *> m_backgrounds;
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    int m_view_w;
    int m_view_h;
};

#endif
