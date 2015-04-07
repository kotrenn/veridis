#include "actorfactory.h"

ActorFactory *ActorFactory::m_instance = NULL;

ActorFactory *ActorFactory::get_instance()
{
	if (m_instance == NULL)
		m_instance = new ActorFactory();
	return m_instance;
}

ActorFactory::ActorFactory()
	:m_actors()
{
	//insert(new PlayerActor());
	insert(new TripleShot());
	insert(new RadialShot());
	insert(new RapidShot());
	insert(new QuadShot());
	insert(new EnemyCrab());
}

void ActorFactory::insert(Actor *actor)
{
	m_actors[actor->get_name()] = actor;
}

ActorFactory::~ActorFactory()
{
	map<string, Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
		delete (*it).second;
}

Actor *ActorFactory::load_actor(Level *level, int x, int y,
                                string name)
{
	if (m_actors.find(name) == m_actors.end())
	{
		fprintf(stderr, "ActorFactory: could not find actor <%s>\n",
		        name.c_str());
		exit(1);
	}
	return m_actors[name]->clone(level, x, y);
}

void ActorFactory::get_actors(vector<string> *dst)
{
	map<string, Actor *>::iterator it;
	for (it = m_actors.begin(); it != m_actors.end(); it++)
		dst->push_back((*it).first);
}
