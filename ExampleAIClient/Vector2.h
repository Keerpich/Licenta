#pragma once
class Vector2
{
public:
	float fX, fY;

public:
	Vector2();
	Vector2(float x, float y);

	Vector2 normalize() const;
	float magnitude() const;
	float dotProduct(Vector2 vec) const;
	float lenSq() const;

	Vector2 operator+(Vector2 &vec);
	Vector2 operator-(Vector2 &vec);
	Vector2 operator*(float scalar);
	float operator*(Vector2 &vec);
	Vector2 operator/(float scalar);
	Vector2& operator=(Vector2 &vec);
	float getAngleWith(Vector2 vec);

	float getX() const;
	float getY() const;

	~Vector2();
};

