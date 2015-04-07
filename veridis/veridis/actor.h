#ifndef ACTOR__H
#define ACTOR__H

#include "camera.h"
#include "level.h"
#include "surface.h"
#include "timer.h"

#define TEAM_PLAYER 0
#define TEAM_ENEMY 1

class Camera;
class Level;

typedef enum direction_enum
{
    NORTH,
    SOUTH,
    WEST,
    EAST
} direction;

class Actor
{
public:
    Actor(Level *level, const string &name, const string &surface);
    Actor(const string &name, const string &surface);
    virtual ~Actor();

    virtual Actor *clone(Level *level, int x, int y) const = 0;

    virtual void pause();
    virtual void unpause();

    virtual void update();
    void move(double dt);
    virtual void draw(Camera *camera) const;

    void explode() const;
    virtual void upgrade(int type) { ++type; }
    virtual void damage();

    Level *get_level() const { return m_level; }
    const string get_name() const { return m_name; }
    const vector2d get_center() const;
    const Circle get_circle() const { return m_circle; }
    const vector2d get_v() const { return m_v; }
    bool get_removable() const { return m_removable; }
    int get_team() const { return m_team; }
    bool get_flying() const { return m_flying; }
    int get_health() const { return m_health; }
    direction get_dir() const { return m_dir; }
    void set_level(Level *level);
    void set_center(const vector2d &center);
    void set_v(const vector2d &v) { m_v = v; }
    void set_target(Actor *target) { m_target = target; }
    void actor_dying(Actor *actor);
protected:
    void clone(Level *level, int x, int y, Actor *actor) const;
    void select_target();
    Uint32 get_ticks() const;
    
    Level *m_level;
    string m_name;
    Surface *m_surface;
    Uint32 m_ticks;
    Uint32 m_pause;
    Circle m_circle;
    vector2d m_v;
    bool m_removable;
    int m_team;
    bool m_flying;
    Actor *m_target;
    int m_health;
    bool m_dead;
    direction m_dir;
};

class PlayerActor : public Actor
{
public:
    PlayerActor(Level *level);
    PlayerActor();
    ~PlayerActor();

    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();

    void update();
    void draw(Camera *camera) const;

    void create_bullet();
    void upgrade(int type);
    void damage();
private:
    Timer m_select_timer;
    double m_fire_rate;
    Timer m_fire_timer;
    int m_num_bullets;
    Timer m_invincible;
};

class TripleShot : public Actor
{
public:
    TripleShot(Level *level);
    TripleShot();
    ~TripleShot();
    
    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();

    void update();

    void create_bullet();
private:
    void random_dir();
    
    Timer m_shoot_timer;
};

class RadialShot : public Actor
{
public:
    RadialShot(Level *level);
    RadialShot();
    ~RadialShot();

    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();
    
    void update();

    void create_bullet();
private:
    void random_dir();
    
    Timer m_shoot_timer;
};

class RapidShot : public Actor
{
public:
    RapidShot(Level *level);
    RapidShot();
    ~RapidShot();

    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();

    void update();

    void create_bullet();
private:
    void random_dir();

    Timer m_shoot_timer;
    int m_num_shots;
};

class QuadShot : public Actor
{
public:
    QuadShot(Level *level);
    QuadShot();
    ~QuadShot();

    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();

    void update();

    void create_bullet();
private:
    void random_dir();

    Timer m_shoot_timer;
};

class EnemyCrab : public Actor
{
public:
    EnemyCrab(Level *level);
    EnemyCrab();
    ~EnemyCrab();

    Actor *clone(Level *level, int x, int y) const;

    void pause();
    void unpause();

    void update();

    void create_bullet();
private:
    void random_color();
    
    Timer m_shoot_timer;
    Timer m_move_timer;
    int m_pos;
    Uint32 m_color;
    int m_num_shots;
};

#endif
