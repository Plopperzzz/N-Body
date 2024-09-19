#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int DIMENSIONS = 3;

constexpr int PARTITIONS = (DIMENSIONS == 2) ? 4 : (DIMENSIONS == 3) ? 8 : 0;

constexpr double EPISLON = 1e-13;

constexpr double G = 6.6743e-11;

constexpr double C = 2.99792458e8;

constexpr double M_PI = 3.14159265358979323846;

#endif // CONSTANTS_H