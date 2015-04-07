#include "actor.h"
#include "bullet.h"
#include "datamanager.h"
#include "particle.h"
#include "resourcemanager.h"

Actor::Actor(Level *level, const string &name, const string &surface)
	:m_level(level),
	 m_name(name),
	 m_surface(NULL),
	 m_ticks(SDL_GetTicks()),
	 m_pause(0),
	 m_circle(),
	 m_v(0, 0),
	 m_removable(true),
	 m_team(TEAM_ENEMY),
	 m_flying(false),
	 m_target(NULL),
	 m_health(10),
	 m_dead(false),
	 m_dir(SOUTH)
{
	m_level->add_actor(this);
	m_circle = Circle(320, 240, 20);
	if (surface.size() > 0)
	{
		ResourceManager *rm = ResourceManager::get_instance();
		m_surface = rm->get_image(surface.c_str());
		m_circle[2] = min(m_surface->get_w() / 8, m_surface->get_h());
		m_circle[2] /= 2;
	}
}

Actor::Actor(const string &name, const string &surface)
	:m_level(NULL),
	 m_name(name),
	 m_surface(NULL),
	 m_ticks(SDL_GetTicks()),
	 m_pause(0),
	 m_circle(),
	 m_v(0, 0),
	 m_removable(true),
	 m_team(TEAM_ENEMY),
	 m_flying(false),
	 m_target(NULL),
	 m_health(10),
	 m_dead(false),
	 m_dir(SOUTH)
{
	if (surface.size() > 0)
	{
		ResourceManager *rm = ResourceManager::get_instance();
		m_surface = rm->get_image(surface.c_str());
		m_circle[2] = min(m_surface->get_w() / 8, m_surface->get_h());
		m_circle[2] /= 2;
	}
}
    

Actor::~Actor()
{
}

void Actor::clone(Level *level, int x, int y, Actor *actor) const
{
	actor->m_level = level;
	actor->m_circle = Circle(x, y, m_circle[2]);

	level->add_actor(actor);
}

void Actor::pause()
{
	m_pause = SDL_GetTicks() - m_ticks;
}

void Actor::unpause()
{
	m_ticks = SDL_GetTicks() - m_pause;
	m_pause = 0;
}

Uint32 Actor::get_ticks() const
{
	Uint32 ret = SDL_GetTicks();
	if (m_pause > 0)
		ret -= m_pause;
	else
		ret -= m_ticks;
	return ret;
}

void Actor::update()
{
	if (m_target == NULL)
		select_target();
	else
	{
		DataManager *dm = DataManager::get_instance();
		int max_dist = dm->get_int("view_dist");
		vector2d center = m_target->get_center();
		int dx = m_circle[0] - center[0];
		int dy = m_circle[1] - center[1];
		if (dx * dx + dy * dy > max_dist * max_dist)
			m_target = NULL;
	}
}

void Actor::move(double dt)
{
	if (m_v[0] < 0)
		m_dir = WEST;
	else if (m_v[0] > 0)
		m_dir = EAST;
	if (m_v[1] < 0)
		m_dir = NORTH;
	else if (m_v[1] > 0)
		m_dir = SOUTH;
    
	m_circle[0] += m_v[0] * dt;
	m_circle[1] += m_v[1] * dt;

	if (m_team == TEAM_PLAYER)
		return;
	if (m_circle[0] < 0)
		m_circle[0] = 0;
	if (m_circle[1] < 0)
		m_circle[1] = 0;
	if (m_circle[0] > m_level->get_w())
		m_circle[0] = m_level->get_w();
	if (m_circle[1] > m_level->get_h())
		m_circle[1] = m_level->get_h();
}

void Actor::draw(Camera *camera) const
{
	Surface *dst = camera->get_surface();
	int x = m_circle[0] - camera->get_x();
	int y = m_circle[1] - camera->get_y();
	int r = m_circle[2];
	if (x + r < 0 ||
	    y + r < 0 ||
	    x - r > camera->get_w() ||
	    y - r > camera->get_h())
		return;
	if (m_surface == NULL)
		dst->draw_circle(mapRGB(255, 255, 255), x, y, r, 1);
	else
	{
		int base = 0;
		switch(m_dir)
		{
		case NORTH: base = 0; break;
		case SOUTH: base = 1; break;
		case WEST:  base = 2; break;
		case EAST:  base = 3; break;
		}

		int frame = 2 * base + (get_ticks() / 500) % 2;
		int w = m_surface->get_w() / 8;
		int h = m_surface->get_h();
		int surface_x = x - 0.5 * w;
		int surface_y = y - 0.5 * h;
		dst->blit(m_surface, surface_x, surface_y,
		          Rect(frame * w, 0, w, h));
	}
	//dst->draw_circle(mapRGB(255, 255, 255), x, y, r, 1);
}

void Actor::explode() const
{
	DataManager *dm = DataManager::get_instance();

	int num = dm->get_int("num_particles");
	for (int i = 0; i < num; ++i)
	{
		int theta = randint(1, 360);
		double speed = 30 * 0.2 * randint(2, 8);
		vector2d v(speed * cos(theta * M_PI / 180.0),
		           speed * sin(theta * M_PI / 180.0));
		Circle pos = m_circle;
		pos[2] = 0.5 * randint(3, 8);
		double time = 0.05 * randint(10, 40);
		new Particle(m_level, mapRGB(0, 0, 255), pos, v, time);
	}
}

void Actor::damage()
{
	if (m_health > 0)
		m_health--;
	if (m_health <= 0 && !m_dead)
	{
		if (randint(1, 2) == 1 && m_team == TEAM_ENEMY)
			new Item(m_level, this);
		m_level->remove_actor(this);
		explode();
		m_dead = true;
	}
}

const vector2d Actor::get_center() const
{
	vector2d ret(m_circle[0], m_circle[1]);
	return ret;
}

void Actor::set_center(const vector2d &center)
{
	m_circle[0] = center[0];
	m_circle[1] = center[1];
}

void Actor::set_level(Level *level)
{
	m_level = level;
	m_level->add_actor(this);
	m_target = NULL;
}

void Actor::actor_dying(Actor *actor)
{
	if (m_target == actor)
		m_target = NULL;
}

void Actor::select_target()
{
	DataManager *dm = DataManager::get_instance();
	int max_dist = dm->get_int("view_dist");
	m_target = m_level->get_next_target(this, m_target, max_dist);
}



PlayerActor::PlayerActor(Level *level)
	:Actor(level, "player", "player.png"),
	 m_select_timer(0.0),
	 m_fire_rate(0),
	 m_fire_timer(0.0),
	 m_num_bullets(1),
	 m_invincible(0.0)
{
	DataManager *dm = DataManager::get_instance();

	m_removable = false;
	m_team = TEAM_PLAYER;
    
	m_circle[0] = m_level->get_w() / 2;
	m_circle[1] = m_level->get_h() / 2;
	m_circle[2] = dm->get_int("player_rad");

	m_health = dm->get_int("player_health");
	m_fire_rate = dm->get_double("player_rate");
}

PlayerActor::PlayerActor()
	:Actor("player", "player.png"),
	 m_select_timer(0.0),
	 m_fire_rate(0),
	 m_fire_timer(0.0),
	 m_num_bullets(1),
	 m_invincible(0.0)
{
}

PlayerActor::~PlayerActor()
{
}

Actor *PlayerActor::clone(Level *level, int x, int y) const
{
	PlayerActor *actor = new PlayerActor();
	Actor::clone(level, x, y, actor);

	return actor;
}

void PlayerActor::pause()
{
	Actor::pause();

	m_select_timer.pause();
	m_fire_timer.pause();
}

void PlayerActor::unpause()
{
	Actor::unpause();

	m_select_timer.unpause();
	m_fire_timer.unpause();
}

void PlayerActor::update()
{
	Actor::update();

	DataManager *dm = DataManager::get_instance();
    
	m_v[0] = 0;
	m_v[1] = 0;

	Uint8 *keys = SDL_GetKeyState(NULL);
	int speed = dm->get_int("player_speed");
	if (keys[SDLK_UP])
		m_v[1] = -speed;
	if (keys[SDLK_DOWN])
		m_v[1] = speed;
	if (keys[SDLK_LEFT])
		m_v[0] = -speed;
	if (keys[SDLK_RIGHT])
		m_v[0] = speed;

	if (keys[SDLK_z])
	{
		if (m_fire_timer.done())
		{
			m_fire_timer.reset(m_fire_rate);
			create_bullet();
		}
	}

	if (keys[SDLK_x])
	{
		if (m_select_timer.done())
		{
			select_target();
			if (m_target != NULL)
				m_select_timer.reset(0.33);
		}
	}
}

void PlayerActor::draw(Camera *camera) const
{
	if (m_invincible.done() ||
	    (int)(m_invincible.get_time() * 30) % 2 == 0)
		Actor::draw(camera);

	Surface *dst = camera->get_surface();
    
	DataManager *dm = DataManager::get_instance();
	int max_health = dm->get_int("player_health");

	Uint32 black = mapRGB(0, 0, 0);
	Uint32 health = mapRGB(255, 0, 0);
	int health_x = 50;
	int health_y = 20;
	Rect rect(health_x, health_y,
	          camera->get_w() - 2 * health_x, 20);
	dst->draw_rect(black, rect, 2);
	double p = (1.0 * m_health) / max_health;
	rect[2] = p * rect[2] - 4;
	rect[0] += 2;
	rect[1] += 2;
	rect[3] -= 4;
	dst->draw_rect_3d(health, rect);
    
	if (m_target == NULL)
		return;

	Circle circle = m_target->get_circle();
	int x = circle[0] - camera->get_x();
	int y = circle[1] - camera->get_y();
	int r = circle[2] * 1.5;
	Uint32 color = mapRGB(255, 0, 0);
	dst->draw_circle(color, x, y, r, 1);
	dst->draw_line(color, x - r, y, x + r, y);
	dst->draw_line(color, x, y - r, x, y + r);
}

void PlayerActor::create_bullet()
{
	if (m_target == NULL)
		return;

	DataManager *dm = DataManager::get_instance();

	vector2d dirs[] = { vector2d(1, 0), vector2d(-1, 0),
	                    vector2d(0, 1), vector2d(0, -1),
	                    vector2d(1, 1), vector2d(-1, 1),
	                    vector2d(1, -1), vector2d(-1, -1) };
	for (int i = 0; i < 8; ++i)
		dirs[i].normalize();
	vector2d dir = m_target->get_center() - get_center();
	int min_i = 0;
	double min_dot = dir * dirs[0];
	for (int i = 1; i < 8; ++i)
	{
		double dot = dir * dirs[i];
		if (dot < min_dot)
		{
			min_dot = dot;
			min_i = i;
		}
	}
	dir = (dir * dirs[min_i]) * dirs[min_i];
	dir.normalize();
	dir *= dm->get_double("pbullet_len");

	/*
	  vector2d dir = m_target->get_center() - get_center();
	  dir.normalize();
	  dir *= dm->get_double("pbullet_len");
	*/

	for (int i = 1; i < m_num_bullets; ++i)
	{
		double theta = i * 5 * M_PI / 180.0;
		double c1 = cos(theta);
		double s1 = sin(theta);
		double c2 = cos(-theta);
		double s2 = sin(-theta);
		vector2d dir_rot1(c1 * dir[0] - s1 * dir[1],
		                  s1 * dir[0] + c1 * dir[1]);
		vector2d dir_rot2(c2 * dir[0] - s2 * dir[1],
		                  s2 * dir[0] + c2 * dir[1]);
		new Bullet(this, dir_rot1, dm->get_double("pbullet_speed"));
		new Bullet(this, dir_rot2, dm->get_double("pbullet_speed"));
	}
	new Bullet(this, dir, dm->get_double("pbullet_speed"));
}

void PlayerActor::upgrade(int type)
{
	DataManager *dm = DataManager::get_instance();
	int max_health = dm->get_int("player_health");
    
	switch(type)
	{
	case 1: m_health = min(max_health, m_health + 1); break;
	case 2: m_num_bullets = min(5, m_num_bullets + 1); break;
	case 3: m_fire_rate = max(0.1, m_fire_rate / 2); break;
	}
}

void PlayerActor::damage()
{
	if (!m_invincible.done())
		return;
    
	Actor::damage();

	DataManager *dm = DataManager::get_instance();
	double max_fire_rate = dm->get_double("player_rate");

	if (randint(1, 2) == 1 || m_num_bullets == 1)
		m_fire_rate = min(max_fire_rate, 2 * m_fire_rate);
	else
		m_num_bullets = max(1, m_num_bullets - 1);

	m_invincible.reset(1.0);
}



TripleShot::TripleShot(Level *level)
	:Actor(level, "triple_shot", "mushroom.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
	m_health = 5;
}

TripleShot::TripleShot()
	:Actor("triple_shot", "mushroom.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
	m_health = 5;
}

TripleShot::~TripleShot()
{
}

void TripleShot::random_dir()
{
	m_v[0] = m_v[1] = 0;
	m_v[randint(0, 1)] = 30 * randint(1, 3) * (2 * randint(0, 1) - 1);
}

Actor *TripleShot::clone(Level *level, int x, int y) const
{
	TripleShot *actor = new TripleShot();
	Actor::clone(level, x, y, actor);

	return actor;
}

void TripleShot::pause()
{
	Actor::pause();

	m_shoot_timer.pause();
}

void TripleShot::unpause()
{
	Actor::unpause();

	m_shoot_timer.unpause();
}

void TripleShot::update()
{
	Actor::update();

	if (m_shoot_timer.done())
	{
		m_shoot_timer.reset(2.0);
		create_bullet();
	}

	if (randint(1, 100) == 1)
		random_dir();
}

void TripleShot::create_bullet()
{
	if (m_target == NULL)
		return;
    
	DataManager *dm = DataManager::get_instance();
    
	vector2d dir = m_target->get_center() - get_center();
	dir.normalize();
	dir *= 2 * dm->get_double("pbullet_len");

	int theta[] = { -30, 0, 30 };
	for (int i = 0; i < 3; ++i)
	{
		double c = cos(theta[i] * M_PI / 180.0);
		double s = sin(theta[i] * M_PI / 180.0);
		vector2d rot_dir(c * dir[0] - s * dir[1],
		                 s * dir[0] + c * dir[1]);
		double v = 0.5 * dm->get_double("pbullet_speed");
		Uint32 color = mapRGB(255, 0, 0);
		new Bullet(this, rot_dir, v, color);
	}
}



RadialShot::RadialShot(Level *level)
	:Actor(level, "radial_shot", "ninja.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
	m_health = 5;
}

RadialShot::RadialShot()
	:Actor("radial_shot", "ninja.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
	m_health = 5;
}

RadialShot::~RadialShot()
{
}

void RadialShot::random_dir()
{
	m_v[0] = m_v[1] = 0;
	m_v[randint(0, 1)] = 30 * randint(1, 3) * (2 * randint(0, 1) - 1);
}

Actor *RadialShot::clone(Level *level, int x, int y) const
{
	RadialShot *actor = new RadialShot();
	Actor::clone(level, x, y, actor);

	return actor;
}

void RadialShot::pause()
{
	Actor::pause();

	m_shoot_timer.pause();
}

void RadialShot::unpause()
{
	Actor::unpause();

	m_shoot_timer.unpause();
}

void RadialShot::update()
{
	Actor::update();

	if (m_shoot_timer.done())
	{
		m_shoot_timer.reset(2.0);
		create_bullet();
	}

	if (randint(1, 100) == 1)
		random_dir();
}

void RadialShot::create_bullet()
{
	if (m_target == NULL)
		return;
    
	DataManager *dm = DataManager::get_instance();
    
	vector2d dir = m_target->get_center() - get_center();
	dir.normalize();
	dir *= 2 * dm->get_double("pbullet_len");

	int theta[] = { -120, 0, 120 };
	for (int i = 0; i < 3; ++i)
	{
		double c = cos(theta[i] * M_PI / 180.0);
		double s = sin(theta[i] * M_PI / 180.0);
		vector2d rot_dir(c * dir[0] - s * dir[1],
		                 s * dir[0] + c * dir[1]);
		double v = 0.5 * dm->get_double("pbullet_speed");
		Uint32 color = mapRGB(0, 0, 255);
		new Bullet(this, rot_dir, v, color);
	}
}



RapidShot::RapidShot(Level *level)
	:Actor(level, "rapid_shot", "devil.png"),
	 m_shoot_timer(0.0),
	 m_num_shots(10)
{
	random_dir();
	m_flying = true;
}

RapidShot::RapidShot()
	:Actor("rapid_shot", "devil.png"),
	 m_shoot_timer(0.0),
	 m_num_shots(10)
{
	random_dir();
	m_flying = true;
}

RapidShot::~RapidShot()
{
}

void RapidShot::random_dir()
{
	m_v[0] = m_v[1] = 0;
	m_v[randint(0, 1)] = 30 * randint(1, 3) * (2 * randint(0, 1) - 1);
}

Actor *RapidShot::clone(Level *level, int x, int y) const
{
	RapidShot *actor = new RapidShot();
	Actor::clone(level, x, y, actor);

	return actor;
}

void RapidShot::pause()
{
	Actor::pause();

	m_shoot_timer.pause();
}

void RapidShot::unpause()
{
	Actor::unpause();

	m_shoot_timer.unpause();
}

void RapidShot::update()
{
	Actor::update();

	if (m_shoot_timer.done())
	{
		m_shoot_timer.reset(5.0);
		m_num_shots = 10;
		create_bullet();
	}
	else if (m_shoot_timer.get_time() * 5 < m_num_shots)
	{
		m_num_shots--;
		create_bullet();
	}

	if (randint(1, 30) == 1)
		random_dir();
}

void RapidShot::create_bullet()
{
	if (m_target == NULL)
		return;
    
	DataManager *dm = DataManager::get_instance();
    
	vector2d dir = m_target->get_center() - get_center();
	dir.normalize();
	dir *= 1.5 * dm->get_double("pbullet_len");
	double v = 0.5 * dm->get_double("pbullet_speed");
	Uint32 color = mapRGB(255, 255, 255);
	switch(m_num_shots % 3)
	{
	case 0: color = mapRGB(255, 255, 0); break;
	case 1: color = mapRGB(255, 0, 255); break;
	case 2: color = mapRGB(0, 255, 255); break;
	}
	new Bullet(this, dir, v, color);
}



QuadShot::QuadShot(Level *level)
	:Actor(level, "quad_shot", "jellyfish.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
}

QuadShot::QuadShot()
	:Actor("quad_shot", "jellyfish.png"),
	 m_shoot_timer(0.0)
{
	random_dir();
}

QuadShot::~QuadShot()
{
}

void QuadShot::random_dir()
{
	m_v[0] = m_v[1] = 0;
	m_v[randint(0, 1)] = 30 * randint(1, 3) * (2 * randint(0, 1) - 1);
}

Actor *QuadShot::clone(Level *level, int x, int y) const
{
	QuadShot *actor = new QuadShot();
	Actor::clone(level, x, y, actor);

	return actor;
}

void QuadShot::pause()
{
	Actor::pause();

	m_shoot_timer.pause();
}

void QuadShot::unpause()
{
	Actor::unpause();

	m_shoot_timer.unpause();
}

void QuadShot::update()
{
	Actor::update();

	if (m_shoot_timer.done())
	{
		m_shoot_timer.reset(1.5);
		create_bullet();
	}

	if (randint(1, 100) == 1)
		random_dir();
}

void QuadShot::create_bullet()
{
	if (m_target == NULL)
		return;
    
	DataManager *dm = DataManager::get_instance();

	vector2d dirs[] = { vector2d(-1, 0),
	                    vector2d(1, 0),
	                    vector2d(0, -1),
	                    vector2d(0, 1) };
	for (int i = 0; i < 4; ++i)
	{
		vector2d dir = dirs[i] * 2 * dm->get_double("pbullet_len");
		double v = dm->get_double("pbullet_speed");
		Uint32 color = mapRGB(0, 255, 0);
		new Bullet(this, dir, v, color);
	}
}



EnemyCrab::EnemyCrab(Level *level)
	:Actor(level, "enemycrab", "enemycrab.png"),
	 m_shoot_timer(4.0),
	 m_move_timer(0.0),
	 m_pos(0),
	 m_color(0),
	 m_num_shots(8)
{
	random_color();
}

EnemyCrab::EnemyCrab()
	:Actor("enemy_crab", "enemycrab.png"),
	 m_shoot_timer(4.0),
	 m_move_timer(0.0),
	 m_pos(0),
	 m_color(0),
	 m_num_shots(8)
{
	random_color();
}

EnemyCrab::~EnemyCrab()
{
}

Actor *EnemyCrab::clone(Level *level, int x, int y) const
{
	EnemyCrab *actor = new EnemyCrab();
	Actor::clone(level, x, y, actor);

	actor->m_health = 250;

	return actor;
}

void EnemyCrab::pause()
{
	Actor::pause();

	m_shoot_timer.pause();
}

void EnemyCrab::unpause()
{
	Actor::unpause();

	m_shoot_timer.unpause();
}

void EnemyCrab::update()
{
	Actor::update();

	if (m_shoot_timer.done())
	{
		m_shoot_timer.reset(6.0);
		create_bullet();
		m_num_shots = 8;
		random_color();
	}
	else if (m_shoot_timer.get_time() * 3 < m_num_shots)
	{
		m_num_shots--;
		create_bullet();
	}

	if (m_move_timer.done())
	{
		m_v[0] = 0;
		if (randint(1, 100) == 1)
		{
			int pos = m_pos;
			while (abs(pos - m_pos) != 1)
				pos = randint(-1, 2);
			m_v[0] = 16.0 * (pos - m_pos);
			m_pos = pos;
			m_move_timer.reset(2.0);
		}
	}
}

void EnemyCrab::random_color()
{
	Uint32 colors[] = { mapRGB(255, 0, 0), mapRGB(0, 255, 255),
	                    mapRGB(0, 255, 0), mapRGB(255, 0, 255),
	                    mapRGB(0, 0, 255), mapRGB(255, 255, 0) };
	m_color = colors[randint(0, 5)];
}

void EnemyCrab::create_bullet()
{
	if (m_target == NULL)
		return;
    
	DataManager *dm = DataManager::get_instance();

	vector2d dir = m_target->get_center() - get_center();
	dir.normalize();
	dir *= 2 * dm->get_double("pbullet_len");

	int theta[] = { -30, -15, 0, 15, 30 };
	for (int i = 0; i < 5; ++i)
	{
		double c = cos(theta[i] * M_PI / 180.0);
		double s = sin(theta[i] * M_PI / 180.0);
		vector2d rot_dir(c * dir[0] - s * dir[1],
		                 s * dir[0] + c * dir[1]);
		double v = 0.5 * dm->get_double("pbullet_speed");
		new Bullet(this, rot_dir, v, m_color);
	}
}
