#ifndef CAMERA__H
#define CAMERA__H

#include "common.h"
#include "actor.h"
#include "datfile.h"
#include "surface.h"

class Actor;

class Camera
{
public:
    Camera(int w, int h, int space_w, int space_h,
	   Surface *surface = NULL);
    Camera(DATFile *in, Surface *surface = NULL);
    ~Camera();

    void save(DATFile *out) const;

    void move();
    void clear();
    void draw(Surface *dst, int x, int y) const;

    Surface *get_surface() const { return m_surface; }
    int get_x() const { return m_x; }
    int get_y() const { return m_y; }
    int get_w() const { return m_w; }
    int get_h() const { return m_h; }
    const vector2d get_pos() const;
    const vector2d get_size() const;
    void set_surface(Surface *surface);
    void set_x(int x) { m_tx = x; }
    void set_y(int y) { m_ty = y; }
    void set_w(int w) { m_w = w; }
    void set_h(int h) { m_h = h; }
    void set_pos(const vector2d &pos);
    void set_target(Actor *target) { m_target = target; }
private:
    Surface *m_surface;
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    int m_tx;
    int m_ty;
    int m_space_w;
    int m_space_h;
    Actor *m_target;
};

#endif
