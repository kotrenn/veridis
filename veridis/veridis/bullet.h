#ifndef BULLET_H
#define BULLET_H

#include "actor.h"
#include "camera.h"
#include "level.h"
#include "surface.h"

class Actor;
class Level;

class Bullet
{
public:
    Bullet(Actor *actor, const vector2d &dir,
	   double v, Uint32 color = mapRGB(255, 255, 255));
    ~Bullet();

    void update();
    void move(double dt);
    void draw(Camera *camera) const;

    Actor *get_parent() const { return m_parent; }
    int get_team() const { return m_team; }
    bool collides(Actor *actor) const;
    const Line get_line() const { return m_line; }
    void actor_dying(Actor *actor);
private:
    Level *m_level;
    Actor *m_parent;
    Line m_line;
    double m_v;
    Uint32 m_color;
    int m_team;
};

#endif
