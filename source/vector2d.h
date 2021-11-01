#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>
#include <math.h>
#include <QPointF>
#include <utility>
#include <QImage>
#include <QString>

using namespace std;

// this class describes a vector in R^3,
// having 3 double coordinates x,y,z
class Vector2D
{
public:
  typedef unsigned int uint;

  double x, y;

  Vector2D(Vector2D * v);
  Vector2D(double _x, double _y);
  Vector2D(QPointF p);
  Vector2D( );
  ~Vector2D( );

  //
  // operator overloading
  //

  // arithmetic operators:

  // returns ans = this + v
  Vector2D operator+(const Vector2D & v) {
    Vector2D ans(this);
    ans.x += v.x;
    ans.y += v.y;
    return ans;
  } // operator+

  // returns ans = this - v
  Vector2D operator-(const Vector2D & v) {
    Vector2D ans(this);
    ans.x -= v.x;
    ans.y -= v.y;
    return ans;
  } // operator-

  // returns ans = this * a
  Vector2D operator*(double a) {
    Vector2D ans(this);
    ans.x *= a;
    ans.y *= a;
    return ans;
  } // operator*

  // returns ans = this / a
  Vector2D operator/(double a) {
    Vector2D ans(this);
    ans.x /= a;
    ans.y /= a;
    return ans;
  } // operator/

  // assigment operators:

  // adds v to this
  Vector2D& operator+=(const Vector2D & v) {
    this->x += v.x;
    this->y += v.y;
    return *this;
  } // operator+=

  // subtracts v from this
  Vector2D& operator-=(const Vector2D & v) {
    this->x -= v.x;
    this->y -= v.y;
    return *this;
  } // operator-=

  // divides this by a
  Vector2D& operator/=(double a) {
    this->x /= a;
    this->y /= a;
    return *this;
  } // operator/=

  // multiplies this by a
  Vector2D& operator*=(double a) {
    this->x *= a;
    this->y *= a;
    return *this;
  } // operator*=

  // short functions:

  // rotates this vector
  void Rotate(double theta) {
    double S = sin(theta);
    double C = cos(theta);
    double x_, y_;
    x_ = x*C - y*S;
    y_ = x*S + y*C;
    x = x_;
    y = y_;
  } // Rotate

  // angle with x-axis
  double Theta( ) {
    return atan2(y, x);
  } // Theta

  double Length( ) {
    return sqrt(x*x + y*y);
  } // Length

  // returns distance from this to v
  double Dist(Vector2D v) {
    Vector2D d(this);
    d -= v;
    return d.Length( );
  } // Dist

  double DotProduct(Vector2D v) {
    return x*v.x + y*v.y;
  } // DotProduct

  QString ToQString(QString name = "vec") {
    QString ans = name + ": ";
    ans += "x = " + QString::number(x) + ", ";
    ans += "y = " + QString::number(y);
    return ans;
  } // ToQString

  // returns x- and y-coordinate of vec
  QPointF XY( ) {
    return QPointF(this->x, this->y);
  } // XY

};

inline Vector2D operator*(double a, const Vector2D & v) {
  Vector2D ans;
  ans.x = v.x * a;
  ans.y = v.y * a;
  return ans;
} // operator*

#endif // VECTOR2D_H
















