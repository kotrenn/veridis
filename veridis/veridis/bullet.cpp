#include "bullet.h"
#include "actor.h"

Bullet::Bullet(Actor *parent, const vector2d &dir,
	       double v, Uint32 color)
    :m_level(parent->get_level()),
     m_parent(parent),
     m_line(),
     m_v(max(v, EPSILON)),
     m_color(color),
     m_team(parent->get_team())
{
    vector2d center = m_parent->get_center();
    m_line = Line(center[0], center[1],
		  center[0] + dir[0],
		  center[1] + dir[1]);
    m_level->add_bullet(this);
}

Bullet::~Bullet()
{
}

void Bullet::update()
{
    list<Actor *> actors;
    m_level->get_collisions(&actors, this);
    list<Actor *>::iterator a;
    for (a = actors.begin(); a != actors.end(); a++)
	(*a)->damage();
    if (actors.size() > 0)
	m_level->remove_bullet(this);
}

void Bullet::move(double dt)
{
    vector2d dir(m_line.get_x2() - m_line.get_x1(),
		 m_line.get_y2() - m_line.get_y1());
    dir.normalize();
    dir *= m_v;
    m_line[0] += dir[0] * dt;
    m_line[1] += dir[1] * dt;
    m_line[2] += dir[0] * dt;
    m_line[3] += dir[1] * dt;
}

void Bullet::draw(Camera *camera) const
{
    Surface *dst = camera->get_surface();
    int x1 = m_line[0] - camera->get_x();
    int y1 = m_line[1] - camera->get_y();
    int x2 = m_line[2] - camera->get_x();
    int y2 = m_line[3] - camera->get_y();
    int min_x = min(x1, x2);
    int min_y = min(y1, y2);
    int max_x = max(x1, x2);
    int max_y = max(y1, y2);
    if (max_x < 0 || max_y < 0 ||
	min_x > camera->get_w() ||
	min_y > camera->get_h())
	return;
    dst->draw_line(m_color, x1, y1, x2, y2, 1);
}

bool Bullet::collides(Actor *actor) const
{
    Circle circle = actor->get_circle();
    return circle.collides(&m_line);
}

void Bullet::actor_dying(Actor *actor)
{
    if (actor == m_parent)
	m_parent = NULL;
}
