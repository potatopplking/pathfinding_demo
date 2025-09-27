#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>

#include "math.hpp"
#include "log.hpp"

class Sprite {
public:
  Sprite() : m_Texture(nullptr, SDL_DestroyTexture) {}
  Sprite(std::string path, Vec2D<float> center) : Sprite() {
    LoadImage(path, center);
  }
  int m_R = 0;
  int m_G = 0;
  int m_B = 0;
  int m_A = 0;
  Sprite(const Sprite &x) = delete;
  Sprite(Sprite &&x) = delete;
  ~Sprite() { LOG_DEBUG("."); }
  void LoadImage(std::string path, Vec2D<float> image_center = {0.0, 0.0}) {
    LOG_INFO("Loading image ", path);
    assert(m_Renderer != nullptr);

    auto surface = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>(
        IMG_Load(path.c_str()), SDL_DestroySurface);

    if (surface == nullptr) {
      LOG_ERROR("IMG_Load failed to load ", path);
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
  static void SetRenderer(std::shared_ptr<SDL_Renderer> renderer) {
    m_Renderer = renderer;
  }

  // GetTexture cannot return pointer to const, as SDL_RenderTexture modifies it
  SDL_Texture *GetTexture() { return m_Texture.get(); }

  Vec2D<float> GetSize() const { return m_Size; }
  Vec2D<float> GetCenter() const { return m_ImageCenter; }

private:
  static std::shared_ptr<SDL_Renderer> m_Renderer;
  std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> m_Texture;
  Vec2D<float> m_Size;
  Vec2D<float> m_ImageCenter;
  float m_TextureWidth = 0;
  float m_TextureHeight = 0;
};
std::shared_ptr<SDL_Renderer> Sprite::m_Renderer = nullptr;


