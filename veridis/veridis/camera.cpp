#include "camera.h"
#include "datamanager.h"

Camera::Camera(int w, int h, int space_w, int space_h,
	       Surface *surface)
    :m_surface(NULL),
     m_x(0), m_y(0),
     m_w(w), m_h(h),
     m_tx(0), m_ty(0),
     m_space_w(space_w),
     m_space_h(space_h),
     m_target(NULL)
{
    if (surface == NULL)
	m_surface = new Surface(m_w, m_h);
    else
	m_surface = new Surface(surface);
}

Camera::Camera(DATFile *in, Surface *surface)
    :m_surface(surface),
     m_x(0), m_y(0),
     m_w(0), m_h(0),
     m_tx(0), m_ty(0),
     m_space_w(0), m_space_h(0),
     m_target(NULL)
{
    vector2d pos;
    in->read_vec(pos);
    m_x = pos[0];
    m_y = pos[1];
    vector2d size;
    in->read_vec(size);
    m_w = size[0];
    m_h = size[1];
    m_tx = m_x;
    m_ty = m_y;
    vector2d space;
    in->read_vec(space);
    m_space_w = space[0];
    m_space_h = space[1];

    if (surface == NULL)
	m_surface = new Surface(m_w, m_h);
    else
	m_surface = new Surface(surface);
}

Camera::~Camera()
{
    delete m_surface;
}

void Camera::save(DATFile *out) const
{
    out->write_vec(vector2d(m_x, m_y));
    out->write_vec(vector2d(m_w, m_h));
    out->write_vec(vector2d(m_space_w, m_space_h));
}

void Camera::move()
{
    if (m_target != NULL)
    {
	vector2d center = m_target->get_center();
	m_tx = center[0] - m_w / 2;
	m_ty = center[1] - m_h / 2;
    }
    
    int dx = m_tx - m_x;
    int dy = m_ty - m_y;
    m_x += 0.05 * dx;
    m_y += 0.05 * dy;
    
    if (m_x < 0)
	m_x = 0;
    if (m_y < 0)
	m_y = 0;
    if (m_x + m_w > m_space_w)
	m_x = m_space_w - m_w;
    if (m_y + m_h > m_space_h)
	m_y = m_space_h - m_h;
}

void Camera::clear()
{
    m_surface->fill(0);
}

void Camera::draw(Surface *dst, int x, int y) const
{
    dst->blit(m_surface, x, y);
}

const vector2d Camera::get_pos() const
{
    vector2d ret(m_x, m_y);
    return ret;
}

const vector2d Camera::get_size() const
{
    vector2d ret(m_w, m_h);
    return ret;
}

void Camera::set_surface(Surface *surface)
{
    if (m_surface == surface)
	return;
    delete m_surface;
    m_surface = new Surface(surface);
}

void Camera::set_pos(const vector2d &pos)
{
    m_tx = pos[0];
    m_ty = pos[1];
}
