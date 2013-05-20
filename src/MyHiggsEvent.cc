#include "MyHiggsEvent.h"

MyHiggsEvent::MyHiggsEvent(){
}

MyHiggsEvent::~MyHiggsEvent(){
}

UInt_t MyHiggsEvent::run() const{
  return _run;
}
UInt_t MyHiggsEvent::ls()  const{
  return _ls;
}
UInt_t MyHiggsEvent::evt() const{
  return _evt;
}

TLorentzVector MyHiggsEvent::lep1() const{
  return _lep1_p4;
}
TLorentzVector MyHiggsEvent::lep2() const{
  return _lep2_p4;
}
TLorentzVector MyHiggsEvent::lep3() const{
  return _lep3_p4;
}
TLorentzVector MyHiggsEvent::lep4() const{
  return _lep4_p4;
}


void MyHiggsEvent::SetRun(UInt_t r){
  _run = r;
}
void MyHiggsEvent::SetLs(UInt_t l){
  _ls = l;
}
void MyHiggsEvent::SetEvt(UInt_t e){
  _evt = e;
}
void MyHiggsEvent::SetLep1(TLorentzVector p4){
  _lep1_p4 = p4;
}
void MyHiggsEvent::SetLep2(TLorentzVector p4){
  _lep2_p4 = p4;
}
void MyHiggsEvent::SetLep3(TLorentzVector p4){
  _lep3_p4 = p4;
}
void MyHiggsEvent::SetLep4(TLorentzVector p4){
  _lep4_p4 = p4;
}
