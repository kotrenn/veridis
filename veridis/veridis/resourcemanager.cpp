#include "resourcemanager.h"

ResourceManager *ResourceManager::m_instance = NULL;

ResourceManager *ResourceManager::get_instance()
{
	if (m_instance == NULL)
		m_instance = new ResourceManager();
	return m_instance;
}

ResourceManager::ResourceManager()
	:m_images(),
	 m_fonts(),
	 m_font(NULL),
	 m_music(),
	 m_cur_music("")
{
	m_font = get_font("freesansbold.ttf", 20);
}

ResourceManager::~ResourceManager()
{
	map<string, Surface *>::iterator si;
	for (si = m_images.begin(); si != m_images.end(); si++)
		delete (*si).second;
	map<pair<string, int>, TTF_Font *>::iterator fi;
	for (fi = m_fonts.begin(); fi != m_fonts.end(); fi++)
		TTF_CloseFont((*fi).second);
	map<string, Mix_Music *>::iterator mi;
	for (mi = m_music.begin(); mi != m_music.end(); mi++)
		Mix_FreeMusic((*mi).second);
}

Surface *ResourceManager::get_image(const char *filename)
{
	string file_str(filename);
	if (m_images.find(file_str) == m_images.end())
	{
		char fullname[80];
		snprintf(fullname, 80, "images/%s", filename);
		m_images[file_str] = new Surface(fullname);
	}
	return m_images[file_str];
}

TTF_Font *ResourceManager::get_font(const char *filename, int size)
{
	if (filename == NULL)
		return m_font;
    
	pair<string, int> font(string(filename), size);
	if (m_fonts.find(font) == m_fonts.end())
	{
		char fullname[80];
		snprintf(fullname, 80, "fonts/%s", filename);
		if ((m_fonts[font] = TTF_OpenFont(fullname, size)) == NULL)
		{
			fprintf(stderr, "Could not open font <%s>\n", filename);
			fprintf(stderr, "%s\n", SDL_GetError());
			exit(1);
		}
	}
	return m_fonts[font];
}

void ResourceManager::play_music(const char *filename)
{
	string file_str(filename);
	if (m_music.find(file_str) == m_music.end())
	{
		char fullname[80];
		snprintf(fullname, 80, "music/%s", filename);
		m_music[file_str] = Mix_LoadMUS(fullname);
		if (m_music[file_str] == NULL)
			fprintf(stderr, "Mix_LoadMUS failed: %s\n", SDL_GetError());
	}
	if (m_cur_music == file_str)
		return;
	else
	{
		m_cur_music = file_str;
		Mix_PlayMusic(m_music[file_str], -1);
	}
}
