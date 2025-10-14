#pragma once

#include "math.hpp"

class Camera
{
public:
  void Pan(const WorldPos& delta);
  void Zoom(float delta);

  WorldPos GetPan() const { return m_Pan; }
  float GetZoom() const { return m_Zoom; }

  WindowPos  WorldToWindow(WorldPos)       const;
  WorldPos   WindowToWorld(WindowPos)      const;
  WindowSize WorldToWindowSize(WorldSize)  const;
  WorldSize  WindowToWorldSize(WindowSize) const;
  
  template <typename T>
  requires std::floating_point<T>
  T WindowToWorldSize(T window_size) const {
    return window_size / static_cast<T>(m_Zoom);
  }
  
  template <typename T>
  requires std::floating_point<T>
  T WorldToWindowSize(T world_size) const {
    return world_size * static_cast<T>(m_Zoom);
  }

private:
  // TODO this should be replaced with a matrix
  float m_Zoom = 1.0f;
  WorldPos m_Pan;
};
