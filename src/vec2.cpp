#include "vec2.h"

namespace teh
{
	void Bresenham(Vec2<int> a, Vec2<int> b, std::function<void (int, int)> drawer)
	{
		int x1 = a.x();
		int y1 = a.y();
		int x2 = b.x();
		int y2 = b.y();
		int delta_x(x2 - x1);
		// if x1 == x2, then it does not matter what we set here
		signed char const ix((delta_x > 0) - (delta_x < 0));
		delta_x = std::abs(delta_x) << 1;

		int delta_y(y2 - y1);
		// if y1 == y2, then it does not matter what we set here
		signed char const iy((delta_y > 0) - (delta_y < 0));
		delta_y = std::abs(delta_y) << 1;

		drawer(x1, y1);

		if (delta_x >= delta_y)
		{
			// error may go below zero
			int error(delta_y - (delta_x >> 1));

			while (x1 != x2)
			{
				if ((error >= 0) && (error || (ix > 0)))
				{
					error -= delta_x;
					y1 += iy;
				}
				// else do nothing

				error += delta_y;
				x1 += ix;

				drawer(x1, y1);
			}
		}
		else
		{
			// error may go below zero
			int error(delta_x - (delta_y >> 1));

			while (y1 != y2)
			{
				if ((error >= 0) && (error || (iy > 0)))
				{
					error -= delta_y;
					x1 += ix;
				}
				// else do nothing

				error += delta_x;
				y1 += iy;

				drawer(x1, y1);
			}
		}
	}
}