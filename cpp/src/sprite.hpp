#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>

#include "log.hpp"
#include "math.hpp"

class Sprite {
public:
  Sprite();
  ~Sprite();
  explicit Sprite(std::string path, WorldPos center = {0, 0});

  Sprite(const Sprite &) = delete;
  Sprite &operator=(const Sprite &) = delete;
  Sprite(Sprite &&) = delete;
  Sprite &operator=(Sprite &&) = delete;

  static void SetRenderer(std::shared_ptr<SDL_Renderer> renderer);

  // GetTexture cannot return pointer to const, as SDL_RenderTexture modifies it
  SDL_Texture *GetTexture() { return m_Texture.get(); }
  WorldPos GetSize() const { return m_Size; }
  WorldPos GetCenter() const { return m_ImageCenter; }

  void LoadImage(std::string path, WorldPos image_center = {0.0, 0.0});

private:
  static std::shared_ptr<SDL_Renderer> m_Renderer;
  std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> m_Texture;
  WorldPos m_Size;
  WorldPos m_ImageCenter;
  float m_TextureWidth = 0;
  float m_TextureHeight = 0;
};
