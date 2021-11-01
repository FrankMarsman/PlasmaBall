#include "plasmaball.h"

#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <QPainter>
#include <QPainterPath>
#include <QImage>
#include <QPen>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif


// returns distance between particle a and b, raised to power [distPower]
double PlasmaBall::GetParticleDist(uint a, uint b) {
  double dx = particles[a].p.x - particles[b].p.x;
  double dy = particles[a].p.y - particles[b].p.y;
  return pow(dx*dx + dy*dy, distPower);
} // GetParticleDist


// computes values of nBdistVec, where nBdistVec[i][j]
// equals distance for dx,dy = i,j
void PlasmaBall::FillnBdistVec( ) {
  this->nBdistVec = vector < vector <double> > (nB+1, vector <double> (nB+1, 0));
  double dx, dy, dx2;
  double size = (2 * R) / nB;
  for (uint i = 0; i < nB+1; i++) {
    dx = 0;
    if (i > 0)
      dx = (i-1) * size;
    dx2 = dx*dx;
    for (uint j = 0; j < nB+1; j++) {
      dy = 0;
      if (j > 0)
        dy = (j-1) * size;
      nBdistVec[i][j] = pow(dx2 + dy*dy, distPower);
    } // for
  } // for
} // FillnBdistVec


// updates [distToEdge] of each particle and returns true
// if distances are final. If [firstRun] is true, we initialize
bool PlasmaBall::UpdateDistToEdge(bool firstRun) {
  // init to straight line distance to edge
  if (firstRun) {

//    sort(particles.begin( ), particles.end( ), [](const Particle& lhs, const Particle& rhs) {
//       return lhs.p.x < rhs.p.x;
//    });
//    sort(particles.begin( ), particles.end( ), [](const Particle& lhs, const Particle& rhs) {
//       return lhs.p.y < rhs.p.y;
//    });

    this->maxEdgeDist = 1e-9;
    this->updatedParticles.clear( );
    double r, dx, dy, dR;
    for (uint i = 0; i < particles.size( ); i++) {
      dx = particles[i].p.x;
      dy = particles[i].p.y;
      r = sqrt(dx*dx + dy*dy);
      dR = this->R - r;

      if (dR < 0) {
        qDebug( ) << "Err in UpdateDistToEdge! dR < 0";
        dR = 0;
      } // if

      particles[i].distToEdge = pow(dR, 2*distPower);
      if (particles[i].distToEdge > maxEdgeDist)
        maxEdgeDist = particles[i].distToEdge;

      particles[i].nextHop = -1;
      particles[i].bX = this->nB * ((particles[i].p.x - R) / (2*R));
      particles[i].bY = this->nB * ((particles[i].p.y - R) / (2*R));
      this->updatedParticles.push_back(i);
    } // for
    return false;
  } // if

  // update dist
  Particle p1, p2;
  bool done = true;
  int skipCounter = 0;
  int totCounter = 0;
  int changeCounter = 0;
  int bx1, by1;
  double pDist, dist1, dist2, min_pDist;
  uint j;
  vector <bool> newUpdatedParticles(this->N, false);
  this->maxEdgeDist = 1e-9;
  for (uint i = 0; i < particles.size( ); i++) {
    p1 = particles[i];

    dist1 = p1.distToEdge;
    if (dist1 > maxEdgeDist)
      maxEdgeDist = dist1;

    bx1 = p1.bX;
    by1 = p1.bY;

    for (uint n = 0; n < updatedParticles.size( ); n++) {
      j = updatedParticles[n];

      if (i == j)
        continue;

      totCounter++;

      p2 = particles[j];
      dist2 = p2.distToEdge;

      if (dist2 >= dist1) {
        skipCounter++;
        continue;
      } // if

      min_pDist = nBdistVec[abs(bx1-p2.bX)][abs(by1-p2.bY)];

      if (dist2 + min_pDist >= dist1) {
        skipCounter++;
        continue;
      } // if

      pDist = GetParticleDist(i, j);

      if (dist2 + pDist < dist1) { // via p2 is faster than what we have
        dist1 = dist2 + pDist;
        particles[i].distToEdge = dist1;
        particles[i].nextHop = j;
        done = false;
        changeCounter++;
        newUpdatedParticles[i] = true;
      } // if
    } // for
  } // for

  this->updatedParticles.clear( );
  for (uint i = 0; i < particles.size( ); i++)
    if (newUpdatedParticles[i])
      updatedParticles.push_back(i);

  //qDebug( ) << "UDTE: skipCounter =" << skipCounter/1000 << "k =" << 100 * double(skipCounter)/totCounter << "%"
  //          << "#changes =" << changeCounter << "#updates =" << updatedParticles.size( );

  return done;
} // UpdateDistToEdge


QImage PlasmaBall::ToQImage(bool drawPaths, bool drawDist) {
  uint IMG_SIZE = 2*R+20;
  QImage img(IMG_SIZE, IMG_SIZE, QImage::Format_ARGB32);
  QPainter qPainter(&img);
  Particle k;
  QPen qPen(Qt::red);

  img.fill(qRgba(0, 0, 0, 0));


  qPainter.setRenderHint(QPainter::Antialiasing);
  //qPainter.fillRect(QRect(0, 0, 2*R+20, 2*R+20), Qt::transparent);
  //qPainter.fillRect(QRect(0, 0, 2*R+20, 2*R+20), QColor(0,0,0));

  // draw ball
  qPen.setColor(Qt::yellow);
  qPen.setWidthF(5);
  qPainter.setPen(qPen);
  qPainter.setBrush(QColor(0, 0, 0));
  qPainter.drawEllipse(3, 3, 2*R+20-6, 2*R+20-6);

  if (drawDist == false) {
    // draw particles
    qPainter.setBrush(QColor(255, 165, 0));
    qPen.setColor(QColor(255, 165, 0));
    qPen.setWidthF(0.25);
    qPainter.setPen(qPen);
    double r = r_draw;
    for (uint i = 0; i < particles.size( ); i++) {
      k = particles[i];
      qPainter.drawEllipse(10+R+k.p.x-r, 10+R+k.p.y-r, 2*r, 2*r);
    } // for
  } // if
  else { // color particles by distance
    qPen.setColor(Qt::black);
    qPen.setWidthF(0.85);
    qPainter.setPen(qPen);
    //double maxDist = pow(this->R, 2*this->distPower);
    double maxDist = maxEdgeDist;
    double r = r_draw;
    for (uint i = 0; i < particles.size( ); i++) {
      k = particles[i];
      double dist = k.distToEdge;
      double c = dist / maxDist;
      if (c > 1)
        c = 1;
      c = 1.0 - c;
      c *= 0.9;
      c += 0.1;
      qPainter.setBrush(QColor(255*c, 55*c, 55*c));
      qPainter.drawEllipse(10+R+k.p.x-r, 10+R+k.p.y-r, 2*r, 2*r);
    } // for
  } // else

  if (drawPaths) {
    qPen.setColor(Qt::blue);
    qPen.setWidthF(w_draw);
    qPainter.setPen(qPen);
    qPainter.setBrush(Qt::blue);



//    for (uint i = 0; i < particles.size( ); i++) {
//      k = particles[i];
//      if (k.nextHop == -1)
//        continue;

//      qPainter.drawLine(10+R+k.p.x, 10+R+k.p.y, 10+R+particles[k.nextHop].p.x, 10+R+particles[k.nextHop].p.y);
//    } // for

    for (uint i = 0; i < Nfix; i++) {
      k = particles[i];
      while (k.nextHop != -1) {
        qPainter.drawLine(10+R+k.p.x, 10+R+k.p.y, 10+R+particles[k.nextHop].p.x, 10+R+particles[k.nextHop].p.y);
        k = particles[k.nextHop];
      } // while
    } // for


    // draw inner ball:
    qPen.setColor(Qt::yellow);
    qPen.setWidthF(5);
    qPainter.setPen(qPen);
    qPainter.setBrush(QColor(102, 0, 102));
    qPainter.drawEllipse(QPointF(IMG_SIZE/2, IMG_SIZE/2), R * qreal(this->coreSize), R * qreal(this->coreSize));
  } // if

  //qPainter.fillRect(QRect(0, 0, 50, 50), Qt::transparent);


  return img;
} // ToQImage

void PlasmaBall::Next(double dt) {

  // update positions;
  for (uint i = 0; i < particles.size( ); i++) {
    particles[i].p += particles[i].v * dt;

//    // sometimes noise, if particle not close to boundary
//    if (particles[i].p.Length( ) < 0.5*R)
//        ;
//      //if (rand( ) % 10 == 0)
//      //  particles[i].p.Rotate(0.0002 * M_PI * (double(rand( )) / RAND_MAX - 0.5));
  } // for

  // fix velocities if particle is outside ball
  for (uint i = 0; i < particles.size( ); i++) {
    Particle k = particles[i];
    if (k.p.Length( ) >= R) { // outside ball
      particles[i].v = -1 * k.v;
      particles[i].p += particles[i].v * dt;

      double angK = k.v.Theta( );
      double angR = k.p.Theta( );
      double theta = angK - angR;

//      if (theta >= M_PI) {
//        theta -= M_PI;
//        qDebug( ) << "theta -= M_PI";
//      } // if
//      else if (theta <= -M_PI) {
//        theta += M_PI;
//        qDebug( ) << "theta += M_PI";
//      } // else if

      particles[i].v.Rotate(theta);
//      k = particles[i];

//      if (k.p.Length( ) >= R) {
//        qDebug( ) << "Error! particle stays outside R!";
//        particles[i].p = Vector2D(0, 0);
//        particles[i].v = Vector2D(0, 0);

//        if (particles[i].p.Length( ) >= R) {
//          qDebug( ) << "Error! particle stays outside R after correction!";
//          qDebug( ) << particles[i].p.ToQString(" position");
//          qDebug( ) << particles[i].v.ToQString(" new v");
//          qDebug( ) << "  angR =" << angR / M_PI;
//          qDebug( ) << "  angK =" << angK / M_PI;
//          qDebug( ) << "  theta =" << theta / M_PI;
//          particles[i].p = Vector2D(50, 50);
//          particles[i].v = Vector2D(0, 0);
//        } // if
//      } // if
    } // if
  } // for


  // RESET SOME UNITS
  for (uint i = 0; i < Nfix; i++) {
    particles[i].p.x = R * coreSize;
    particles[i].p.y = 0;
    particles[i].p.Rotate(i * (2.0*M_PI/Nfix));
    particles[i].v = Vector2D(1,1);
    particles[i].v.Rotate(2 * M_PI * double(rand( )) / RAND_MAX);
    particles[i].v.x += 0.1 * (double(rand( )) / RAND_MAX - 0.5);
    particles[i].v.y += 0.1 * (double(rand( )) / RAND_MAX - 0.5);
  } // for

  stepNr++;
  T += dt;
} // Next

// returns path starting from particle k0, ending in point where r >= maxR
vector<uint> PlasmaBall::GetPathToBoundary(uint k0, double maxR) {
  vector <double> rList; // list of R of each particle
  for (uint i = 0; i < particles.size( ); i++)
    rList.push_back(particles[i].p.Length( ));

  vector <uint> path;
  path.push_back(k0);
  double curR; // current distance, and distance of next particle
  double minDist, dist;
  int newK;
  Particle curK, nextK;
  curK = particles[k0];
  curR = curK.p.Length( );
  while (curR < maxR) {
    minDist = 2*R + 1;
    newK = -1;
    for (uint i = 0; i < particles.size( ); i++) {
      if (rList[i] > curR + 0.001) { // particle is closer
        nextK = particles[i];
        dist = (nextK.p - curK.p).Length( );
        if (dist < minDist) { // closest particle so far
          minDist = dist;
          newK = i;
        } // if
      } // if
    } // for
    if (newK == -1) { // fail
      //qDebug( ) << "Problem: newK = -1";
      break;
    } // if
    path.push_back(newK);
    curK = particles[newK];
    curR = curK.p.Length( );

    if (path.size( ) >= particles.size( )) {
      qDebug( ) << "WTF te lange path";
      break;
    } // if
  } // while
  //qDebug( ) << "GetPathToBoundary, k =" << k0 << "path size =" << path.size( );
  return path;
} // GetPathToBoundary

// fills vector paths with paths
void PlasmaBall::ComputePaths( ) {
  uint P = 10; // max number of paths
  double maxStartR; // path must start from point < maxStartR
  maxStartR = R * 0.4;
  double endR = 0.9 * R; // path ends here

  this->paths.clear( );

  if (P > particles.size( ))
    qDebug( ) << "Error in ComputePaths! P too large!";

  //qDebug( ) << "Computing paths...";
  for (uint i = 0; i < particles.size( ); i++) {
    if (paths.size( ) >= P)
      return;
    if (particles[i].p.Length( ) <= maxStartR) {
      //qDebug( ) << "Found start path! numPaths =" << paths.size( );
      this->paths.push_back(GetPathToBoundary(i, endR));
    } // if
  } // for
} // ComputePaths




PlasmaBall::PlasmaBall(uint _N, double _R) {
  this->R = _R;
  this->N = _N;
  this->T = 0;
  this->stepNr = 0;
  this->distPower = 2;
  this->nB = 100;
  this->maxEdgeDist = 1e-9;
  this->Nfix = 6;
  this->r_draw = 0.55;
  this->w_draw = 3.5;
  this->coreSize = 0.1;

  FillnBdistVec( );

  // init particles:
  for (uint i = 0; i < N; i++) {
    Particle part;
    part.p.x = (R * double(rand( )) / RAND_MAX) - 0.5 * R; // in [-0.5R 0.5R]
    part.p.y = (R * double(rand( )) / RAND_MAX) - 0.5 * R; // in [-0.5R 0.5R]

    part.v = Vector2D(1,1);
    part.v.Rotate(2 * M_PI * double(rand( )) / RAND_MAX);
    part.v.x += 0.1 * (double(rand( )) / RAND_MAX - 0.5);
    part.v.y += 0.1 * (double(rand( )) / RAND_MAX - 0.5);
    part.nextHop = -1;
    this->particles.push_back(part);
  } // for
} // PlasmaBall

PlasmaBall::~PlasmaBall( ) {

} // ~PlasmaBall




































