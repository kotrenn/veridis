#ifndef PARTICLE__H
#define PARTICLE__H

#include "level.h"
#include "surface.h"
#include "timer.h"

class Level;

class Particle
{
public:
    Particle(Level *level, Uint32 color, const Circle &pos,
	     const vector2d &v, double time);
    ~Particle();

    void pause();
    void unpause();
    
    void update();
    void move(double dt);
    void draw(Camera *camera) const;

    bool dead() const;
private:
    Level *m_level;
    Uint32 m_color;
    Circle m_pos;
    vector2d m_v;
    Timer m_timer;
};

#endif
