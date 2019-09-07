//
//    Calculat shower shape.
//
//    Created by He Miao 2008, 1, 12
//
#ifndef EMC_REC_SHOWER_SHAPE_H
#define EMC_REC_SHOWER_SHAPE_H

class RecEmcShower;

class EmcRecShowerShape
{
 public:
   void CalculateMoment(RecEmcShower& aShower) const;
   void SecondMoment(RecEmcShower& aShower) const;
   void LatMoment(RecEmcShower& aShower) const;
   void A20Moment(RecEmcShower& aShower) const;
   void A42Moment(RecEmcShower& aShower) const;
};

#endif // EMC_REC_SHOWER_SHAPE_H

