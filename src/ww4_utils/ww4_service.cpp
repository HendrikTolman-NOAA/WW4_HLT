/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.cpp
 * @brief Common mathematical and physical constants for WAVEWATCH IV.
 * @details This source file defines derived constants that cannot be evaluated
 *          using constexpr.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @date Initial, 2026-04-09
 */

#include "ww4_utils/ww4_service.hpp"
#include <algorithm>
#include <cmath>

namespace ww4_utils {

namespace constants {

/** @brief Square root of gravity.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
const double SQRTG = std::sqrt(GRAV);

/**
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @details Converted from WW3 routine KZEONE. Original source: ACM.
 */
void kzeone(double x, double y, double &re0, double &im0, double &re1,
            double &im1) noexcept {
  static constexpr double exsq[8] = {
      0.5641003087264,    0.4120286874989,    0.1584889157959,
      0.3078003387255e-1, 0.2778068842913e-2, 0.1000044412325e-3,
      0.1059115547711e-5, 0.1522475804254e-8};
  static constexpr double tsq[8] = {0.0,
                                    3.19303633920635e-1,
                                    1.29075862295915,
                                    2.95837445869665,
                                    5.40903159724444,
                                    8.80407957805676,
                                    1.34685357432515,
                                    2.02499163658709};

  double r2_original = x * x + y * y;
  if (r2_original >= 1.96e2) {
    double rterm = 1.0;
    double iterm = 0.0;
    re0 = 1.0;
    im0 = 0.0;
    re1 = 1.0;
    im1 = 0.0;
    double p1 = 8.0 * r2_original;
    double p2 = std::sqrt(r2_original);
    double l = 3.91 + 8.12e1 / p2;
    double r1 = 1.0;
    double r2 = 1.0;
    int m = -8;
    int k = 3;
    for (int n = 1; n <= static_cast<int>(l); ++n) {
      m += 8;
      k -= m;
      r1 *= static_cast<double>(k - 4);
      r2 *= static_cast<double>(k);
      double t1 = static_cast<double>(n) * p1;
      double t2 = rterm;
      rterm = (t2 * x + iterm * y) / t1;
      iterm = (-t2 * y + iterm * x) / t1;
      re0 += r1 * rterm;
      im0 += r1 * iterm;
      re1 += r2 * rterm;
      im1 += r2 * iterm;
    }
    double t1 = std::sqrt(p2 + x);
    double t2 = -y / t1;
    p1 = 8.86226925452758e-1 / p2;
    double rterm_local = p1 * std::cos(y);
    double iterm_local = -p1 * std::sin(y);
    r1 = re0 * rterm_local - im0 * iterm_local;
    r2 = re0 * iterm_local + im0 * rterm_local;
    re0 = t1 * r1 - t2 * r2;
    im0 = t1 * r2 + t2 * r1;
    r1 = re1 * rterm_local - im1 * iterm_local;
    r2 = re1 * iterm_local + im1 * rterm_local;
    re1 = t1 * r1 - t2 * r2;
    im1 = t1 * r2 + t2 * r1;
  } else if (r2_original >= 1.849e1) {
    double x2 = 2.0 * x;
    double y2 = 2.0 * y;
    double r1 = y2 * y2;
    double p1 = std::sqrt(x2 * x2 + r1);
    double p2 = std::sqrt(p1 + x2);
    double t1 = exsq[0] / (2.0 * p1);
    re0 = t1 * p2;
    im0 = t1 / p2;
    re1 = 0.0;
    im1 = 0.0;
    for (int n = 1; n < 8; ++n) {
      double t2 = x2 + tsq[n];
      p1 = std::sqrt(t2 * t2 + r1);
      p2 = std::sqrt(p1 + t2);
      t1 = exsq[n] / p1;
      re0 += t1 * p2;
      im0 += t1 / p2;
      t1 = exsq[n] * tsq[n];
      re1 += t1 * p2;
      im1 += t1 / p2;
    }
    double t2 = -y2 * im0;
    double re1_val = re1 / r2_original;
    double im1_val = y2 * im1 / r2_original;
    double rterm_local = SQRT2 * std::cos(y);
    double iterm_local = -SQRT2 * std::sin(y);
    im0 = re0 * iterm_local + t2 * rterm_local;
    re0 = re0 * rterm_local - t2 * iterm_local;
    double t1_local = re1_val * rterm_local - im1_val * iterm_local;
    double t2_local = re1_val * iterm_local + im1_val * rterm_local;
    re1 = t1_local * x + t2_local * y;
    im1 = -t1_local * y + t2_local * x;
  } else {
    double x2 = x / 2.0;
    double y2 = y / 2.0;
    double p1 = x2 * x2;
    double p2 = y2 * y2;
    double t1 = -(std::log(p1 + p2) / 2.0 + 0.5772156649015329);
    double t2 = -std::atan2(y, x);
    x2 = p1 - p2;
    y2 = x * y2;
    double rterm = 1.0;
    double iterm = 0.0;
    re0 = t1;
    im0 = t2;
    t1 += 0.5;
    re1 = t1;
    im1 = t2;
    p2 = std::sqrt(r2_original);
    double l = 2.106 * p2 + 4.4;
    if (p2 < 0.8)
      l = 2.129 * p2 + 4.0;
    for (int n = 1; n <= static_cast<int>(l); ++n) {
      double p1_local = static_cast<double>(n);
      double p2_local = static_cast<double>(n * n);
      double r1 = rterm;
      rterm = (r1 * x2 - iterm * y2) / p2_local;
      iterm = (r1 * y2 + iterm * x2) / p2_local;
      t1 += 0.5 / p1_local;
      re0 += t1 * rterm - t2 * iterm;
      im0 += t1 * iterm + t2 * rterm;
      p1_local += 1.0;
      t1 += 0.5 / p1_local;
      re1 += (t1 * rterm - t2 * iterm) / p1_local;
      im1 += (t1 * iterm + t2 * rterm) / p1_local;
    }
    double r1 = x / r2_original - 0.5 * (x * re1 - y * im1);
    double r2_local = -y / r2_original - 0.5 * (x * im1 + y * re1);
    p1 = std::exp(x);
    re0 = p1 * re0;
    im0 = p1 * im0;
    re1 = p1 * r1;
    im1 = p1 * r2_local;
  }
}

/**
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @details Converted from WW3 routine KERKEI.
 */
void kerkei(double x, double &ker, double &kei) noexcept {
  double zr = x * 0.5 * std::sqrt(2.0);
  double zi = zr;
  double cyr, cyi, cyr1, cyi1;
  kzeone(zr, zi, cyr, cyi, cyr1, cyi1);
  double exp_zr = std::exp(zr);
  ker = cyr / exp_zr;
  kei = cyi / exp_zr;
}

/**
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @details Converted from WW3 routine TABU_FW. Original author: F. Ardhuin.
 */
void tabuFw(std::span<double, SIZEFWTABLE + 1> fwTable, double &delab) noexcept {
  static constexpr int niter = 100;
  double ker, kei;
  double abr, abrlog, factor, fsubw, fsubwmemo, dzeta0, dzeta0memo;

  delab = (ABMAX - ABMIN) / static_cast<double>(SIZEFWTABLE);
  double l10 = std::log(10.0);

  for (int i = 0; i <= SIZEFWTABLE; ++i) {
    abrlog = ABMIN + static_cast<double>(i) * delab;
    abr = std::exp(abrlog * l10);
    factor = 1.0 / abr / (21.2 * KAPPA);
    fsubw = 0.05;
    dzeta0 = 0.0;
    for (int iter = 0; iter < niter; ++iter) {
      fsubwmemo = fsubw;
      dzeta0memo = dzeta0;
      dzeta0 = factor * std::pow(fsubw, -0.5);
      kerkei(2.0 * std::sqrt(dzeta0), ker, kei);
      fsubw = 0.08 / (ker * ker + kei * kei);
      fsubw = 0.5 * (fsubwmemo + fsubw);
      dzeta0 = 0.5 * (dzeta0memo + dzeta0);
    }
    fwTable[i] = std::min(fsubw, 0.5);
  }
}

} // namespace constants

} // namespace ww4_utils
