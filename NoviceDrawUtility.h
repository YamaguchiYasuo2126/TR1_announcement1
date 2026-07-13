#pragma once
#include "Vector2.h"
#include "AABB.h"

void DrawCircle(const Vector2& position, float radius, unsigned int color);

void DrawBox(const AABBInt& area, unsigned int color);
