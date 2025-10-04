#pragma once

#include "math.hpp"

class Camera
{
public:
  WindowPos WorldToWindow(WorldPos) const;
  WorldPos WindowToWorld(WindowPos) const;

private:
  // TODO this should be replaced with a matrix
  float m_Zoom;
  WorldPos m_RectCorner; // upper left corner (0,0) of the drawn regios

};
