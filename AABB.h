#pragma once

struct AABB
{
	Vector2 min{ -100.0f, -100.0f };
	Vector2 max{ 100.0f, 100.0f };

};

struct AABBInt
{
	Vector2Int min{ -100, -100 };
	Vector2Int max{ 100, 100 };
};
