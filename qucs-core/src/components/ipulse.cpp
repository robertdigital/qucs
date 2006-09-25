/*
 * ipulse.cpp - pulse current source class implementation
 *
 * Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: ipulse.cpp,v 1.6 2006-09-25 07:45:01 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "net.h"
#include "component_id.h"
#include "ipulse.h"

ipulse::ipulse () : circuit (2) {
  type = CIR_IPULSE;
  setISource (true);
}

void ipulse::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void ipulse::initDC (void) {
  nr_double_t i = getPropertyDouble ("I1");
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void ipulse::initAC (void) {
  allocMatrixMNA ();
  clearI ();
}

void ipulse::initTR (void) {
  initDC ();
}

void ipulse::calcTR (nr_double_t t) {
  nr_double_t i1 = getPropertyDouble ("I1");
  nr_double_t i2 = getPropertyDouble ("I2");
  nr_double_t t1 = getPropertyDouble ("T1");
  nr_double_t t2 = getPropertyDouble ("T2");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  nr_double_t it = 0;
  nr_double_t s  = getNet()->getSrcFactor ();

  if (t < t1) { // before pulse
    it = i1;
  }
  else if (t >= t1 && t < t1 + tr) { // rising edge
    it = i1 + (i2 - i1) / tr * (t - t1);
  }
  else if (t >= t1 + tr && t < t2 - tf) { // during full pulse
    it = i2;
  }
  else if (t >= t2 - tf && t < t2) { // falling edge
    it = i2 + (i1 - i2) / tf * (t - (t2 - tf));
  }
  else { // after pulse
    it = i1;
  }
  setI (NODE_1, +it * s); setI (NODE_2, -it * s);
}
