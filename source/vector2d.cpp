#include "Vector2D.h"

#include <QDebug>
#include <QPointF>
#include <cmath>

// for graphics
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QPen>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

// create copy of v
Vector2D::Vector2D(Vector2D * v) {
  x = v->x;
  y = v->y;
} // Vector2D

// default; x = y = z = 0
Vector2D::Vector2D( ) {
  x = 0;
  y = 0;
} // Vector2D

// set z to 0 and x, y to point
Vector2D::Vector2D(QPointF p) {
  x = p.x( );
  y = p.y( );
} // Vector2D

// set x y z
Vector2D::Vector2D(double _x, double _y) {
  x = _x;
  y = _y;
} // Vector2D

Vector2D::~Vector2D( ) {

} // ~Vector2D











