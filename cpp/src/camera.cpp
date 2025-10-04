#include "camera.hpp"
#include "math.hpp"


// for now only pass-through placeholder functions,
// since we draw the whole map 

WindowPos Camera::WorldToWindow(WorldPos world) const
{
  return WindowPos{world[0], world[1]};
}

WorldPos Camera::WindowToWorld(WindowPos window) const
{
    return WorldPos{window[0], window[1]};
}
