#ifndef RESOURCE_MANAGER__H
#define RESOURCE_MANAGER__H

#include "surface.h"

class ResourceManager
{
public:
    static ResourceManager *get_instance();
    ~ResourceManager();

    Surface *get_image(const char *filename);
    TTF_Font *get_font(const char *filename = NULL, int size = 10);
    int get_font_size() const { return 20; }
    void play_music(const char *filename);
private:
    ResourceManager();
    static ResourceManager *m_instance;

    map<string, Surface *> m_images;
    map<pair<string, int>, TTF_Font *> m_fonts;
    TTF_Font *m_font;
    map<string, Mix_Music *> m_music;
    string m_cur_music;
};

#endif
