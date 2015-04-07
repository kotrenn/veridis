#ifndef ITEM__H
#define ITEM__H

#include "actor.h"
#include "level.h"
#include "surface.h"

class Actor;
class Level;

class Item
{
public:
    Item(Level *level, Actor *src);
    ~Item();

    void update();
    void move(double dt);
    void draw(Camera *camera) const;

    bool collides(Actor *actor) const;

    const vector2d get_center() const;
    const Circle get_circle() const { return m_circle; }
    const vector2d get_v() const { return m_v; }
    int get_type() const { return m_type; }
    void set_center(const vector2d &center);
    void set_v(const vector2d &v) { m_v = v; }
private:
    Level *m_level;
    Circle m_circle;
    vector2d m_v;
    int m_type;
};

#endif
