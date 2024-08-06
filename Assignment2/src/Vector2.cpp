#include <cmath>
#include "Vector2.h"

Vector2::Vector2() {}

Vector2::Vector2(float xin, float yin) : x(xin), y(yin) {}

Vector2 &Vector2::add(Vector2 &other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2 &Vector2::scale(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 &Vector2::normalize()
{
    float len = length();
    x /= len;
    y /= len;
    return *this;
}

Vector2 &Vector2::perpendicular()
{
    float temp = x;
    x = -y;
    y = temp;
    return *this;
}

Vector2 Vector2::clone()
{
    return Vector2(x, y);
}

Vector2 Vector2::operator+(const Vector2 &other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator*(float scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const
{
    return Vector2(x / scalar, y / scalar);
}

void Vector2::operator+=(const Vector2 &rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vector2::operator-=(const Vector2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vector2::operator*=(const float val)
{
    x *= val;
    y *= val;
}

void Vector2::operator/=(const float val)
{
    x /= val;
    y /= val;
}

Vector2 Vector2::operator-(const Vector2 &other) const
{
    return Vector2(x - other.x, y - other.y);
}

bool Vector2::operator==(const Vector2 &other) const
{
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2 &other) const
{
    return x != other.x || y != other.y;
}

float Vector2::length() const
{
    return sqrt(x * x + y * y);
}

float Vector2::dist(Vector2 v) const
{
    float dx = v.x - x;
    float dy = v.y - y;
    return sqrt(dx * dx + dy * dy);
}
