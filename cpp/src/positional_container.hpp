#pragma once

#include <vector>
#include <memory>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iterator>

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
  virtual bool Add(std::shared_ptr<T> t) = 0;
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
  bool Add(std::shared_ptr<T> t) override
  {
    m_Items.push_back(t);
    return true;
  }
  
  std::vector<std::weak_ptr<T>> Get(const WorldPos& center, float radius) override
  {
    std::vector<std::weak_ptr<T>> matched_items;
    for (const auto& item : m_Items)
    {
      if (center.DistanceTo(item->GetPosition()) < radius)
      {
        matched_items.push_back(item);
      }
    }
    return matched_items;
  }
  
  // no update needed here, as we have no smart lookup scheme
  void UpdateAll() override {}
  void Update(std::shared_ptr<T>) override {}

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
      m_Grid.emplace_back(chunks);
      for (size_t j = 0; j < chunks; j++)
      {
        m_Grid[i][j].reserve(16);
      }
    }
  }

  // calling Add on object that is already in the container is UB
  bool Add(std::shared_ptr<T> item) override
  {
    const auto& world_pos = item->GetPosition();
    if (!CheckBounds(world_pos))
    {
      return false;
    }
    m_Items.push_back(item);
    auto coords = GetCoords(world_pos);
    m_Grid[coords.x()][coords.y()].push_back(item);
    m_ReverseGridLookup[item] = coords;
    // TODO should we call Update instead?
    //Update(item);
    return true;
  }
  
  std::vector<std::weak_ptr<T>> Get(const WorldPos& center, float radius) override
  {
    vector_wptr output_vec{};

    Get(output_vec, center, radius);
    
    return output_vec;
  }

  void Get(std::vector<std::weak_ptr<T>>& output_vec, const WorldPos& corner, const WorldSize& size)
  {
    const WorldSize half_size = size / 2.0f;
    const WorldPos center =  corner + half_size.ChangeTag<WorldPos>();
    float radius = half_size.x();
    Get(output_vec, center, radius);
  }

  // TODO add those Get methods to the interface

  void Get(std::vector<std::weak_ptr<T>>& output_vec, const WorldPos& center, float radius)
  {
    output_vec.clear();
    const WorldPos corner_1 = center + radius; 
    const WorldPos corner_2 = center - radius; 

    if (!CheckBounds(corner_1) || !CheckBounds(corner_2))
    {
      return;
    }

    const auto A = GetCoords(corner_1);
    const auto B = GetCoords(corner_2);

    auto [x_min_f, x_max_f] = std::minmax(A.x(), B.x());
    auto [y_min_f, y_max_f] = std::minmax(A.y(), B.y());

    size_t x_min = static_cast<size_t>(std::floor(x_min_f));
    size_t x_max = static_cast<size_t>(std::ceil(x_max_f));
    size_t y_min = static_cast<size_t>(std::floor(y_min_f));
    size_t y_max = static_cast<size_t>(std::ceil(y_max_f));

    // TODO this goes through more positions than we need
    for (size_t x = x_min; x <= x_max; x++)
    {
      for (size_t y = y_min; y <= y_max; y++)
      {
        std::ranges::copy(m_Grid[x][y], std::back_inserter(output_vec));
      }
    }
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
    coord_type current_coords = GetCoords(item->GetPosition());
    coord_type last_known_coords = m_ReverseGridLookup[item]; 
    if (current_coords == last_known_coords)
    {
      return;
    }
    vector_wptr& vec = m_Grid[last_known_coords.x()][last_known_coords.y()];
    // remove the old weak ptr from the map
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                   [&](const std::weak_ptr<T>& w)
                   {
                       return !w.owner_before(item) && !item.owner_before(w);
                   }),
    vec.end());
    // add new weak ptr to the map
    m_Grid[current_coords.x()][current_coords.y()].push_back(item);
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
  // TODO it would be better to have vector<T> - contiguous memory, more cache-friendly?
  std::vector<std::shared_ptr<T>> m_Items;
  grid_type m_Grid;

  // normal  lookup: WorldPos -> coord_type -> vector_wptr -> std::shared_ptr<T>
  // reverse lookup: std::shared_ptr<T> -> vector_wptr -> coord_type
  // we need the reverse lookup because T.GetPosition() may change and we need to delete
  // the old weak_ptr from vector_wptr (without iterating through all of them).
  // Also it might be useful to have T -> location lookup
  // Note: hash of std::shared_ptr<T> may give us trouble if we free the memory and new one points
  // to the same location, maybe it would be better to hash the object itself?
  std::unordered_map<std::shared_ptr<T>, coord_type> m_ReverseGridLookup;
};
