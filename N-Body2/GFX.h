#pragma once
#include <iostream>
#include <csignal>
#include <Camera.h>

#include <Renderer.h>


void* GFXInit(unsigned int width, unsigned int height, unsigned char major, unsigned char minor, const char* windowTitle);
void GFXFrameTime(double& lastTime, int& nbFrames);
