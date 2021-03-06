//----------------------------------*-C++-*----------------------------------//
/*!
 * \file   cdi_analytic/Pseudo_Line_Analytic_MultigroupOpacity.cc
 * \author Kent G. Budge
 * \date   Tue Apr  5 08:42:25 MDT 2011
 * \note   Copyright (C) 2016-2018 Los Alamos National Security, LLC.
 *         All rights reserved. */
//---------------------------------------------------------------------------//

#include "Pseudo_Line_Analytic_MultigroupOpacity.hh"
#include "cdi/CDI.hh"
#include "ds++/DracoMath.hh"
#include "ds++/Packing_Utils.hh"
#include "ode/quad.hh"
#include "ode/rkqs.hh"

namespace rtt_cdi_analytic {
using namespace std;
using namespace rtt_ode;
using namespace rtt_dsxx;
using namespace rtt_cdi;

typedef Analytic_MultigroupOpacity::sf_double sf_double;
typedef Analytic_MultigroupOpacity::vf_double vf_double;

//---------------------------------------------------------------------------//
class PLP_Functor {
public:
  typedef double return_type;

  PLP_Functor(Pseudo_Line_Base const *ptr, double const T) : ptr_(ptr), T_(T) {}

  double operator()(double x);

private:
  Pseudo_Line_Base const *ptr_;
  double T_;
};

double PLP_Functor::operator()(double x) {
  return ptr_->monoOpacity(x, T_) * Pseudo_Line_Base::BB(T_, x);
}

//---------------------------------------------------------------------------//
class PLPW_Functor {
public:
  typedef double return_type;

  PLPW_Functor(double const T) : T_(T) {}

  double operator()(double x);

private:
  double T_;
};

double PLPW_Functor::operator()(double x) {
  return Pseudo_Line_Base::BB(T_, x);
}

//---------------------------------------------------------------------------//
class PLR_Functor {
public:
  typedef double return_type;

  PLR_Functor(Pseudo_Line_Base const *ptr, double const T) : ptr_(ptr), T_(T) {}

  double operator()(double x);

private:
  Pseudo_Line_Base const *ptr_;
  double T_;
};

double PLR_Functor::operator()(double x) {
  return Pseudo_Line_Base::DBB(T_, x) / ptr_->monoOpacity(x, T_);
}

//---------------------------------------------------------------------------//
class PLRW_Functor {
public:
  typedef double return_type;

  PLRW_Functor(double const T) : T_(T) {}

  double operator()(double x);

private:
  double T_;
};

double PLRW_Functor::operator()(double x) {
  return Pseudo_Line_Base::DBB(T_, x);
}

//---------------------------------------------------------------------------//
Pseudo_Line_Analytic_MultigroupOpacity::Pseudo_Line_Analytic_MultigroupOpacity(
    sf_double const &group_bounds, rtt_cdi::Reaction const reaction,
    std::shared_ptr<Expression const> const &continuum,
    unsigned number_of_lines, double line_peak, double line_width,
    unsigned number_of_edges, double edge_ratio, double Tref, double Tpow,
    double emin, double emax, Averaging const averaging, unsigned const qpoints,
    unsigned seed)
    : Analytic_MultigroupOpacity(group_bounds, reaction),
      Pseudo_Line_Base(continuum, number_of_lines, line_peak, line_width,
                       number_of_edges, edge_ratio, Tref, Tpow, emin, emax,
                       seed),
      averaging_(averaging), qpoints_(qpoints) {}

//---------------------------------------------------------------------------//
// Packing function

Analytic_MultigroupOpacity::sf_char
Pseudo_Line_Analytic_MultigroupOpacity::pack() const {
  sf_char const pdata = Analytic_MultigroupOpacity::pack();
  sf_char const pdata2 = Pseudo_Line_Base::pack();

  sf_char Result(pdata.size() + pdata2.size());
  copy(pdata.begin(), pdata.end(), Result.begin());
  copy(pdata2.begin(), pdata2.end(), Result.begin() + pdata.size());
  return pdata;
}

//---------------------------------------------------------------------------//
sf_double
Pseudo_Line_Analytic_MultigroupOpacity::getOpacity(double T,
                                                   double /*rho*/) const {
  sf_double const &group_bounds = this->getGroupBoundaries();
  unsigned const number_of_groups = group_bounds.size() - 1U;
  sf_double Result(number_of_groups, 0.0);

  double line_width = this->line_width();

  switch (averaging_) {
  case NONE: {
    double g1 = group_bounds[0];
    for (unsigned g = 0; g < number_of_groups; ++g) {
      double const g0 = g1;
      g1 = group_bounds[g + 1];
      double const nu = 0.5 * (g0 + g1);
      Result[g] = monoOpacity(nu, T);
    }
  } break;

  case ROSSELAND: {
    PLR_Functor rfunctor(this, T);
    PLRW_Functor wfunctor(T);

    double g1 = group_bounds[0];
    for (unsigned g = 0; g < number_of_groups; ++g) {
      double const g0 = g1;
      g1 = group_bounds[g + 1];
      double eps = 1e-5;
      double t = 0.0, b = 0.0;
      if (qpoints_ == 0) {
        double x1 = g0;
        while (x1 < g1) {
          double x0 = x1;
          x1 += 2 * line_width;
          if (x1 > g1)
            x1 = g1;

          t += rtt_ode::quad(rfunctor, x0, x1, eps,
                             rkqs<double, Quad_To_ODE<PLR_Functor>>);
          b += rtt_ode::quad(wfunctor, x0, x1, eps,
                             rkqs<double, Quad_To_ODE<PLRW_Functor>>);
        }
      } else {
        for (unsigned ig = 0; ig < qpoints_; ++ig) {
          double const x = (ig + 0.5) * (g1 - g0) / qpoints_ + g0;

          double w = CDI::integrateRosselandSpectrum(g0, g1, T);

          t += w / monoOpacity(x, T);
          b += w;
        }
      }

      Result[g] = b / t;
    }
  } break;

  case PLANCK: {
    double g1 = group_bounds[0];
    for (unsigned g = 0; g < number_of_groups; ++g) {
      PLP_Functor pfunctor(this, T);
      PLPW_Functor wfunctor(T);

      double const g0 = g1;
      g1 = group_bounds[g + 1];
      double eps = 1e-5;
      double t = 0.0, b = 0.0;
      if (qpoints_ == 0) {
        double x1 = g0;
        while (x1 < g1) {
          double x0 = x1;
          x1 += 2 * line_width;
          if (x1 > g1)
            x1 = g1;

          t += rtt_ode::quad(pfunctor, x0, x1, eps,
                             rkqs<double, Quad_To_ODE<PLP_Functor>>);
          b += rtt_ode::quad(wfunctor, x0, x1, eps,
                             rkqs<double, Quad_To_ODE<PLPW_Functor>>);
        }
      } else {
        for (unsigned ig = 0; ig < qpoints_; ++ig) {
          double const x = (ig + 0.5) * (g1 - g0) / qpoints_ + g0;

          double w = CDI::integratePlanckSpectrum(g0, g1, T);

          t += w * monoOpacity(x, T);
          b += w;
        }
      }

      Result[g] = t / b;
    }
  } break;

  default:
    Insist(false, "bad case");
  }

  return Result;
}

//---------------------------------------------------------------------------//
vf_double Pseudo_Line_Analytic_MultigroupOpacity::getOpacity(sf_double const &T,
                                                             double rho) const {
  unsigned const n = T.size();
  vf_double Result(n);

  for (unsigned i = 0; i < n; ++i) {
    double const Ti = T[i];
    Result[i] = getOpacity(Ti, rho);
  }
  return Result;
}

//---------------------------------------------------------------------------//
vf_double
Pseudo_Line_Analytic_MultigroupOpacity::getOpacity(double const T,
                                                   sf_double const &rho) const {
  return vf_double(rho.size(), getOpacity(T, rho[0]));
}

//---------------------------------------------------------------------------//
Pseudo_Line_Analytic_MultigroupOpacity::std_string
Pseudo_Line_Analytic_MultigroupOpacity::getDataDescriptor() const {
  std_string descriptor;

  rtt_cdi::Reaction const reaction = getReactionType();

  if (reaction == rtt_cdi::TOTAL)
    descriptor = "Pseudo Line Multigroup Total";
  else if (reaction == rtt_cdi::ABSORPTION)
    descriptor = "Pseudo Line Multigroup Absorption";
  else if (reaction == rtt_cdi::SCATTERING)
    descriptor = "Pseudo Line Multigroup Scattering";
  else {
    Insist(0, "Invalid Pseudo Line multigroup model opacity!");
  }

  return descriptor;
}

} // end namespace rtt_cdi_analytic

//---------------------------------------------------------------------------//
// end of Pseudo_Line_Analytic_MultigroupOpacity.cc
//---------------------------------------------------------------------------//
