/**
 * @file EFResult.h
 * @author <a href="mailto:karr@ifae.es> Kristo Karr </a> 
 *         <a href="mailto:merino@ifae.es> Gonzalo Merino </a> 
 * $Author: fucd $
 * $Revision: 1.3 $
 * $Date: 2008/10/29 02:23:27 $
 *
 * @brief Defines the EFResult RDO structure
 */
#ifndef EFRESULT_H
#define EFRESULT_H

#include <vector>
#include <string>
#include <stdint.h>

#include "GaudiKernel/DataObject.h"

//hxt #include "CLIDSvc/CLASS_DEF.h" 

static const CLID& CLID_EFResult=11006;

class EFResult: public DataObject
{
      
 public:

  /* Constructor: Fill possible answers list */
  EFResult(std::vector<std::string>);
  /* Constructor: Fill data members vector */
  EFResult(std::vector<uint32_t>,uint32_t);
  /* Empty constructor: Init to 0 */
  EFResult();
  /* Destructor */
  virtual ~EFResult();

  /*EFResult CLID accessor.*/
  static const CLID& classID();
  /*EFResult CLID accessor.*/
  virtual const  CLID& clID() const;

  /* Method returning the EFResult 32bit ints vector contents */
  const std::vector<uint32_t> getEFVec() const;
  const uint32_t getEFVec(uint32_t pos) const;

  /* Method appending 32bit ints to the EF vector contents */
  bool push_back(uint32_t val);
  bool appToEFVec(double val,uint32_t pos);
  bool addToEFVec(uint32_t val,uint32_t pos);
  bool setVecBit(uint32_t val,uint32_t vecpos,uint32_t bbegin,uint32_t bend);
  bool setVecBit(bool val,uint32_t vecpos,uint32_t bitpos);

  /* Method to store the EF answer */
  void setAnswer(std::string &answer);
  void setMidAnswer(std::string &answer);
  /////////////////////////////zoujh///////////////////
  void reset();

  /* Method to retrieve the EF answer string */
  const std::string getAnswer() const;
  const std::string getMidAnswer() const;

  /* Method to retrieve the EF answer (string vector) index */
  const int getAnswerIndex() const;
  const int getMidAnswerIndex() const;
  
  const int getDecNum() const{ 
    if(m_efVec.size()>=54)return m_efVec.size()-54;
    else return 0;
  }
 private:
  std::vector<std::string> m_answerVector;  ///< List of supported string answers
  int m_answerIndex; ///< Index of the chosen EF answer inside the List 
  int m_midanswerIndex;
  std::vector<uint32_t> m_efVec; ///< Vector of 32bit ints with EFResult data members  
};
  
//hxt  CLASS_DEF(EFResult ,11006,0)

#endif
