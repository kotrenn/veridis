#include "consolemanager.h"

#include "datamanager.h"

ConsoleManager::ConsoleManager(AbstractManager *inner)
	:DecoratorManager(inner),
	 m_console(NULL)
{
	m_console = new Console(10);
}

ConsoleManager::~ConsoleManager()
{
	delete m_console;
}

void ConsoleManager::key_pressed(SDLKey key, SDLMod mod)
{
	if (key == SDLK_BACKQUOTE)
		m_console->set_visible(m_console->is_visible() == false);
	
	if (m_console->is_visible())
	{
		if (key == SDLK_RETURN)
		{
			string input = m_console->get_input();
			m_console->new_line();
			parse_input(input);
		}
		else
			m_console->key_pressed(key, mod);
	}
	else
	{
		DecoratorManager::key_pressed(key, mod);
	}
}

void ConsoleManager::draw(Surface *dst) const
{
	DecoratorManager::draw(dst);
    
	m_console->draw(dst);
}

void ConsoleManager::parse_input(string input)
{
	if (input.size() == 0)
		return;
    
	DataManager *dm = DataManager::get_instance();
    
	vector<string> words;
	words.push_back("");
	for (unsigned int i = 0; i < input.size(); ++i)
	{
		if (input[i] == ' ')
		{
			if (words.back().size() > 0)
				words.push_back("");
		}
		else
		{
			words.back().push_back(input[i]);
		}
	}
	if (words.back().size() == 0)
		words.pop_back();

	if (words[0] == "save")
	{
		if (words[1] == "settings")
		{
			dm->save();
		}
		else if (words[1] == "level")
		{
			Level *level = m_inner->get_level();
			int x = level->get_x();
			int y = level->get_y();
			level->save();
			char buf[80];
			snprintf(buf, 80, "Saved level to map_%d_%d.dat", x, y);
			m_console->add_line(buf);
		}
	}
	else if (words[0] == "set")
	{
		if (words[1] == "int")
		{
			int val;
			sscanf(words[3].c_str(), "%d", &val);
			dm->set_int(words[2].c_str(), val);
		}
		else if (words[1] == "str")
		{
			dm->set_str(words[2].c_str(), words[3].c_str());
		}
		else if (words[1] == "vec")
		{
			double x, y;
			sscanf(words[3].c_str(), "%lf %lf", &x, &y);
			dm->set_vec(words[2].c_str(), vector2d(x, y));
		}
		else if (words[1] == "double")
		{
			double val;
			sscanf(words[3].c_str(), "%lf", &val);
			dm->set_double(words[2].c_str(), val);
		}
	}
	else if (words[0] == "load")
	{
		if (words[1] == "level")
		{
			int x = atoi(words[2].c_str());
			int y = atoi(words[3].c_str());
			m_inner->set_level(new Level(x, y));
			char buf[80];
			snprintf(buf, 80, "Loaded level [%d, %d]", x, y);
		}
	}
	else if (words[0] == "clear")
	{
		if (words[1] == "spawn")
		{
			m_inner->get_level()->clear_spawn();
		}
	}
	else if (words[0] == "malloy")
	{
		if (words[1] == "mode")
		{
			PlayerActor *player = m_inner->get_player();
			for (int i = 0; i < 10; ++i)
				for (int j = 1; j <= 3; ++j)
					player->upgrade(j);
		}
	}
}
