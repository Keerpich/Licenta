#include "Vector2.h"
#include "cmath"

///<summary>Initialize the vector with (0,0)</summary>
Vector2::Vector2()
{
	fX = 0.f;
	fY = 0.f;
}

///<summary>Initialize the vector with (x,y)</summary>
Vector2::Vector2(float x, float y)
{
	fX = x;
	fY = y;
}

///<summary>Get the x component of the vector</summary>
float Vector2::getX() const
{
	return fX;
}

///<summary>Get the y component of the vector</summary>
float Vector2::getY() const
{
	return fY;
}

///<summary>Returns this vector's magnitude</summary>
float Vector2::magnitude() const
{
	return sqrt(fX*fX + fY*fY);
}

///<summary>Returns the squared length of the vector</summary>
float Vector2::lenSq() const
{
	return fX*fX + fY*fY;
}

///<summary>Returns this vector's normalized version</summary>
Vector2 Vector2::normalize() const
{
	float mag = this->magnitude();
	return Vector2(fX / mag, fY / mag);
}

///<summary>Divide by a float</summary>
Vector2 Vector2::operator/(float scalar)
{
	return Vector2(fX / scalar, fY / scalar);
}

///<summary>Multiplies this vector with a scalar</summary>
Vector2 Vector2::operator*(float scalar)
{
	return Vector2(fX * scalar, fY * scalar);
}

///<summary>Adds a vector to this one</summary>
Vector2 Vector2::operator+(Vector2 &vec)
{
	return Vector2(this->fX + vec.fX, this->fY + vec.fY);
}

///<summary>Substracts a vector from this one</summary>
Vector2 Vector2::operator-(Vector2 &vec)
{
	return Vector2(this->fX - vec.fX, this->fY - vec.fY);
}

///<summary>Returns the dot product of the this vector with another</summary>
float Vector2::operator*(Vector2 &vec)
{
	return this->fX * vec.fX + this->fY * vec.fY;
}

Vector2& Vector2::operator=(Vector2 &vec)
{
	this->fX = vec.fX;
	this->fY = vec.fY;
	return *this;
}

///<summary>Returns the angle formed by this vector with the other</summary>
float Vector2::getAngleWith(Vector2 vec)
{
	return acos(this->normalize() * vec.normalize());
}

Vector2::~Vector2()
{
}