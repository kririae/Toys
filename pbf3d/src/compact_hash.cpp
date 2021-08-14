//
// Created by kr2 on 8/10/21.
//

// currently `index sort`

#include "compact_hash.hpp"
#include "common.hpp"
#include "omp.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <parallel/algorithm>

CompactHash::CompactHash(float _radius)
    : radius(_radius), n_grids(glm::ceil(2 * border / radius) + 1)
{
  hash_map = std::vector<std::vector<int>>(n_grids * n_grids * n_grids);
}

void CompactHash::add_particle(const SPHParticle &p)
{
  data.push_back(p);
}

std::vector<SPHParticle> &CompactHash::get_data()
{
  return data;
}

int CompactHash::n_points() const
{
  return data.size();
}

int CompactHash::n_neighbor(uint index) const
{
  return neighbor_map[index].size();
}

int CompactHash::neighbor(uint index, uint neighbor_index) const
{
  return neighbor_map[index][neighbor_index];
}

void CompactHash::build()
{
  neighbor_map = std::vector<std::vector<uint>>(n_points());
  const int data_size = data.size();

  // Clear previous information
  for (auto &i : hash_map)
    i.clear();

  // Initialize the hash_map
  for (int i = 0; i < data_size; ++i) {
    const int hash_map_index = hash(data[i].pos);
    hash_map[hash_map_index].push_back(i);
  }

#pragma omp parallel for default(none) shared(data_size, n_grids)
  for (int i = 0; i < data_size; ++i) {
    const auto &center = data[i];
    const auto &grid_index = get_grid_index(center.pos);

    for (int u = grid_index.x - 1; u <= grid_index.x + 1; ++u) {
      for (int v = grid_index.y - 1; v <= grid_index.y + 1; ++v) {
        for (int w = grid_index.z - 1; w <= grid_index.z + 1; ++w) {
          if (u < 0 || v < 0 || w < 0 || u >= n_grids || v >= n_grids ||
              w >= n_grids)  // TODO: implement AABB
            continue;

          const int _hash_index = hash_from_grid(u, v, w);
          const std::vector<int> &map_item = hash_map[_hash_index];
          std::for_each(map_item.cbegin(), map_item.cend(), [&](int j) {
            if (center.dist(data[j]) <= radius)
              neighbor_map[i].push_back(j);
          });
        }
      }
    }
  }

}

inline int CompactHash::hash(float x, float y, float z) const
{
  const auto &grid_index = get_grid_index(glm::vec3(x, y, z));
  return hash_from_grid(grid_index);
}

inline int CompactHash::hash(const glm::vec3 &p)
{
  return hash(p.x, p.y, p.z);
}

inline glm::ivec3 CompactHash::get_grid_index(const glm::vec3 &p) const
{
  int u = (int)(glm::floor((p.x + border) / radius));
  int v = (int)(glm::floor((p.y + border) / radius));
  int w = (int)(glm::floor((p.z + border) / radius));
  return {u, v, w};
}

inline int CompactHash::hash_from_grid(int u, int v, int w) const
{
  return u + v * n_grids + w * n_grids * n_grids;
}

inline int CompactHash::hash_from_grid(const glm::ivec3 &p) const
{
  return hash_from_grid(p.x, p.y, p.z);
}

std::vector<uint> &CompactHash::neighbor_vec(uint index)
{
  // the result should not be changed
  return neighbor_map[index];
}
