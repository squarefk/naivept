#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef double real;

enum Material{DIFF, SPEC, REFR, CERA, MARBLE, METAL};

const int HEIGHT = 720;
const int WIDTH = 1280;
const real ALPHA = 0.9f;
const real APERTURE = 4.0f;
const real INIT_GLOBAL_RATE = 0.01f;
// 0.01 => 720 * 1280
const int TOTAL_PHOTON_NUMBER = 5e6;
const real EXPOSURE = 0.5;

#endif
