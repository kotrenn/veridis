#ifndef ACTOR_FACTORY__H
#define ACTOR_FACTORY__H

#include "actor.h"
#include "datfile.h"

class ActorFactory
{
public:
    static ActorFactory *get_instance();
    ~ActorFactory();

    Actor *load_actor(Level *level, int x, int y,
		      string name);

    void get_actors(vector<string> *dst);
private:
    ActorFactory();
    static ActorFactory *m_instance;

    void insert(Actor *actor);

    map<string, Actor *> m_actors;
};

#endif
