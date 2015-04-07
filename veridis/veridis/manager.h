#ifndef MANAGER__H
#define MANAGER__H

#include "console.h"
#include "level.h"

class AbstractManager
{
public:
    AbstractManager();
    virtual ~AbstractManager();

    virtual void key_pressed(SDLKey key, SDLMod mod) = 0;
    virtual void mouse_event(SDL_MouseButtonEvent event) = 0;

    virtual void update() = 0;
    virtual void draw(Surface *dst) const = 0;

    virtual Level *get_level() const = 0;
    virtual Camera *get_camera() const = 0;
    virtual PlayerActor *get_player() const = 0;
    virtual void set_level(Level *level) = 0;
    virtual void set_camera(Camera *camera) = 0;
    virtual void set_player(PlayerActor *player) = 0;
};

class DecoratorManager : public AbstractManager
{
public:
    DecoratorManager(AbstractManager *inner);
    virtual ~DecoratorManager();

    virtual void key_pressed(SDLKey key, SDLMod mod)
    { m_inner->key_pressed(key, mod); }
    virtual void mouse_event(SDL_MouseButtonEvent event)
    { m_inner->mouse_event(event); }

    virtual void update() { m_inner->update(); }
    virtual void draw(Surface *dst) const { m_inner->draw(dst); }

    Level *get_level() const { return m_inner->get_level(); }
    Camera *get_camera() const { return m_inner->get_camera(); }
    PlayerActor *get_player() const { return m_inner->get_player(); }
    void set_level(Level *level) { m_inner->set_level(level); }
    void set_camera(Camera *camera) { m_inner->set_camera(camera); }
    void set_player(PlayerActor *player) { m_inner->set_player(player); }
protected:
    AbstractManager *m_inner;
};

class Manager : public AbstractManager
{
public:
    Manager();
    ~Manager();

    void key_pressed(SDLKey key, SDLMod);
    void mouse_event(SDL_MouseButtonEvent);

    void pause();
    void unpause();

    void update();
    void draw(Surface *dst) const;

    Level *get_level() const { return m_level; }
    Camera *get_camera() const { return m_camera; }
    PlayerActor *get_player() const { return m_player; }
    void set_level(Level *level);
    void set_camera(Camera *camera) { m_camera = camera; }
    void set_player(PlayerActor *player) { m_player = player; }
protected:
    Level *m_level;
    Camera *m_camera;
    bool m_paused;
    PlayerActor *m_player;
    Uint32 m_prev_move;
    Uint32 m_pause_ticks;
};

#endif
