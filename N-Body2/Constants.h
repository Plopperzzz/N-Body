#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int DIMENSIONS = 2;

constexpr int PARTITIONS = (DIMENSIONS == 2) ? 4 : (DIMENSIONS == 3) ? 8 : 0;

constexpr double EPISLON = 1e-13;

constexpr double G = 6.6743e-11;

constexpr double C = 2.99792458e8;

constexpr double M_PI =   3.14159265358979323;
constexpr double SQRT_2 = 1.41421356237309515;
constexpr double SQRT_3 = 1.73205080756887719;


#endif // CONSTANTS_H