#ifndef MCTRUTH_DATASTRUCTURE_H
#define MCTRUTH_DATASTRUCTURE_H

typedef struct {
  int trackIndex;
  int PDGcode;
  int charge;
  int v0Index;
  int v1Index;
  int px;
  int py;
  int pz;
  int energy;
  int minDaughterIndex;
  int maxDaughterIndex;
} TrackTruth_t;

typedef struct {
  int vertexIndex;
  int parentTrackIndex;
  int x;
  int y;
  int z;
  int time;
} VertexTruth_t;

typedef struct {
  int trackIndex;
  int layerId;
  int cellId;
  int edep;
  int driftD;
  int x;
  int y;
  int z;
  int posFlag;
} MdcTruth_t;

typedef struct {
  int trackIndex;
  int partId;
  int scinNb;
  int strip;
  int x;
  int y;
  int z;
  int px;
  int py;
  int pz;
  int trackLength;
  int time;
} TofTruth_t;

typedef struct {
  int trackIndex;
  int partId;
  int numTheta;
  int numPhi;
  int x;
  int y;
  int z;
  int px;
  int py;
  int pz;
  int totalEdep;
} EmcTruth_t;

typedef struct {
  int trackIndex;
  int partId;
  int segId;
  int gapId;
  int stripId;
  int x;
  int y;
  int z;
  int px;
  int py;
  int pz;
} MucTruth_t;

#endif
