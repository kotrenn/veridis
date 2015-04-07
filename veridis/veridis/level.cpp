#include "level.h"

#include "actorfactory.h"
#include "datamanager.h"
#include "resourcemanager.h"

Level::Level()
    :m_x(0), m_y(0),
     m_tiles(NULL),
     m_tile_w(0),
     m_tile_h(0),
     m_w(0), m_h(0),
     m_curid(0),
     m_actors(), m_actors_add(), m_actors_remove(),
     m_bullets(), m_bullets_add(), m_bullets_remove(),
     m_items(), m_items_add(), m_items_remove(),
     m_particles(),
     m_spawn()
{
    DataManager *dm = DataManager::get_instance();

    vector2d size = dm->get_vec("size");
    m_w = size[0];
    m_h = size[1];

    TileFactory *tf = TileFactory::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    m_tile_w = m_w / tile_w;
    m_tile_h = m_h / tile_h;
    m_tiles = (Tile *)malloc(sizeof(Tile) * m_tile_w * m_tile_h);
    for (int y = 0; y < m_tile_h; ++y)
    {
	for (int x = 0; x < m_tile_w; ++x)
	{
	    int tile_x = randint(0, 5);
	    int tile_y = randint(0, 2);
	    tile_x = tile_y = 0;
	    if (randint(1, 10) == 1 ||
		x == 0 || y == 0 ||
		x == m_tile_w - 1 ||
		y == m_tile_h - 1)
	    {
		tile_x = 1;
	    }
	    m_tiles[y * m_tile_w + x] = tf->build_tile(tile_x, tile_y);
	}
    }

    /*
    int num = dm->get_int("num_enemies");
    for (int i = 0; i < num; ++i)
    {
	new TripleShot(this);
	new RadialShot(this);
    }
    */
}

Level::Level(int x, int y)
    :m_x(x), m_y(y),
     m_tiles(NULL),
     m_w(0), m_h(0),
     m_curid(0),
     m_actors(), m_actors_add(), m_actors_remove(),
     m_bullets(), m_bullets_add(), m_bullets_remove(),
     m_items(), m_items_add(), m_items_remove(),
     m_particles()
{
    DATFile in;
    char filename[80];
    snprintf(filename, 80, "map_%d_%d.dat", m_x, m_y);
    in.open(filename, "r");
    
    vector2d size;
    in.read_vec(size);
    m_w = size[0];
    m_h = size[1];

    in.read_int(&m_tile_w);
    in.read_int(&m_tile_h);
    m_tiles = (Tile *)malloc(sizeof(Tile) * m_tile_w * m_tile_h);
    for (int i = 0; i < m_tile_w * m_tile_h; ++i)
	m_tiles[i] = Tile(&in);

    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int num_spawn;
    in.read_int(&num_spawn);
    ActorFactory *af = ActorFactory::get_instance();
    for (int i = 0; i < num_spawn; ++i)
    {
	int x, y;
	string actor;
	in.read_int(&x);
	in.read_int(&y);
	in.read_str(&actor);
	int actor_x = tile_w * (1.0 * x + 0.5);
	int actor_y = tile_h * (1.0 * y + 0.5);
	af->load_actor(this, actor_x, actor_y, actor);
	m_spawn[pair<int, int>(x, y)] = actor;
    }

    in.close();
}

Level::~Level()
{
    free(m_tiles);
    
    add_lists();

    set<Actor *>::iterator a;
    set<Bullet *>::iterator b;
    set<Item *>::iterator i;

    for (a = m_actors.begin(); a != m_actors.end(); a++)
	remove_actor(*a);
    for (b = m_bullets.begin(); b != m_bullets.end(); b++)
	remove_bullet(*b);
    for (i = m_items.begin(); i != m_items.end(); i++)
	remove_item(*i);
    m_actors.clear();
    m_bullets.clear();
    m_items.clear();
    
    clear_lists();

    while (m_particles.size() > 0)
    {
	delete m_particles.back();
	m_particles.pop_back();
    }
}

void Level::save() const
{
    DATFile out;
    char filename[80];
    snprintf(filename, 80, "map_%d_%d.dat", m_x, m_y);
    out.open(filename, "w");
    
    set<Actor *>::const_iterator a;
    set<Bullet *>::const_iterator b;
    set<Item *>::const_iterator i;
    list<Particle *>::const_iterator p;

    out.write_vec(vector2d(m_w, m_h));
    out.write_int(m_tile_w);
    out.write_int(m_tile_h);
    for (int i = 0; i < m_tile_w * m_tile_h; ++i)
	m_tiles[i].save(&out);

    out.write_int(m_spawn.size());
    map<pair<int, int>, string>::const_iterator it;
    for (it = m_spawn.begin(); it != m_spawn.end(); it++)
    {
	out.write_int((*it).first.first);
	out.write_int((*it).first.second);
	out.write_str((*it).second.c_str());
    }

    out.close();
}

void Level::pause()
{
    set<Actor *>::iterator a;
    list<Particle *>::iterator p;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	(*a)->pause();
    for (p = m_particles.begin(); p != m_particles.end(); p++)
	(*p)->pause();
}

void Level::unpause()
{
    set<Actor *>::iterator a;
    list<Particle *>::iterator p;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	(*a)->unpause();
    for (p = m_particles.begin(); p != m_particles.end(); p++)
	(*p)->unpause();
}

void Level::update()
{
    set<Actor *>::iterator a;
    set<Bullet *>::iterator b;
    set<Item *>::iterator i;

    for (a = m_actors.begin(); a != m_actors.end(); a++)
	(*a)->update();
    for (b = m_bullets.begin(); b != m_bullets.end(); b++)
	(*b)->update();
    for (i = m_items.begin(); i != m_items.end(); i++)
	(*i)->update();

    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    for (b = m_bullets.begin(); b != m_bullets.end(); b++)
    {
	if (out_of_bounds(*b))
	    remove_bullet(*b);
	Line line = (*b)->get_line();
	int x1 = line.get_x1() / tile_w;
	int y1 = line.get_y1() / tile_h;
	int x2 = line.get_x2() / tile_w;
	int y2 = line.get_y2() / tile_h;
	if (x1 >= 0 && x1 < m_tile_w &&
	    y1 >= 0 && y1 < m_tile_h)
	    if (m_tiles[y1 * m_tile_w + x1].is_wall())
		remove_bullet(*b);
	if (x2 >= 0 && x2 < m_tile_w &&
	    y2 >= 0 && y2 < m_tile_h)
	    if (m_tiles[y2 * m_tile_w + x2].is_wall())
		remove_bullet(*b);
    }
    
    add_lists();
    clear_lists();

    list<Particle *>::iterator p = m_particles.begin();
    while (p != m_particles.end())
    {
	(*p)->update();
	if ((*p)->dead())
	{
	    Particle *particle = *p;
	    p = m_particles.erase(p);
	    delete particle;
	}
	else
	    p++;
    }
}

void Level::add_lists()
{
    set<Actor *>::iterator a;
    set<Bullet *>::iterator b;
    set<Item *>::iterator i;
    
    for (a = m_actors_add.begin(); a != m_actors_add.end(); a++)
	m_actors.insert(*a);
    for (b = m_bullets_add.begin(); b != m_bullets_add.end(); b++)
	m_bullets.insert(*b);
    for (i = m_items_add.begin(); i != m_items_add.end(); i++)
	m_items.insert(*i);

    m_actors_add.clear();
    m_bullets_add.clear();
    m_items_add.clear();
}

void Level::clear_lists()
{
    set<Actor *>::iterator a;
    set<Bullet *>::iterator b;
    set<Item *>::iterator i;

    for (a = m_actors_remove.begin(); a != m_actors_remove.end(); a++)
    {
	set<Actor *>::iterator a2;
	for (a2 = m_actors.begin(); a2 != m_actors.end(); a2++)
	    (*a2)->actor_dying(*a);
	for (b = m_bullets.begin(); b != m_bullets.end(); b++)
	    (*b)->actor_dying(*a);
    }
    for (a = m_actors_remove.begin(); a != m_actors_remove.end(); a++)
    {
	m_actors.erase(*a);
	if ((*a)->get_team() != TEAM_PLAYER)
	    delete *a;
    }

    for (b = m_bullets_remove.begin(); b != m_bullets_remove.end(); b++)
    {
	m_bullets.erase(*b);
	delete *b;
    }

    for (i = m_items_remove.begin(); i != m_items_remove.end(); i++)
    {
	m_items.erase(*i);
	delete *i;
    }

    m_actors_remove.clear();
    m_bullets_remove.clear();
    m_items_remove.clear();
}

void Level::move(double dt)
{
    set<Actor *>::iterator a;
    set<Bullet *>::iterator b;
    set<Item *>::iterator i;
    
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	(*a)->move(dt);
    for (b = m_bullets.begin(); b != m_bullets.end(); b++)
	(*b)->move(dt);
    for (i = m_items.begin(); i != m_items.end(); i++)
	(*i)->move(dt);

    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    for (a = m_actors.begin(); a != m_actors.end(); a++)
    {
	if (!(*a)->get_flying())
	{
	    Circle circle = (*a)->get_circle();
	    int min_x = (circle[0] - circle[2]) / tile_w;
	    int min_y = (circle[1] - circle[2]) / tile_h;
	    int max_x = (circle[0] + circle[2]) / tile_w;
	    int max_y = (circle[1] + circle[2]) / tile_h;
	    min_x = max(min_x, 0);
	    min_y = max(min_y, 0);
	    max_x = min(max_x, m_tile_w - 1);
	    max_y = min(max_y, m_tile_h - 1);
	    if (min_x < m_tile_w && min_y < m_tile_h &&
		max_x > 0 && max_y > 0)
		for (int x = min_x; x <= max_x; x++)
		    for (int y = min_y; y <= max_y; y++)
			m_tiles[y * m_tile_w + x].fix(*a,
						      x * tile_w,
						      y * tile_h,
						      tile_w,
						      tile_h);
	}
    }

    list<Particle *>::iterator p;
    for (p = m_particles.begin(); p != m_particles.end(); p++)
	(*p)->move(dt);
}

void Level::draw(Camera *camera) const
{
    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int min_x = camera->get_x() / tile_w;
    int min_y = camera->get_y() / tile_h;
    int max_x = min_x + camera->get_w() / tile_h;
    int max_y = min_y + camera->get_h() / tile_h;
    min_x = max(min_x, 0);
    min_y = max(min_y, 0);
    max_x = min(max_x, m_tile_w - 1);
    max_y = min(max_y, m_tile_h - 1);
    for (int y = min_y; y <= max_y; ++y)
	for (int x = min_x; x <= max_x; ++x)
	    m_tiles[y * m_tile_w + x].draw(camera, x, y, tile_w, tile_h);

    set<Actor *>::const_iterator a;
    set<Bullet *>::const_iterator b;
    set<Item *>::const_iterator i;
    
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	(*a)->draw(camera);
    for (b = m_bullets.begin(); b != m_bullets.end(); b++)
	(*b)->draw(camera);
    for (i = m_items.begin(); i != m_items.end(); i++)
	(*i)->draw(camera);

    list<Particle *>::const_iterator p;
    for (p = m_particles.begin(); p != m_particles.end(); p++)
	(*p)->draw(camera);

    /*
    Surface *dst = camera->get_surface();

    Uint32 white = mapRGB(255, 255, 255);

    char actor_str[80];
    snprintf(actor_str, 80, "m_actors.size(): %d", (int)m_actors.size());
    dst->draw_string(actor_str, white, 5, 5);
    
    char bullet_str[80];
    snprintf(bullet_str, 80, "m_bullets.size(): %d", (int)m_bullets.size());
    dst->draw_string(bullet_str, white, 5, 25);
    */
}

void Level::add_actor(Actor *actor)
{
    m_actors_add.insert(actor);
}

void Level::add_bullet(Bullet *bullet)
{
    m_bullets_add.insert(bullet);
}

void Level::add_item(Item *item)
{
    m_items_add.insert(item);
}

void Level::add_particle(Particle *particle)
{
    m_particles.push_back(particle);
}

void Level::remove_actor(Actor *actor)
{
    m_actors_remove.insert(actor);
}

void Level::remove_bullet(Bullet *bullet)
{
    m_bullets_remove.insert(bullet);
}

void Level::remove_item(Item *item)
{
    m_items_remove.insert(item);
}

void Level::clear_items()
{
    set<Item *>::iterator i;
    for (i = m_items.begin(); i != m_items.end(); i++)
	remove_item(*i);
}

bool Level::out_of_bounds(const Bullet *bullet) const
{
    Rect bounds(0, 0, m_w, m_h);
    Line line = bullet->get_line();
    return !line.collides(&bounds);
}

void Level::get_collisions(list<Actor *> *actors,
			   const Bullet *bullet) const
{
    set<Actor *>::const_iterator a;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	if ((*a)->get_team() != bullet->get_team())
	    if (bullet->collides(*a))
		actors->push_back(*a);
}

void Level::get_collisions(list<Actor *> *actors,
			   const Item *item) const
{
    set<Actor *>::const_iterator a;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	if (item->collides(*a))
	    actors->push_back(*a);
}

Actor *Level::get_next_target(Actor *src, Actor *target, double dist) const
{
    Actor *ret = NULL;
    set<Actor *>::const_iterator a;

    bool enemies = false;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	if ((*a)->get_team() != src->get_team())
	    if ((src->get_center() - (*a)->get_center()).norm() < dist)
		enemies = true;

    if (!enemies)
	return NULL;

    bool has_target = false;
    for (a = m_actors.begin(); a != m_actors.end(); a++)
	if (*a == target)
	    has_target = true;

    if (!has_target)
	target = NULL;

    double cur_dist;
    if (target == NULL)
    {
	a = m_actors.begin();
	bool first = true;
	do
	{
	    if (!first)
		a++;
	    first = false;
	    ret = *a;
	    vector2d disp = src->get_center() - (*a)->get_center();
	    cur_dist = disp.norm();
	} while ((*a)->get_team() == src->get_team() || cur_dist > dist);
    }
    else
    {
	set<Actor *>::const_iterator target_it;
	for (a = m_actors.begin(); a != m_actors.end(); a++)
	    if (*a == target)
		target_it = a;

	a = target_it;
	do
	{
	    a++;
	    if (a == m_actors.end())
		a = m_actors.begin();
	    vector2d disp = src->get_center() - (*a)->get_center();
	    cur_dist = disp.norm();
	} while ((*a)->get_team() == src->get_team() || cur_dist > dist);
	ret = *a;
    }
    return ret;
}

string Level::get_spawn(int x, int y) const
{
    string ret = "";
    pair<int, int> pos(x, y);
    map<pair<int, int>, string>::const_iterator it;
    if ((it = m_spawn.find(pos)) != m_spawn.end())
	ret = (*it).second;
    return ret;
}

void Level::set_tile(int x, int y, int tile_x, int tile_y)
{
    m_tiles[y * m_tile_w + x].set_x(tile_x);
    m_tiles[y * m_tile_w + x].set_y(tile_y);
}

void Level::set_spawn(int x, int y, string actor)
{
    if (x < 0 || y < 0 || x >= m_tile_w || y >= m_tile_h)
	return;
    m_spawn[pair<int, int>(x, y)] = actor;
}

void Level::remove_spawn(int x, int y)
{
    pair<int, int> pos(x, y);
    map<pair<int, int>, string>::iterator it;
    if ((it = m_spawn.find(pos)) != m_spawn.end())
	m_spawn.erase(it);
}

void Level::clear_spawn()
{
    m_spawn.clear();
}

void Level::draw_spawn(Surface *dst, Camera *camera) const
{
    DataManager *dm = DataManager::get_instance();
    int tile_w = dm->get_int("tile_w");
    int tile_h = dm->get_int("tile_h");
    int spawn_w = 20;
    int spawn_h = 20;
    int min_x = camera->get_x() / tile_w;
    int min_y = camera->get_y() / tile_h;
    int max_x = 1 + (camera->get_x() + camera->get_w()) / tile_w;
    int max_y = 1 + (camera->get_y() + camera->get_h()) / tile_h;
    Uint32 color = mapRGB(255, 0, 0);
    map<pair<int, int>, string>::const_iterator it;
    for (it = m_spawn.begin(); it != m_spawn.end(); it++)
    {
	pair<int, int> pos = (*it).first;
	if (min_x <= pos.first && pos.first <= max_x &&
	    min_y <= pos.second && pos.second <= max_y)
	{
	    Rect bounds(pos.first  * tile_w,
			pos.second * tile_h,
			spawn_w, spawn_h);
	    bounds[0] += (tile_w - spawn_w) / 2 - camera->get_x();
	    bounds[1] += (tile_h - spawn_h) / 2 - camera->get_y();
	    dst->draw_rect(color, bounds);
	}
    }
}
