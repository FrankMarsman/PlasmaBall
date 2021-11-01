#ifndef PLASMABALL_H
#define PLASMABALL_H

#include <vector>
#include <vector2d.h>
#include <QImage>
#include <cmath>
#include <math.h>
#include <algorithm>


using namespace  std;

class PlasmaBall
{
public:
  typedef unsigned int uint;

  struct Particle {
    Vector2D p, v;
    double distToEdge; // distance to edge (may be via path)
    int nextHop; // to what particle to hop (-1 means straight to edge)
    int bX, bY; // in which block does particle fall
  }; // Particle

  double R; // radius of ball
  double distPower; // used to compute distance
  uint N; // number of particles
  uint nB; // divide area into [nB]*[nB] blocks

  double T;
  double maxEdgeDist;
  uint stepNr;
  uint Nfix;
  double r_draw, w_draw; // radius of particle and width of lines
  double coreSize;

  vector <vector <uint> > paths;
  vector <Particle> particles;
  vector < vector <double> > nBdistVec;
  vector <uint> updatedParticles; // particles that had their dist changed

  // functions:
  double GetParticleDist(uint a, uint b); // distance between particle [a] and [b]
  bool UpdateDistToEdge(bool firstRun = false);

  QImage ToQImage(bool drawPaths = true, bool drawDist = false);
  vector <uint> GetPathToBoundary(uint k0, double maxR);

  void Next(double dt);
  void ComputePaths( );
  void FillnBdistVec( );


  PlasmaBall(uint _N, double _R = 200);
  ~PlasmaBall( );
};

#endif // PLASMABALL_H
