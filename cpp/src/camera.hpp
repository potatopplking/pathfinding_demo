#pragma once

#include "math.hpp"

class Camera
{
public:
  void Pan(const WorldPos& delta);
  void Zoom(float delta);

  WindowPos  WorldToWindow(WorldPos)       const;
  WorldPos   WindowToWorld(WindowPos)      const;
  WindowSize WorldToWindowSize(WorldSize)  const;
  WorldSize  WindowToWorldSize(WindowSize) const;

private:
  // TODO this should be replaced with a matrix
  float m_Zoom = 1.0f;
  WorldPos m_Pan;
};
