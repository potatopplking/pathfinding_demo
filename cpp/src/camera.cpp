#include "camera.hpp"
#include "math.hpp"
#include "log.hpp"

// for now only pass-through placeholder functions,
// since we draw the whole map 


void Camera::Pan(const WorldPos& delta)
{
  LOG_DEBUG("m_Pan before: ", m_Pan, " delta ", delta);
  m_Pan += delta;
  LOG_DEBUG("m_Pan after: ", m_Pan);
}

void Camera::Zoom(const WorldPos&)
{

}

WindowPos Camera::WorldToWindow(WorldPos world) const
{
  const auto& v = world + m_Pan;
  return WindowPos{v[0], v[1]};
}

WorldPos Camera::WindowToWorld(WindowPos window) const
{
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
