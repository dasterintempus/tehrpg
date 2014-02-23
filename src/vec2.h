#pragma once	
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <algorithm>

#define PI 3.14159265

namespace teh
{
	template <class T>
	T to_radians(const T& degrees)
	{
		return degrees * PI / 180.0;
	}
	
	template <class T>
	T to_degrees(const T& radians)
	{
		return radians * 180.0 / PI;
	}
	
	template <class T>
	class Vec2
	{
		public:
			friend std::ostream& operator<< (std::ostream& stream, const Vec2<T> vec)
			{
				stream << vec.x() << ", " << vec.y();
				return stream;
			}
		
			static bool do_segments_intersect(const Vec2<T>& a1, const Vec2<T>& a2, const Vec2<T>& b1, const Vec2<T>& b2)
			{
				Vec2<T> p = a1;
				Vec2<T> q = b1;
				Vec2<T> r = a2;
				Vec2<T> s = b2;
				if (r.cross(s) == 0 && (q-p).cross(r) == 0)
				{
					if ((q-p).dot(r) > 0 && (q-p).dot(r) > r.dot(r) && (p-q).dot(s) > 0 && (p-q).dot(s) < s.dot(s))
					{
						return false;
					}
					else
					{
						return true;
					}
				}
				else if (r.cross(s) == 0 && (q-p).cross(r) != 0)
				{
					return false;
				}
				else if (r.cross(s) != 0)
				{
					T t = (q - p).cross(s)/(r.cross(s));
					T u = (q - p).cross(r)/(r.cross(s));
					
					if (0 <= t && t <= 1 && 0 <= u && u <= 1)
					{
						return true;
					}
				}
				return false;
			}
		
			static Vec2 from_angle(const T& angle)
			{
				return Vec2(cos(angle), sin(angle));
			}
		
			Vec2() : _x((T)0), _y((T)0)
			{
				
			}
			
			Vec2(const T& x, const T& y) : _x(x), _y(y)
			{
				
			}
			
			T x() const
			{
				return _x;
			}
			
			T y() const
			{
				return _y;
			}
			
			void x(T in)
			{
				_x = in;
			}
			
			void y(T in)
			{
				_y = in;
			}
			
			Vec2 operator+(const Vec2& other) const
			{
				return Vec2(x() + other.x(), y() + other.y());
			}
			
			Vec2 operator-(const Vec2& other) const
			{
				return Vec2(x() - other.x(), y() - other.y());
			}
			
			Vec2 operator*(const Vec2& other) const
			{
				return Vec2(x() * other.x(), y() * other.y());
			}
			
			Vec2 operator*(const T& other) const
			{
				return Vec2(x() * other, y() * other);
			}
			
			Vec2 operator/(const Vec2& other) const
			{
				return Vec2(x() / other.x(), y() / other.y());
			}
			
			Vec2 operator/(const T& other) const
			{
				return Vec2(x() / other, y() / other);
			}
			
			T dot(const Vec2& other) const
			{
				return x() * other.x() + y() * other.y();
			}
			
			T cross(const Vec2& other) const
			{
				return x() * other.y() - y() * other.x();
			}
			
			T length() const
			{
				return sqrt(x() * x() + y() * y());
			}
			
			Vec2 unit() const
			{
				return Vec2(x(), y())/length();
			}
			
			Vec2 rotated(const T& angle) const
			{
				T nx = x() * cos(angle) - y() * sin(angle);
				T ny = x() * sin(angle) + y() * cos(angle);
				return Vec2(x(), y());
			}
			
			Vec2 rotated_right() const
			{
				return rotated(to_radians<T>(90));
			}
			
			Vec2 rotated_left() const
			{
				return rotated(to_radians<T>(-90));
			}
			
			void rotate(const T& angle)
			{
				T nx = x() * cos(angle) - y() * sin(angle);
				T ny = x() * sin(angle) + y() * cos(angle);
				x(nx);
				y(ny);
			}
			
			Vec2 rotated_around(const T& angle,const Vec2<T>& pivot) const
			{
				return (Vec2<T>(x(), y()) - pivot).rotated(angle) + pivot;
			}
			
			T angle() const
			{
				return atan2(y(), x());
			}
			
		private:
			T _x;
			T _y;
	};
	
	template <class T>
	class Quad
	{
		public:
			friend std::ostream& operator<< (std::ostream& stream, const Quad<T>& quad)
			{
				stream << "(" << quad.a() << ") ";
				stream << "(" << quad.b() << ") ";
				stream << "(" << quad.c() << ") ";
				stream << "(" << quad.d() << ") ";
				return stream;
			}
		
			static Quad rectangle(const Vec2<T>& topleft, const Vec2<T>& bottomright)
			{
				return Quad(topleft, Vec2<T>(bottomright.x(), topleft.y()), bottomright, Vec2<T>(topleft.x(), bottomright.y()));
			}
			
			static Quad rectangle(const T& left, const T& top, const T& right, const T& bottom)
			{
				return rectangle(Vec2<T>(left, top), Vec2<T>(right, bottom));
			}
		
			Quad(const Vec2<T>& a, const Vec2<T>& b, const Vec2<T>& c, const Vec2<T>& d)
				: _a(a), _b(b), _c(c), _d(d)
			{
				
			}
			
			Quad() : _a(Vec2<T>(0,0)), _b(Vec2<T>(0,0)), _c(Vec2<T>(0,0)), _d(Vec2<T>(0,0))
			{
				
			}
			
			Vec2<T> a() const
			{
				return _a;
			}
			
			Vec2<T> b() const
			{
				return _b;
			}
			
			Vec2<T> c() const
			{
				return _c;
			}
			
			Vec2<T> d() const
			{
				return _d;
			}
			
			T a2b_distance() const
			{
				return (a()-b()).length();
			}
			
			T b2c_distance() const
			{
				return (b()-c()).length();
			}
			
			T c2d_distance() const
			{
				return (c()-d()).length();
			}
			
			T d2a_distance() const
			{
				return (d()-a()).length();
			}
			
			void a(const Vec2<T>& in)
			{
				_a = in;
			}
			
			void b(const Vec2<T>& in)
			{
				_b = in;
			}
			
			void c(const Vec2<T>& in)
			{
				_c = in;
			}
			
			void d(const Vec2<T>& in)
			{
				_d = in;
			}
			
			template <class otherT>
			Quad<otherT> bounding_rectangle() const
			{
				Quad<otherT> out;
				
				otherT left = (otherT)(floor(a().x()));
				if ((otherT)(floor(b().x())) < left) left = (otherT)(floor(b().x()));
				if ((otherT)(floor(c().x())) < left) left = (otherT)(floor(c().x()));
				if ((otherT)(floor(d().x())) < left) left = (otherT)(floor(d().x()));
				
				otherT right = (otherT)(floor(a().x()));
				if ((otherT)(floor(b().x())) > right) right = (otherT)(floor(b().x()));
				if ((otherT)(floor(c().x())) > right) right = (otherT)(floor(c().x()));
				if ((otherT)(floor(d().x())) > right) right = (otherT)(floor(d().x()));
				
				otherT top = (otherT)(floor(a().y()));
				if ((otherT)(floor(b().y())) < top) top = (otherT)(floor(b().y()));
				if ((otherT)(floor(c().y())) < top) top = (otherT)(floor(c().y()));
				if ((otherT)(floor(d().y())) < top) top = (otherT)(floor(d().y()));
				
				otherT bottom = (otherT)(floor(a().y()));
				if ((otherT)(floor(b().y())) > bottom) bottom = (otherT)(floor(b().y()));
				if ((otherT)(floor(c().y())) > bottom) bottom = (otherT)(floor(c().y()));
				if ((otherT)(floor(d().y())) > bottom) bottom = (otherT)(floor(d().y()));
				
				return Quad<otherT>::rectangle(left, top, right, bottom);
			}
			
			bool contains(const Vec2<T>& point) const
			{
				unsigned short int intersections = 0;
				T biggestside = std::max(a2b_distance(), std::max(b2c_distance(), std::max(c2d_distance(), d2a_distance())));
				Vec2<T> raypoint(point.x()+(biggestside*2), point.y()+(biggestside*2));
				
				if (Vec2<T>::do_segments_intersect(point, raypoint, a(), b()))
					intersections++;
				if (Vec2<T>::do_segments_intersect(point, raypoint, b(), c()))
					intersections++;
				if (Vec2<T>::do_segments_intersect(point, raypoint, c(), d()))
					intersections++;
				if (Vec2<T>::do_segments_intersect(point, raypoint, d(), a()))
					intersections++;
				
				return (intersections%2) == 1;
			}
		private:
			Vec2<T> _a;
			Vec2<T> _b;
			Vec2<T> _c;
			Vec2<T> _d;
	};
	
	void Bresenham(Vec2<int> a, Vec2<int> b, std::function<void (int, int)> drawer);
}