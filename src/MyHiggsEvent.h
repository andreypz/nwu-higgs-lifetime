#ifndef _HHH_H
#define _HHH_H

#include "TObject.h"
#include "TLorentzVector.h"

//using namespace std;

class MyHiggsEvent : public TObject {
 private:
  UInt_t _run;
  UInt_t _ls;
  UInt_t _evt;


  TLorentzVector _lep1_p4;
  TLorentzVector _lep2_p4;
  TLorentzVector _lep3_p4;
  TLorentzVector _lep4_p4;

 public:
  MyHiggsEvent();
  virtual ~MyHiggsEvent();


  UInt_t run() const;
  UInt_t ls()  const;
  UInt_t evt() const;

  TLorentzVector lep1() const;
  TLorentzVector lep2() const;
  TLorentzVector lep3() const;
  TLorentzVector lep4() const;


  void SetRun(UInt_t r);
  void SetLs(UInt_t l);
  void SetEvt(UInt_t e);

  void SetLep1(TLorentzVector p4);
  void SetLep2(TLorentzVector p4);
  void SetLep3(TLorentzVector p4);
  void SetLep4(TLorentzVector p4);
  ClassDef(MyHiggsEvent, 1);
};

#endif /* _HHH_H */
