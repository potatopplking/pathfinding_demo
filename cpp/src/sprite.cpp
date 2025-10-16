#include "sprite.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>

#include "log.hpp"
#include "math.hpp"

Sprite::Sprite() : m_Texture(nullptr, SDL_DestroyTexture) {}

Sprite::Sprite(std::string path, WorldPos center) : Sprite() {
  LoadImage(path, center);
}

Sprite::~Sprite() { LOG_DEBUG("."); }

void Sprite::LoadImage(std::string path, WorldPos image_center) {
  LOG_INFO("Loading image ", path);
  assert(m_Renderer != nullptr);

  auto surface = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>(
      IMG_Load(path.c_str()), SDL_DestroySurface);

  if (surface == nullptr) {
    LOG_ERROR("IMG_Load failed: ", SDL_GetError());
    throw std::runtime_error("Failed to load resources");
  }

  m_Texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(
      SDL_CreateTextureFromSurface(m_Renderer.get(), surface.get()),
      SDL_DestroyTexture);
  if (m_Texture == nullptr) {
    LOG_ERROR("SDL_CreateTextureFromSurface failed");
    throw std::runtime_error("Failed to load resources");
  }
  float w, h;
  SDL_GetTextureSize(m_Texture.get(), &w, &h);
  m_Size = {w, h};
  m_ImageCenter = image_center;
}

// Renderer is shared for all class instances - we need it in order
// to create textures from images
void Sprite::SetRenderer(std::shared_ptr<SDL_Renderer> renderer) {
  m_Renderer = renderer;
}

std::shared_ptr<SDL_Renderer> Sprite::m_Renderer = nullptr;
