
#ifndef PSI2D_RELDEP_H
#define PSI2D_RELDEP_H

#include <vector>

#include "Tools.h"
#include "PicParams.h"
#include "InputData.h"
#include "Species.h"
#include "PSI2D.h"

// PSI class for Reflection and Deposition
class PSI2D_RefDep : public PSI2D
{

public:
    //! Constructor
    PSI2D_RefDep(PicParams& params, SmileiMPI* smpi);
    ~PSI2D_RefDep();



    //! Method called in the main smilei loop to apply PSI at each timestep
    void performPSI(PicParams&,std::vector<Species*>&,int);

private:


};


#endif
