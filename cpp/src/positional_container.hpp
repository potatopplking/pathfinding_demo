#pragma once

#include <vector>
#include <memory>
#include <cstdlib>

#include "math.hpp"
#include "log.hpp"

template <typename T>
concept HasPosition = requires(T t, WorldPos pos) {
  { t.GetPosition() } -> std::convertible_to<WorldPos>;
  t.SetPosition(pos);
};

template <typename T>
concept HasCollisions = requires(T t) {
  t.Dummy(); // TODO
};

template <typename T>
requires HasPosition<T>
class IPositionalContainer
{
public:
  virtual ~IPositionalContainer() = default;
  virtual bool Add(const T& t) = 0;
  virtual std::vector<std::weak_ptr<T>> Get(const WorldPos& p, float radius) = 0;
  virtual void UpdateAll() = 0;
  virtual void Update(std::shared_ptr<T> item) = 0;
};


template <typename T>
class IColliderContainer : public IPositionalContainer<T>
{
public:
  virtual std::vector<std::weak_ptr<T>> GetCollisions() = 0;
};

template <typename T>
class SimpleContainer : IPositionalContainer<T>
{
public:
  bool Add(const T& t) override
  {
    m_Items.push_back(std::make_shared<T>(t));
  }
  
  std::vector<std::weak_ptr<T>> Get(const WorldPos& center, float radius) override
  {
    std::vector<std::weak_ptr<T>> matched_items;
    for (const auto& item : m_Items)
    {
      const auto& A = item->GetPosition();
      if (center.DistanceTo(item->GetPosition()) < radius)
      {
        matched_items.push_back(item);
      }
    }
    return matched_items;
  }
  
  // no update needed here, as we have no smart lookup scheme
  void UpdateAll() override {}
  void Update(std::shared_ptr<T> item) override {}

private:
  std::vector<std::shared_ptr<T>> m_Items;
};


template <class T>
class PositionalContainer : IPositionalContainer<T>
{
public:

  PositionalContainer(const WorldSize& size, size_t chunks) :
    m_GridSize{size},
    m_GridStep{size / chunks},
    m_ChunksPerAxis{chunks}
  {
    LOG_INFO("Size: ", m_GridSize, " step: ", m_GridStep);
    m_Grid.reserve(chunks);
    for (size_t i = 0; i < chunks; i++)
    {
      //m_Grid.push_back(std::vector<vector_wptr>{chunks});
      m_Grid.emplace_back(chunks);
      for (size_t j = 0; j < chunks; j++)
      {
        // let's try reserving place for 16 items
        m_Grid[i][j].reserve(16);
      }
    }
  }

  // calling Add on object that is already in the container is UB
  bool Add(const T& item) override
  {
    const auto& world_pos = item.GetPosition();
    if (!CheckBounds(world_pos))
    {
      return false;
    }
    auto ptr = std::make_shared<T>(item);
    m_Items.push_back(ptr);
    auto coords = GetCoords(world_pos);
    // here we assume that the object is not already in the grid!
    m_Grid[coords.x()][coords.y()].push_back(ptr);

    // TODO should we call Update instead?
    //Update(ptr);
  }
  
  std::vector<std::weak_ptr<T>> Get(const WorldPos& center, float radius) override
  {
    return vector_wptr{};
  }

  void Get(std::vector<std::weak_ptr<T>>& output_vec, const WorldPos& center, float radius)
  {

  }

  void UpdateAll() override
  {
    for (auto ptr : m_Items)
    {
      // TODO is this efficient? Maybe use const ref?
      Update(ptr);
    }
  }
  void Update(std::shared_ptr<T> item) override
  {
    auto coords = GetCoords(item->GetPosition());
    // remove the old weak ptr from the map
    
    // add new weak ptr to the map
    m_Grid[coords.x()][coords.y()].push_back(item);
  }

private:
  
  using coord_type = vec<size_t, 2>;
  using vector_wptr = std::vector<std::weak_ptr<T>>;
  using grid_type = std::vector<std::vector<vector_wptr>>;

  coord_type GetCoords(const WorldPos &wp)
  {
    auto coord_float = wp / m_ChunksPerAxis;
    return coord_type{
      static_cast<size_t>(coord_float.x()),
      static_cast<size_t>(coord_float.y())
    };
  }

  bool CheckBounds(const WorldPos& pos)
  {
    auto [x,y] = pos;
    bool x_in_bounds = 0.0f < x && x < m_GridSize.x(); 
    bool y_in_bounds = 0.0f < y && y < m_GridSize.y(); 
    return x_in_bounds && y_in_bounds;
  }

  WorldSize m_GridSize;
  WorldSize m_GridStep;
  size_t m_ChunksPerAxis;
  std::vector<std::shared_ptr<T>> m_Items;
  

  grid_type m_Grid;
};
