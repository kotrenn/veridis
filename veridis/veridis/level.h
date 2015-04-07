#ifndef LEVEL__H
#define LEVEL__H

class Camera;

#include "camera.h"
#include "actor.h"
#include "bullet.h"
#include "item.h"
#include "particle.h"
#include "surface.h"
#include "tile.h"
#include "datfile.h"

class Actor;
class Bullet;
class Item;
class Particle;
class Tile;

class Level
{
public:
    Level();
    Level(int x, int y);
    virtual ~Level();

    void save() const;

    void pause();
    void unpause();

    void update();
    void move(double dt);
    void draw(Camera *camera) const;

    void add_actor(Actor *actor);
    void add_bullet(Bullet *bullet);
    void add_item(Item *item);
    void add_particle(Particle *particle);
    void remove_actor(Actor *actor);
    void remove_bullet(Bullet *bullet);
    void remove_item(Item *item);

    void clear_items();

    bool out_of_bounds(const Bullet *bullet) const;
    void get_collisions(list<Actor *> *actors,
			const Bullet *bullet) const;
    void get_collisions(list<Actor *> *actors,
			const Item *item) const;

    Actor *get_next_target(Actor *src, Actor *target, double dist) const;

    int get_x() const { return m_x; }
    int get_y() const { return m_y; }
    int get_w() const { return m_w; }
    int get_h() const { return m_h; }
    int get_tile_w() const { return m_tile_w; }
    int get_tile_h() const { return m_tile_h; }
    void set_x(int x) { m_x = x; }
    void set_y(int y) { m_y = y; }

    string get_spawn(int x, int y) const;

    void set_tile(int x, int y, int tile_x, int tile_y);

    void set_spawn(int x, int y, string actor);
    void remove_spawn(int x, int y);
    void clear_spawn();
    
    void draw_spawn(Surface *dst, Camera *camera) const;
private:
    void add_lists();
    void clear_lists();

    int m_x;
    int m_y;
    
    Tile *m_tiles;
    int m_tile_w;
    int m_tile_h;

    int m_w;
    int m_h;

    unsigned int m_curid;

    set<Actor *> m_actors;
    set<Actor *> m_actors_add;
    set<Actor *> m_actors_remove;

    set<Bullet *> m_bullets;
    set<Bullet *> m_bullets_add;
    set<Bullet *> m_bullets_remove;

    set<Item *> m_items;
    set<Item *> m_items_add;
    set<Item *> m_items_remove;
    
    list<Particle *> m_particles;

    map<pair<int, int>, string> m_spawn;
};

#endif
