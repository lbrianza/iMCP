#ifndef histo_func_h
#define histo_func_h

#include "TH1.h"


class histoFunc
{
 public:
  
  
  //! ctor
  histoFunc(TH1F* histo);
    
  //! dtor
  ~histoFunc()
  
  
  //! operator()
    double operator()(double* x, double* par);
  
   
 private:
  
  TH1F* histo_p;
};

#endif
