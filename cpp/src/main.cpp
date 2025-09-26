#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <expected>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <unordered_set>
#include <vector>

#include "array.hpp"
#include "log.hpp"
#include "math.hpp"

//
// Utils
//

//
// Math stuff
//

// Forward declarations
class Sprite;
class Sound;
class UserAction;

//
// UI stuff. This is the part directly dependent on the SDL
//

// containers

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class Sound {
public:
  Sound() { LOG_DEBUG("."); }
  Sound(const Sound &x) { LOG_DEBUG("."); }
  Sound(Sound &&x) noexcept { LOG_DEBUG("."); }
  ~Sound() { LOG_DEBUG("."); }
};

class AudioOutput {
public:
  AudioOutput() { LOG_DEBUG("."); }
  AudioOutput(const AudioOutput &x) = delete;
  AudioOutput(AudioOutput &&x) = delete;
  ~AudioOutput() { LOG_DEBUG("."); }

  std::expected<void, std::string> Init() {
    LOG_INFO("Initialing audio output");
    return {};
  }

  void PlaySound(Sound &s);
};

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

// User interface classes

class Window {
public:
  Window(const Window &x) = delete;
  Window(Window &&x) = delete;
  Window(int width, int height) : m_Width(width), m_Height(height) {
    LOG_DEBUG(".");
  }

  std::expected<void, std::string> Init() {
    LOG_DEBUG(".");

    if (SDL_Init(SDL_INIT_VIDEO) == false) {
      return std::unexpected(std::string("SDL could not initialize! Error: ") +
                             SDL_GetError());
    }
    m_Window = SDL_CreateWindow("SDL2 Window", m_Width, m_Height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (m_Window == nullptr) {
      std::atexit(SDL_Quit);
      return std::unexpected(
          std::string("Window could not be created! Error: ") + SDL_GetError());
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

    m_Renderer = std::shared_ptr<SDL_Renderer>(
        SDL_CreateRenderer(m_Window, NULL), SDL_DestroyRenderer);
    if (m_Renderer == nullptr) {
      SDL_DestroyWindow(m_Window);
      std::atexit(SDL_Quit);
      return std::unexpected(
          std::string("Renderer could not be created! Error: ") +
          SDL_GetError());
    }

    // Set renderer to the Sprite class
    Sprite::SetRenderer(m_Renderer);

    // TODO this needs to be tied to map size
    SDL_SetRenderScale(m_Renderer.get(), 1.0f, 1.0f);

    return {};
  }

  ~Window() {
    // SDL_DestroyRenderer(m_Renderer); // handled by shared_ptr
    SDL_GL_DestroyContext(m_Context);
    SDL_DestroyWindow(m_Window);
    std::atexit(SDL_Quit);
    LOG_DEBUG(".");
  }

  void DrawSprite(const Vec2D<float> &position, Sprite &s) {
    Vec2D<float> size = s.GetSize();
    Vec2D<float> img_center = s.GetCenter();
    SDL_FRect rect = {position.x - img_center.x, position.y - img_center.y,
                      size.x, size.y};
    SDL_RenderTexture(m_Renderer.get(), s.GetTexture(), nullptr, &rect);
  }

  void DrawRect(const Vec2D<float> &position, const Vec2D<float> size,
                uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    SDL_FRect rect = {position.x, position.y,
                      size.x, size.y};
    SDL_SetRenderDrawColor(m_Renderer.get(), R, G, B, A);
    SDL_RenderFillRect(m_Renderer.get(), &rect);
  }

  void ClearWindow() {
    SDL_SetRenderDrawColor(m_Renderer.get(), 50, 50, 50, 255);
    SDL_RenderClear(m_Renderer.get());
  }

  void Flush() { SDL_RenderPresent(m_Renderer.get()); }

  void DrawCircle(const Vec2D<float> &position, float radius) {
    int cx = static_cast<int>(position.x);
    int cy = static_cast<int>(position.y);
    SDL_SetRenderDrawColor(m_Renderer.get(), 255, 0, 0, 255);
    for (int i = 0; i < 360; ++i) {
      double a = i * M_PI / 180.0;
      SDL_RenderPoint(m_Renderer.get(),
                      cx + static_cast<int>(std::round(radius * std::cos(a))),
                      cy + static_cast<int>(std::round(radius * std::sin(a))));
    }
  }

  std::shared_ptr<SDL_Renderer> m_Renderer = nullptr;
  SDL_Window *m_Window;
  SDL_GLContext m_Context;

private:
  uint32_t m_Width;
  uint32_t m_Height;
};

class UserAction {
public:
  enum class Type { NONE, EXIT, MOVE, CROUCH, STAND, FIRE, MOVE_TARGET};

  UserAction() = default;

  UserAction(Type t) : type(t) {}

  UserAction(Type t, char key) : type(t), Argument{.key = key} {}

  UserAction(Type t, Vec2D<float> v) : type(t), Argument{.position = v} {}

  ~UserAction() = default;

  Type type;

  union {
    Vec2D<float> position;
    char key;
  } Argument;
};

class UserInput {
public:
  UserInput()
      : // pre-alloc some space
        m_Actions(10) {
    LOG_DEBUG(".");
  };

  UserInput(const UserInput &x) = delete;
  UserInput(UserInput &&x) = delete;
  ~UserInput() { LOG_DEBUG("."); };

  std::expected<void, std::string> Init() { return {}; }

  const std::vector<UserAction> &GetActions() {
    m_Actions.clear();
    static Vec2D<float> move_direction = {0.0f, 0.0f};
    static bool send_move_action = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool key_down = event.type == SDL_EVENT_KEY_DOWN ? true : false;
        SDL_KeyboardEvent kbd_event = event.key;
        if (kbd_event.repeat) {
          // SDL repeats KEY_DOWN if key is held down, we ignore that
          continue;
        }
        LOG_DEBUG("Key '", static_cast<char>(kbd_event.key),
                  key_down ? "' down" : "' up");

        switch (kbd_event.key) {
        case 'q':
          m_Actions.emplace_back(UserAction::Type::EXIT);
          // further processing of inputs is not needed
          return m_Actions;
        case 'w':
        case 's':
        case 'a':
        case 'd':
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT: {
          static std::map<char, Vec2D<float>> move_base{
              {'w', {0.0, 1.0}},
              {'s', {0.0, -1.0}},
              {'a', {1.0, 0.0}},
              {'d', {-1.0, 0.0}},
              {static_cast<char>(SDLK_UP), {0.0, 1.0}},
              {static_cast<char>(SDLK_DOWN), {0.0, -1.0}},
              {static_cast<char>(SDLK_LEFT), {1.0, 0.0}},
              {static_cast<char>(SDLK_RIGHT), {-1.0, 0.0}},
          };

          float direction = key_down ? -1.0f : 1.0f;
          send_move_action = true;
          move_direction += move_base[kbd_event.key] * direction;
          break;
        }
        case SDLK_SPACE:
          if (key_down)
            m_Actions.emplace_back(UserAction::Type::FIRE);
          break;
        default:
          LOG_INFO("Key '", static_cast<char>(kbd_event.key), "' not mapped");
          break;
        }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        SDL_MouseButtonEvent mouse_event = event.button;
        LOG_DEBUG("Mouse down: ", mouse_event.x, ", ", mouse_event.y);
        m_Actions.emplace_back(UserAction::Type::MOVE_TARGET,
                               Vec2D<float>{mouse_event.x, mouse_event.y});
      } else {
        // TODO uncomment, for now too much noise
        // LOG_WARNING("Action not processed");
      }
    }
    if (send_move_action) {
      m_Actions.emplace_back(UserAction::Type::MOVE,
                             move_direction.normalized());
      send_move_action = false;
    }
    return m_Actions;
  }

private:
  std::vector<UserAction> m_Actions;
};

//
// Game classes
//

class Entity {
public:
  enum class Type : std::uint8_t {
    NONE,
    PLAYER,
    WALL,
    TILE,
    COUNT // must be last
  };

  Entity(Vec2D<float> position = {0.0f, 0.0f}) : m_Position(position) {
    LOG_DEBUG("spawning entity at position ", position);
  }

  friend std::ostream &operator<<(std::ostream &os, const Entity &obj) {
    static constexpr std::array<std::string_view,
                                static_cast<size_t>(Entity::Type::COUNT)>
        type_name{"NONE", "PLAYER", "WALL", "TILE"};
    size_t idx = static_cast<size_t>(obj.GetType());
    assert(idx < type_name.size());
    os << type_name[idx];
    return os;
  }

  virtual Sprite &GetSprite() = 0;
  virtual constexpr float GetCollisionRadius() const = 0;
  virtual constexpr float GetCollisionRadiusSquared() {
    return GetCollisionRadius() * GetCollisionRadius();
  }

  virtual constexpr Type GetType() const = 0;
  void SetFlagExpired() { m_FlagExpired = true; }
  bool IsFlaggedExpired() { return m_FlagExpired; }

  const Vec2D<float> &GetPosition() const { return m_Position; }
  void SetPosition(Vec2D<float> new_pos) { m_Position = new_pos; }

  const Vec2D<float> &GetActualVelocity() const { return m_ActualVelocity; }
  const Vec2D<float> &GetRequestedVelocity() const {
    return m_RequestedVelocity;
  }
  void SetActualVelocity(const Vec2D<float> &new_velocity) {
    m_ActualVelocity = new_velocity;
  }
  void SetRequestedVelocity(const Vec2D<float> &new_velocity) {
    m_RequestedVelocity = new_velocity;
  }

  void ZeroActualVelocityInDirection(Vec2D<float> direction) {
    // Vectors e1, e2 form the basis for a local coord system,
    // where e1 is formed by the direction where we want to zero-out
    // the velocity, and e2 is the orthogonal vector.
    // Scalars q1, q2 are coordinates for e1, e2 basis.
    Vec2D<float> e1 = direction.normalized();
    Vec2D<float> e2 = e1.orthogonal();

    // q1 * e1 + q2 * e2 = v, from this follows:
    auto &v = GetActualVelocity();
    float q2 = (v.y * e1.x - v.x * e1.y) / (e2.y * e1.x - e2.x * e1.y);
    float q1 = (v.x - q2 * e2.x) / e1.x;

    // We then zero-out the q1, but only if it's positive - meaning
    // it is aiming in the direction of "direction", not out.
    // (otherwise we're not able to move out from collision with
    // another object)
    if (q1 > 0.0f) {
      SetActualVelocity(q2 * e2);
    }
  }

  virtual void Update(float time_delta) {
    m_Position += m_ActualVelocity * time_delta;
  }

  virtual bool IsMovable() const = 0;
  virtual bool IsCollidable() const = 0;

protected:
  Vec2D<float> m_Position;
  Vec2D<float> m_ActualVelocity;
  Vec2D<float> m_RequestedVelocity;

private:
  bool m_FlagExpired = false;
  static constexpr float m_CollisionRadiusSq = 1000.0f;
};

class Wall final : public Entity {
public:
  Wall(Vec2D<float> pos = {0.0f, 0.0f}) : Entity(pos) {
    LOG_DEBUG(".");
    if (m_Sprite == nullptr) {
      LoadResources();
    }
  }
  Wall(const Wall &x) = delete;
  Wall(Wall &&x) = delete;

  Sprite &GetSprite() override {
    assert(m_Sprite != nullptr);
    return *m_Sprite;
  }
  constexpr Entity::Type GetType() const override { return Entity::Type::WALL; }
  constexpr float GetCollisionRadius() const override { return 50.0f; }
  bool IsMovable() const override { return false; }
  bool IsCollidable() const override { return true; }

private:
  void LoadResources() {
    m_Sprite = std::make_unique<Sprite>("resources/wall.png",
                                        Vec2D<float>{50.0f, 50.0f});
  }
  static std::unique_ptr<Sprite> m_Sprite;
};
std::unique_ptr<Sprite> Wall::m_Sprite;

class Player final : public Entity {
public:
  Player() {
    LOG_DEBUG(".");
    if (m_Sprite == nullptr) {
      LoadResources();
    }
  }
  Player(const Player &x) = delete;
  Player(Player &&x) = delete;

  Sprite &GetSprite() override {
    assert(m_Sprite != nullptr);
    return *m_Sprite;
  }

  constexpr Entity::Type GetType() const override {
    return Entity::Type::PLAYER;
  }
  constexpr float GetCollisionRadius() const override { return 50.0f; }
  bool IsMovable() const override { return true; }
  bool IsCollidable() const override { return true; }

private:
  void LoadResources() {
    m_Sprite = std::make_unique<Sprite>("resources/player.png",
                                        Vec2D<float>{38.0f, 46.0f});
  }
  static std::unique_ptr<Sprite> m_Sprite;
};

std::unique_ptr<Sprite> Player::m_Sprite;

using Collision = std::pair<std::shared_ptr<Entity>, std::shared_ptr<Entity>>;

struct Tile {
  float cost;
  uint8_t R, G, B, A;
};

static const std::map<std::string_view, Tile> tile_types = {
    {"Grass", {1.0, 0, 200, 0, 255}},
    {"Mud", {2.0, 100, 100, 100, 255}},
    {"Road", {0.5, 200, 200, 200, 255}},
    {"Water", {10.0, 0, 50, 200, 255}},
};

using TilePos = Vec2D<int>;
using WorldPos = Vec2D<float>;

class Map {

  // TODO using = ... for tile vector

public:
  static constexpr float TILE_SIZE = 100.0f; // tile size in world

  Map(int rows, int cols) : m_Cols(cols), m_Rows(rows) {
    bool sw = true;
    LOG_DEBUG("cols = ", cols, " rows = ", rows);
    m_Tiles = std::vector<std::vector<const Tile *>>{};
    for (size_t row = 0; row < m_Rows; row++) {
      m_Tiles.push_back(std::vector<const Tile *>{});
      for (size_t col = 0; col < m_Cols; col++) {
        if (sw)
          m_Tiles[row].push_back(&tile_types.at("Grass"));
        else
          m_Tiles[row].push_back(&tile_types.at("Water"));
        sw = !sw;
      }
      sw = !sw;
    }
  }
  Map() : Map(0, 0) {}

  const std::vector<std::vector<const Tile *>> &GetMapTiles() const {
    return m_Tiles;
  }

  WorldPos TileToWorld(TilePos p) const {
    return WorldPos{p.x * TILE_SIZE, p.y * TILE_SIZE};
  }

  TilePos WorldToTile(WorldPos p) const {
    return TilePos{p.x / TILE_SIZE, p.y / TILE_SIZE};
  }

  Vec2D<float> GetTileSize() const {
    return Vec2D<float>{TILE_SIZE, TILE_SIZE};
  }

  const Tile* GetTileAt(TilePos p) const {
    assert(IsTilePosValid(p));
    size_t row = p.x;
    size_t col = p.y;

    return m_Tiles[row][col];

  }

  const Tile* GetTileAt(WorldPos p) const {
    return GetTileAt(WorldToTile(p));
  }

  bool IsTilePosValid(TilePos p) const {
    size_t row = p.x;
    size_t col = p.y;

    return row < m_Tiles.size() && col < m_Tiles[0].size();
  }

  template<typename T>
  double GetTileVelocityCoeff(T p) const {
    return 1.0 / GetTileAt(p)->cost;
  }

private:
  // std::vector<std::vector<const Tile*>> m_Tiles;
  std::vector<std::vector<const Tile *>> m_Tiles;
  size_t m_Cols = 0;
  size_t m_Rows = 0;
};

class PathFindingDemo {
public:
  PathFindingDemo(int width, int height)
      : m_Width(width), m_Height(height), // TODO delete width, height
        m_Map(width, height) {
    LOG_DEBUG(".");
  }

  ~PathFindingDemo() { LOG_DEBUG("."); }

  PathFindingDemo(const PathFindingDemo &m) = delete;
  PathFindingDemo(PathFindingDemo &&m) = delete;

  void AddEntity(std::shared_ptr<Entity> e) {
    // TODO emplace_back
    m_Entities.push_back(e);
  }

  void CreateMap() {
    m_Entities.clear();
    m_Player = std::make_shared<Player>();
    m_Player->SetPosition(Vec2D<float>{200.0f, 200.0f});
    m_Entities.push_back(m_Player);
  }

  std::shared_ptr<Player> GetPlayer() { return m_Player; }

  Vec2D<float> GetRandomPosition() const { return Vec2D<float>{0.0, 0.0}; }

  std::vector<std::shared_ptr<Entity>> &GetEntities() { return m_Entities; }

  void UpdateEntities() {
    float time_delta = 1.0f;

    // Remove entities marked as expired and handle expiry logic (e.g. bomb
    // exploding); for all other entities, reset the actual velocity to the
    // requested; actual velocity will be updated later with collisions
    for (auto &entity : m_Entities) {
      if (entity->IsFlaggedExpired()) {
        ExpiryGameLogic(*entity);
        auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
        if (it != m_Entities.end()) {
          std::swap(*it, m_Entities.back());
          m_Entities.pop_back();
        }
      } else {
        // Actual velocity might be changed later by collisions
        auto pos = GetPlayer()->GetPosition();
        double tile_velocity_coeff = m_Map.GetTileVelocityCoeff(pos);
        entity->SetActualVelocity(entity->GetRequestedVelocity() * tile_velocity_coeff);
      }
    }

    // Handle collisions:
    //  - update actual velocity for colliding objects
    //  - modify velocity based on current tile
    //  - handle collision logic
    auto &collisions = GetEntityCollisions();
    //    LOG_DEBUG("number of collisions: ", collisions.size());
    for (auto &collision : collisions) {
      Entity &A = *std::get<0>(collision);
      Entity &B = *std::get<1>(collision);
      if (!A.IsMovable())
        continue;
      // modify actual speed
      // LOG_DEBUG("Collision: A is ", A, ", B is ", B);
      Vec2D<float> AB = B.GetPosition() - A.GetPosition();
      A.ZeroActualVelocityInDirection(AB);
      // handle logic
      CollisionGameLogic(A, B);
    }

    // Update entities: this advances animations,
    // internal timers, and updates positions (with velocity
    // modified by the collision handling)
    for (auto &entity : m_Entities) {
      entity->Update(time_delta);
    }

  }

  void CollisionGameLogic(Entity &A, Entity &B) {
    // not used for path finding demo
  }

  void ExpiryGameLogic(Entity &entity) {
    // not used for path finding demo
  }

  const std::vector<Collision> &GetEntityCollisions() {
    static std::vector<Collision> m_Collisions;

    m_Collisions.clear();
    for (const auto &entity_A : m_Entities) {
      for (const auto &entity_B : m_Entities) {
        if (entity_A == entity_B)
          continue;
        if (!entity_A->IsCollidable() || !entity_B->IsCollidable())
          continue;
        // check distance of player to given entity
        auto position_A = entity_A->GetPosition();
        auto position_B = entity_B->GetPosition();
        auto distance_sq = position_A.distance_squared(position_B);
        auto collision_distance_sq =
            entity_A->GetCollisionRadiusSquared() +
            entity_B->GetCollisionRadiusSquared() +
            2 * entity_A->GetCollisionRadius() * entity_B->GetCollisionRadius();
        // TODO use vector instructions
        if (distance_sq < collision_distance_sq) {
          m_Collisions.emplace_back(Collision(entity_A, entity_B));
        }
      }
    }
    return m_Collisions;
  }

  void HandleActions(const std::vector<UserAction> &actions) {
    for (const auto &action : actions) {
      if (action.type == UserAction::Type::EXIT) {
        LOG_INFO("Exit requested");
        m_ExitRequested = true;
      } else if (action.type == UserAction::Type::FIRE) {
        LOG_INFO("Fire");
        // AddEntity(m_Player->CreateBomb());
      } else if (action.type == UserAction::Type::MOVE) {
        LOG_INFO("Move direction ", action.Argument.position);
        m_Player->SetRequestedVelocity(action.Argument.position * 4.0f);
      } else if (action.type == UserAction::Type::MOVE_TARGET) {
        TilePos p = m_Map.WorldToTile(action.Argument.position);
        LOG_INFO("Move target: ", action.Argument.position, ", tile pos: ", p);
        // TODO move
      }
    };
  }

  const Map &GetMap() const { return m_Map; }

  bool IsCollisionBoxVisible() const { return m_DrawCollisionBox; }
  bool IsExitRequested() const { return m_ExitRequested; }

private:
  int m_Width;
  int m_Height;
  bool m_ExitRequested = false;
  bool m_DrawCollisionBox = true;
  std::vector<std::shared_ptr<Entity>> m_Entities;
  std::shared_ptr<Player> m_Player;
  Map m_Map;
};

// GameLoop class handles user input and audio/video output,
// client side only. No game logic should be handled here.

class GameLoop {
public:
  GameLoop() = default;

  void Run() {
    LOG_INFO("Running the game");
    while (!m_Game->IsExitRequested()) {
      m_Game->HandleActions(m_UserInput->GetActions());
      m_Game->UpdateEntities();

      m_Window->ClearWindow();

      // draw the map (terrain tiles)
      const Map &map = m_Game->GetMap();
      const auto &tiles = map.GetMapTiles();
      for (size_t row = 0; row < tiles.size(); row++) {
        for (size_t col = 0; col < tiles[row].size(); col++) {
          // LOG_DEBUG("Drawing rect (", row, ", ", col, ")");
          m_Window->DrawRect(
              map.TileToWorld(TilePos{row, col}),
              map.GetTileSize(), tiles[row][col]->R, tiles[row][col]->G,
              tiles[row][col]->B, tiles[row][col]->A);
        }
      }

      // draw all the entities (player etc)
      for (auto &entity : m_Game->GetEntities()) {
        m_Window->DrawSprite(entity->GetPosition(), entity->GetSprite());
        if (m_Game->IsCollisionBoxVisible()) {
          m_Window->DrawCircle(entity->GetPosition(),
                               entity->GetCollisionRadius());
        }
      }

      m_Window->Flush();
      // TODO measure fps
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
  }

  inline void SetGame(std::unique_ptr<PathFindingDemo> x) {
    m_Game = std::move(x);
  }
  inline void SetWindow(std::unique_ptr<Window> x) { m_Window = std::move(x); }
  inline void SetUserInput(std::unique_ptr<UserInput> x) {
    m_UserInput = std::move(x);
  }
  inline void SetAudioOutput(std::unique_ptr<AudioOutput> x) {
    m_AudioOutput = std::move(x);
  }

private:
  std::unique_ptr<PathFindingDemo> m_Game;
  std::unique_ptr<Window> m_Window;
  std::unique_ptr<UserInput> m_UserInput;
  std::unique_ptr<AudioOutput> m_AudioOutput;
};

int main(int argc, char **argv) {
  constexpr int error = -1;

  /*
   * Initialize the input/output system
   */

  auto window = std::make_unique<Window>(640, 480); // the holy resolution
  // auto window_init = window->Init();
  if (auto initialized = window->Init(); !initialized) {
    LOG_ERROR(initialized.error());
    return error;
  }

  auto user_input = std::make_unique<UserInput>();
  if (auto initialized = user_input->Init(); !initialized) {
    LOG_ERROR(initialized.error());
    return error;
  }

  auto audio_output = std::make_unique<AudioOutput>();
  if (auto initialized = audio_output->Init(); !initialized) {
    LOG_ERROR(initialized.error());
    return error;
  }

  /*
   * Initialize the map and run the pathfinding demo
   */

  auto demo = std::make_unique<PathFindingDemo>(10, 10);
  demo->CreateMap();

  auto game_loop = GameLoop{};
  game_loop.SetWindow(std::move(window));
  game_loop.SetUserInput(std::move(user_input));
  game_loop.SetAudioOutput(std::move(audio_output));
  game_loop.SetGame(std::move(demo));
  game_loop.Run();
}
