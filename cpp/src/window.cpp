#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cmath>
#include <cstdlib>
#include <expected>
#include <memory>
#include <string>

#include "window.hpp"

#include "log.hpp"
#include "math.hpp"
#include "sprite.hpp"

Window::Window(int width, int height) : m_Width(width), m_Height(height) {
  LOG_DEBUG(".");
}

std::expected<void, std::string> Window::Init() {
  LOG_DEBUG(".");

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    return std::unexpected(std::string("SDL could not initialize! Error: ") +
                           SDL_GetError());
  }
  m_Window = SDL_CreateWindow("SDL2 Window", m_Width, m_Height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (m_Window == nullptr) {
    std::atexit(SDL_Quit);
    return std::unexpected(std::string("Window could not be created! Error: ") +
                           SDL_GetError());
  }

  m_Context = SDL_GL_CreateContext(m_Window);
  if (m_Context == nullptr) {
    SDL_DestroyWindow(m_Window);
    std::atexit(SDL_Quit);
    return std::unexpected(
        std::string("GL context could not be created! Error: ") +
        SDL_GetError());
  }

  if (glewInit() != GLEW_OK) {
    SDL_GL_DestroyContext(m_Context);
    SDL_DestroyWindow(m_Window);
    std::atexit(SDL_Quit);
    return std::unexpected("GLEW init failed!");
  }

  // Resize();

  m_Renderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(m_Window, NULL),
                                             SDL_DestroyRenderer);
  if (m_Renderer == nullptr) {
    SDL_DestroyWindow(m_Window);
    std::atexit(SDL_Quit);
    return std::unexpected(
        std::string("Renderer could not be created! Error: ") + SDL_GetError());
  }

  // Set renderer to the Sprite class
  Sprite::SetRenderer(m_Renderer);

  // TODO this needs to be tied to map size
  SDL_SetRenderScale(m_Renderer.get(), 1.0f, 1.0f);

  return {};
}

Window::~Window() {
  // SDL_DestroyRenderer(m_Renderer); // handled by shared_ptr
  SDL_GL_DestroyContext(m_Context);
  SDL_DestroyWindow(m_Window);
  std::atexit(SDL_Quit);
  LOG_DEBUG(".");
}

void Window::DrawSprite(const WindowPos &position, Sprite &s, float scale) {
  WorldSize size = s.GetSize() * scale;
  WorldPos img_center = s.GetCenter() * scale;
  SDL_FRect rect = {position.x() - img_center.x(), position.y() - img_center.y(),
                    size.x(), size.y()};
  SDL_RenderTexture(m_Renderer.get(), s.GetTexture(), nullptr, &rect);
}

void Window::DrawFilledRect(const WindowPos &position, const WindowSize size, uint8_t R,
                      uint8_t G, uint8_t B, uint8_t A) {
  SDL_FRect rect = {position.x(), position.y(), size.x(), size.y()};
  SDL_SetRenderDrawColor(m_Renderer.get(), R, G, B, A);
  SDL_RenderFillRect(m_Renderer.get(), &rect);
}

void Window::DrawRect(const WindowPos &position, const WindowSize size, uint8_t R, uint8_t G, uint8_t B) {
  SDL_FRect rect = {position.x(), position.y(), size.x(), size.y()};
  SDL_SetRenderDrawColor(m_Renderer.get(), R, G, B, 255);
  SDL_RenderRect(m_Renderer.get(), &rect);
}

void Window::ClearWindow() {
  SDL_SetRenderDrawColor(m_Renderer.get(), 50, 50, 50, 255);
  SDL_RenderClear(m_Renderer.get());
}

void Window::Flush() { SDL_RenderPresent(m_Renderer.get()); }

// TODO use some struct for color
void Window::DrawCircle(const WindowPos &position, float radius, uint8_t R, uint8_t G, uint8_t B) {
  int cx = static_cast<int>(position.x());
  int cy = static_cast<int>(position.y());
  SDL_SetRenderDrawColor(m_Renderer.get(), R, G, B, 255);
  for (int i = 0; i < 360; ++i) {
    double a = i * M_PI / 180.0;
    SDL_RenderPoint(m_Renderer.get(),
                    cx + static_cast<int>(std::round(radius * std::cos(a))),
                    cy + static_cast<int>(std::round(radius * std::sin(a))));
  }
}

void Window::DrawLine(const WindowPos &A, const WindowPos &B)
{
  SDL_SetRenderDrawColor(m_Renderer.get(), 255, 0, 0, 255);
  SDL_RenderLine(m_Renderer.get(), A.x(), A.y(), B.x(), B.y());
}

