#include "NoviceDrawUtility.h"
#include <Novice.h>
#include "AABB.h"


void DrawCircle(const Vector2& position, float radius, unsigned int color)
{

	Novice::DrawEllipse
	(
		static_cast<int>(position.x), static_cast<int>(position.y),
		static_cast<int>(radius), static_cast<int>(radius),
		0.0f, color, kFillModeSolid
	);

}

void DrawBox(const AABBInt& area, unsigned int color)
{
	Novice::DrawBox
	(
		area.min.x, area.min.y,
		area.max.x - area.min.x,
		area.max.y - area.min.y,
		0.0f,
		color,
		kFillModeWireFrame
	);
}