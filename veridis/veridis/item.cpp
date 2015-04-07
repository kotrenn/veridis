#include "item.h"

#include "datamanager.h"

Item::Item(Level *level, Actor *src)
    :m_level(level),
     m_circle(src->get_center()[0],
	      src->get_center()[1], 10),
     m_v(0, 0),
     m_type(randint(1, 3))
{
    m_level->add_item(this);
}

Item::~Item()
{
}

void Item::update()
{
    list<Actor *> actors;
    m_level->get_collisions(&actors, this);
    list<Actor *>::iterator a;
    for (a = actors.begin(); a != actors.end(); a++)
    {
	if ((*a)->get_team() == TEAM_PLAYER)
	{
	    (*a)->upgrade(m_type);
	    m_level->remove_item(this);
	}
    }
}

void Item::move(double dt)
{
    m_circle[0] += m_v[0] * dt;
    m_circle[1] += m_v[1] * dt;
}

void Item::draw(Camera *camera) const
{
    Surface *dst = camera->get_surface();
    Uint32 color;
    switch(m_type)
    {
    case 1: color = mapRGB(255, 0, 0); break;
    case 2: color = mapRGB(0, 255, 0); break;
    case 3: color = mapRGB(0, 0, 255); break;
    }
    dst->draw_circle(color,
		     m_circle[0] - camera->get_x(),
		     m_circle[1] - camera->get_y(),
		     m_circle[2]);
}

bool Item::collides(Actor *actor) const
{
    Circle circle = actor->get_circle();
    return circle.collides(&m_circle);
}

const vector2d Item::get_center() const
{
    vector2d ret(m_circle[0], m_circle[1]);
    return ret;
}

void Item::set_center(const vector2d &center)
{
    m_circle[0] = center[0];
    m_circle[1] = center[1];
}
