#pragma once
#ifndef RENDERER_H
#define RENDERER_H
#include <glad/glad.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>

#define ASSERT(x) if (!(x)) assert(false)

#ifdef DEBUG
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLCheckError())
#else
#define GLCall(x) x
#endif

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

void GLClearError();

bool GLCheckError();
#endif
