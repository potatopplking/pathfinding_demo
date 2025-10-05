#pragma once

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cmath>
#include <expected>
#include <memory>
#include <string>

#include "math.hpp"
#include "sprite.hpp"

class Window {
public:
  Window(int width, int height);
  ~Window();

  Window(const Window &x) = delete;
  Window(Window &&x) = delete;
  Window &operator=(const Window &) = delete;
  Window &operator=(Window &&) = delete;

  std::expected<void, std::string> Init();
  void DrawSprite(const WindowPos &position, Sprite &s);
  void DrawRect(const WindowPos &position, const WindowSize size, uint8_t R,
                uint8_t G, uint8_t B, uint8_t A);
  void ClearWindow();
  void Flush();
  void DrawCircle(const WindowPos &position, float radius);
  void DrawLine(const WindowPos &A, const WindowPos &B);

private:
  uint32_t m_Width;
  uint32_t m_Height;
  std::shared_ptr<SDL_Renderer> m_Renderer = nullptr;
  SDL_Window *m_Window;
  SDL_GLContext m_Context;

};
