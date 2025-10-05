#include "camera.hpp"
#include "math.hpp"
#include "log.hpp"

// for now only pass-through placeholder functions,
// since we draw the whole map 


void Camera::Pan(const WorldPos& delta)
{
  m_Pan += delta;
}

void Camera::Zoom(float delta)
{
  constexpr float ZOOM_SCALE = 0.1f;
  m_Zoom += delta * ZOOM_SCALE;
  LOG_DEBUG("Zoom: ", m_Zoom);
}

WindowPos Camera::WorldToWindow(WorldPos world) const
{
  const auto& v = world + m_Pan;
  return WindowPos{v[0], v[1]} * m_Zoom;
}

WorldPos Camera::WindowToWorld(WindowPos window) const
{
  window /= m_Zoom;
  return WorldPos{window[0], window[1]} - m_Pan;
}


WindowSize Camera::WorldToWindowSize(WorldSize world) const
{
  const auto& v = world;
  // no zoom yet, just pass-through
  return WindowSize{v[0], v[1]};
}

WorldSize Camera::WindowToWorldSize(WindowSize window) const
{
    return WorldSize{window[0], window[1]};
}
