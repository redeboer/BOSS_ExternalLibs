/*
 *      2018/5/30   Long Peixun     IHEP
 *                  Convert DST tracks to REC tracks
*/

#ifndef DST_CONVERT
#define DST_CONVERT

#include "RootEventData/TMdcTrack.h"
#include "RootEventData/TTofTrack.h"
#include "RootEventData/TEmcTrack.h"
#include "RootEventData/TMucTrack.h"
#include "RootEventData/TRecMdcTrack.h"
#include "RootEventData/TRecTofTrack.h"
#include "RootEventData/TRecEmcShower.h"
#include "RootEventData/TRecMucTrack.h"

//Convert TMdcTrack to TRecMdcTrack
void cvtMdcDst2MdcRec(TRecMdcTrack* dist, const TMdcTrack* src)
{
    dist->setTrackId(src->trackId());
    dist->setStat(src->stat());
    dist->setChi2(src->chi2());
    dist->setNdof(src->ndof());
    dist->setNster(src->nster());
    dist->setNlayer(src->nlayer());
    
    Double_t tmpHelix[5];
    for (int i = 0; i < 5; ++i) tmpHelix[i] = src->helix(i);
    dist->setHelix(tmpHelix);
    Double_t tmpErr[15];
    for (int i = 0; i < 15; ++i) tmpErr[i] = src->err(i);
    dist->setErr(tmpErr);
}

//Convert TTofTrack to TRecTofTrack
void cvtTofDst2TofRec(TRecTofTrack* dist, const TTofTrack* src)
{
    dist->setTofTrackID(src->tofTrackID());
    dist->setTrackID(src->trackID());
    dist->setTofID(src->tofID());
    dist->setStatus(src->status());
    dist->setPath(src->path());
    dist->setZrHit(src->zrhit());
    dist->setPh(src->ph());
    dist->setTof(src->tof());
    dist->setErrTof(src->errtof());
    dist->setBeta(src->beta());
    dist->setQuality(src->quality());
    dist->setT0(src->t0());
    dist->setErrT0(src->errt0());
    dist->setErrZ(src->errz());
    dist->setPhi(src->phi());
    dist->setErrPhi(src->errphi());
    dist->setEnergy(src->energy());
    dist->setErrEnergy(src->errenergy());

    Double_t tmpTexp[5];
    for (int i = 0; i < 5; ++i) tmpTexp[i] = src->texp(i);
    dist->setTexp(tmpTexp);
    Double_t tmpToffset[6];
    for (int i = 0; i < 6; ++i) tmpToffset[i] = src->toffset(i);
    dist->setTexp(tmpToffset);
    Double_t tmpSigma[6];
    for (int i = 0; i < 6; ++i) tmpSigma[i] = src->sigma(i);
    dist->setTexp(tmpSigma);
}

//Convert TEmcTrack to TRecEmcShower
void cvtEmcDst2EmcRec(TRecEmcShower* dist, const TEmcTrack* src)
{
    dist->setTrackId(src->trackId());
    dist->setNumHits(src->numHits());
    dist->setStatus(src->status());
    dist->setCellId(src->cellId());
    dist->setModule(src->module());
    dist->setX(src->x());
    dist->setY(src->y());
    dist->setZ(src->z());
    dist->setTheta(src->theta());
    dist->setPhi(src->phi());
    dist->setDtheta(src->dtheta());
    dist->setDphi(src->dphi());
    dist->setEnergy(src->energy());
    dist->setDE(src->dE());
    dist->setESeed(src->eSeed());
    dist->setE3x3(src->e3x3());
    dist->setE5x5(src->e5x5());
    dist->setTime(src->time());
    dist->setSecondMoment(src->secondMoment());
    dist->setLatMoment(src->latMoment());
    dist->setA20Moment(src->a20Moment());
    dist->setA42Moment(src->a42Moment());

    Double_t tmpErr[6];
    for (int i = 0; i < 6; ++i) tmpErr[i] = src->err(i);
    dist->setErr(tmpErr);
}

//Convert TMucTrack to TRecMucTrack
void cvtMucDst2MucRec(TRecMucTrack* dist, const TMucTrack* src)
{
    dist->setTrackId(src->trackId());
    dist->setId(src->id());
    dist->setStatus(src->status());
    dist->setType(src->type());
    dist->setStartPart(src->startPart());
    dist->setEndPart(src->endPart());
    dist->setBrLastLayer(src->brLastLayer());
    dist->setEcLastLayer(src->ecLastLayer());
    dist->setNumHits(src->numHits());
    dist->setNumLayers(src->numLayers());
    dist->setMaxHitsInLayer(src->maxHitsInLayer());
    dist->setDepth(src->depth());
    dist->setChi2(src->chi2());
    dist->setDof(src->dof());
    dist->setRms(src->rms());
    dist->setDistance(src->distance());
    dist->setDeltaPhi(src->deltaPhi());
    dist->setXPos(src->xPos());
    dist->setYPos(src->yPos());
    dist->setZPos(src->zPos());
    dist->setXPosSigma(src->xPosSigma());
    dist->setYPosSigma(src->yPosSigma());
    dist->setZPosSigma(src->zPosSigma());
    dist->setPx(src->px());
    dist->setPy(src->py());
    dist->setPz(src->pz());
    dist->setkalRechi2(src->kalRechi2());
    dist->setkalDof(src->kaldof());
    dist->setkalDepth(src->kaldepth());
    dist->setkalbrLastLayer(src->kalbrLastLayer());
    dist->setkalecLastLayer(src->kalecLastLayer());
}

#endif