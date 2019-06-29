#pragma once

// TODO: this file should be renamed; and, realistically, these enums
// should be moved somewhere else

enum neighborhood {
  OLIM4,
  OLIM8,
  OLIM6,
  OLIM18,
  OLIM26,
  OLIM3D,
  FMM2,
  FMM3
};

/**
 * TODO: `cost_func' -> `quadrature'
 */
enum cost_func {
  MP0,
  MP1,
  RHR
};

typedef enum status {
  SUCCESS
} status_e;
