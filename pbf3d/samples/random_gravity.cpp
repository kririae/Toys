//
// Created by kr2 on 8/11/21.
//

#include "gui.hpp"
#include "particle.hpp"
#include "pbd.hpp"
#include <iostream>
#include <random>

constexpr int NParticles = 4000;
constexpr int WIDTH = 800, HEIGHT = 600;
constexpr float radius = 1;

[[maybe_unused]] Random rd_global;

int main()
{
  // random generator initialization

  RTGUI_particles gui(WIDTH, HEIGHT);
  PBDSolver pbd(radius);
  pbd.set_gui(&gui);
  std::function<void()> callback = [obj = &pbd] { obj->callback(); };

  // for (int i = 0; i < NParticles; ++i) {
  //   pbd.add_particle(SPHParticle(rd_global.rand() * 0.7,
  //                                rd_global.rand() * 0.7,
  //                                rd_global.rand() * 0.7));
  // }
  //
  // std::cout << "NParticles: " << NParticles << std::endl;
  const int _range = border / 1.5;
  int cnt = 0;
  for (float x = -_range; x <= _range; x += radius) {
    for (float y = -_range; y <= _range; y += radius) {
      for (float z = -_range; z <= _range; z += radius) {
        ++cnt;
        pbd.add_particle(SPHParticle(x, y, z));
      }
    }
  }
  std::cout << "NParticles: " << cnt << std::endl;

  gui.main_loop(callback);
  gui.del();

  return 0;
}
