/*
 * mscorner.cpp - microstrip corner class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id: mscorner.cpp,v 1.1 2004-07-25 16:58:47 margraf Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "mscorner.h"

mscorner::mscorner () : circuit (2) {
  type = CIR_MSCORNER;
}

void mscorner::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W = getPropertyDouble ("W");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");

  /* local variables */
  complex s11, s21;
  nr_double_t L, C, wh = W/H;

  // check validity
  if ((wh < 0.2) || (wh > 6.0)) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for 0.2 <= W/h <= 6.0\n");
  }
  if ((er < 2.36) || (er > 10.4)) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for 2.36 <= er <= 10.4\n");
  }
  if (frequency/h < 1e6) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for freq/h[mm] <= 1GHz\n");
  }

  // capacitance in pF
  C = W * ( (10.35 * er + 2.5) * wh + (2.6 * er + 5.64) );
  // inductance in nH
  L = 220.0 * h * ( 1.0 - 1.35 * exp( -0.18 * pow(wh, 1.39) ) );

  s21 = complex(0.0, 1e12 / (2*M_PI*frequency*C));
  s11 = complex(0.0, 2e-9*M_PI*frequency*L) + s21;

  matrix matrixZ (2,2);
  matrixZ.set (1, 1, s11);
  matrixZ.set (1, 2, s21);
  matrixZ.set (2, 1, s21);
  matrixZ.set (2, 2, s11);
  matrixZ = ztos (matrixZ);

  setS (1, 1, matrixZ.get (1, 1));
  setS (1, 2, matrixZ.get (1, 2));
  setS (2, 1, matrixZ.get (2, 1));
  setS (2, 2, matrixZ.get (2, 2));
}

void mscorner::calcDC (void) {
  setC (1, 1, +1.0); setC (1, 2, -1.0);
  setB (1, 1, +1.0); setB (2, 1, -1.0);
  setE (1, 0.0);
  setD (1, 1, 0.0);
}
