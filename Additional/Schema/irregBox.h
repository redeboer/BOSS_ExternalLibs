//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: irregBox.h,v 1.1 2003/10/29 03:17:47 rado Exp $
// GEANT4 tag $Name: GDML_0_1_0 $
//
// 
// --------------------------------------------------------------
// Comments
//
// --------------------------------------------------------------
//
#ifndef IRREGBOX_H
#define IRREGBOX_H 1

#include "Saxana/SAXObject.h"
#include "Schema/SolidType.h"

class irregBox :  public SolidType {
public:
  irregBox() {
  }
  virtual ~irregBox() {
  }
  virtual SAXObject::Type type() {
    return SAXObject::element;
  }
  
  const std::string& get_x1() const {
    return m_x1;
  }
  const std::string& get_x2() const {
    return m_x2;
  }
  const std::string& get_x3() const {
    return m_x3;
  }
  const std::string& get_x4() const {
    return m_x4;
  }
  const std::string& get_x5() const {
    return m_x5;
  }
  const std::string& get_x6() const {
    return m_x6;
  }
  const std::string& get_x7() const {
    return m_x7;
  }
  const std::string& get_x8() const {
    return m_x8;
  }
  const std::string& get_y1() const {
    return m_y1;
  }
  const std::string& get_y2() const {
    return m_y2;
  }
  const std::string& get_y3() const {
    return m_y3;
  }
  const std::string& get_y4() const {
    return m_y4;
  }
  const std::string& get_y5() const {
    return m_y5;
  }
  const std::string& get_y6() const {
    return m_y6;
  }
  const std::string& get_y7() const {
    return m_y7;
  }
  const std::string& get_y8() const {
    return m_y8;
  }
  const std::string& get_z1() const {
    return m_z1;
  }
  const std::string& get_z2() const {
    return m_z2;
  }
  const std::string& get_z3() const {
    return m_z3;
  }
  const std::string& get_z4() const {
    return m_z4;
  }
  const std::string& get_z5() const {
    return m_z5;
  }
  const std::string& get_z6() const {
    return m_z6;
  }
  const std::string& get_z7() const {
    return m_z7;
  }
  const std::string& get_z8() const {
    return m_z8;
  }
  
  void set_x1( const std::string& x1 ) {
    m_x1 = x1;
  }
  void set_x2( const std::string& x2 ) {
    m_x2 = x2;
  }
  void set_x3( const std::string& x3 ) {
    m_x3 = x3;
  }
  void set_x4( const std::string& x4 ) {
    m_x4 = x4;
  }
  void set_x5( const std::string& x5 ) {
    m_x5 = x5;
  }
  void set_x6( const std::string& x6 ) {
    m_x6 = x6;
  }
  void set_x7( const std::string& x7 ) {
    m_x7 = x7;
  }
  void set_x8( const std::string& x8 ) {
    m_x8 = x8;
  }
  void set_y1( const std::string& y1 ) {
    m_y1 = y1;
  }
  void set_y2( const std::string& y2 ) {
    m_y2 = y2;
  }
  void set_y3( const std::string& y3 ) {
    m_y3 = y3;
  }
  void set_y4( const std::string& y4 ) {
    m_y4 = y4;
  }
  void set_y5( const std::string& y5 ) {
    m_y5 = y5;
  }
  void set_y6( const std::string& y6 ) {
    m_y6 = y6;
  }
  void set_y7( const std::string& y7 ) {
    m_y7 = y7;
  }
  void set_y8( const std::string& y8 ) {
    m_y8 = y8;
  }
  void set_z1( const std::string& z1 ) {
    m_z1 = z1;
  }
  void set_z2( const std::string& z2 ) {
    m_z2 = z2;
  }
  void set_z3( const std::string& z3 ) {
    m_z3 = z3;
  }
  void set_z4( const std::string& z4 ) {
    m_z4 = z4;
  }
  void set_z5( const std::string& z5 ) {
    m_z5 = z5;
  }
  void set_z6( const std::string& z6 ) {
    m_z6 = z6;
  }
  void set_z7( const std::string& z7 ) {
    m_z7 = z7;
  }
  void set_z8( const std::string& z8 ) {
    m_z8 = z8;
  }
  
private:
  std::string m_x1, m_x2, m_x3, m_x4, m_x5, m_x6, m_x7, m_x8;
  std::string m_y1, m_y2, m_y3, m_y4, m_y5, m_y6, m_y7, m_y8;
  std::string m_z1, m_z2, m_z3, m_z4, m_z5, m_z6, m_z7, m_z8;
};



#endif // IRREGBOX_H
