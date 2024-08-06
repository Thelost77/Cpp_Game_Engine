#ifndef VECTOR2_H

class Vector2 {
public:
    float x, y;

    // Constructors
    Vector2();
    Vector2(float xin, float yin);

    Vector2& add(Vector2 &other);
    Vector2& scale(float scalar);
    Vector2& normalize();
    Vector2& perpendicular();
    Vector2 clone();

    bool operator==(const Vector2 &other) const;
    bool operator!=(const Vector2 &other) const;

    Vector2 operator+ (const Vector2& rhs) const;
    Vector2 operator- (const Vector2& rhs) const;
    Vector2 operator* (float scalar) const;
    Vector2 operator/ (float scalar) const;

    void operator+= (const Vector2& rhs);
    void operator-= (const Vector2& rhs);
    void operator*= (const float val);
    void operator/= (const float val);

    float length() const;
    float dist(Vector2 v) const;

};

#define VECTOR2_H
#endif
