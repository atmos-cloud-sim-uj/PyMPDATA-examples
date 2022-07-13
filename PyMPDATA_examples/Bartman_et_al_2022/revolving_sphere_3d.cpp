/**
 * based on tests:tests/paper_2015_GMD/4_revolving_sphere_3d/revolving_sphere_3d.cpp
 * @copyright University of Warsaw
 * @section LICENSE
 * GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 */

#include <boost/math/constants/constants.hpp>
using boost::math::constants::pi;

#include <libmpdata++/solvers/mpdata.hpp>
#include <libmpdata++/concurr/threads.hpp>
using namespace libmpdataxx;

void test(const double dt, const int nx, const int nt, const int n_iters)
{
  enum {x, y, z};
  struct ct_params_t : ct_params_default_t
  {
    using real_t = double;
    enum { n_dims = 3 };
    enum { n_eqns = 1 };
    enum { opts = 0 };
  };

  using slv_t = solvers::mpdata<ct_params_t>;
  typename slv_t::rt_params_t p;

  // pre instantation
  p.n_iters = n_iters;
  p.grid_size = {nx, nx, nx};

  // post instantation
  const typename ct_params_t::real_t
    L = 100,
    dx = L / (nx - 1),
    dy = dx,
    dz = dx,
    h = 4,
    r = 15,
    d = 25 / sqrt(3),
    x0 = 50 - d,
    y0 = 50 + d,
    z0 = 50 + d;
  
  p.di = dx;
  p.dj = dy;
  p.dk = dz;
  p.dt = dt;

  // instantation
  concurr::threads<
    slv_t,
    bcond::open, bcond::open,
    bcond::open, bcond::open,
    bcond::open, bcond::open
  > slv(p);

  blitz::firstIndex i;
  blitz::secondIndex j;
  blitz::thirdIndex k;

  // sphere shape
  decltype(slv.advectee()) tmp(slv.advectee().extent());
  tmp.reindexSelf(slv.advectee().base());
  tmp =   blitz::pow(i * dx - x0, 2)
        + blitz::pow(j * dx - y0, 2)
        + blitz::pow(k * dx - z0, 2);
  slv.advectee() = where(tmp - pow(r, 2) <= 0, h, 0);

  const typename ct_params_t::real_t
    omega = 0.1,
    xc = 50,
    yc = 50,
    zc = 50;

  // constant angular velocity rotational field
  slv.advector(x) = omega / sqrt(3) * (-(j * dy - yc) + (k * dz - zc)) * dt / dx;
  slv.advector(y) = omega / sqrt(3) * ( (i * dx - xc) - (k * dz - zc)) * dt / dy;
  slv.advector(z) = omega / sqrt(3) * (-(i * dx - xc) + (j * dy - yc)) * dt / dz;

  // time stepping
  slv.advance(nt);
}

int main(int argc, char **argv)
{
  assert(argc == 5);
  test(
    atof(std::getenv("DT")),
    atoi(std::getenv("NX")),
    atoi(std::getenv("N_STEPS")),
    atoi(std::getenv("N_ITERS"))
  );
}
