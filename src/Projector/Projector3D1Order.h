#ifndef PROJECTOR3D1ORDER_H
#define PROJECTOR3D1ORDER_H

#include "Projector3D.h"


class Projector3D1Order : public Projector3D {
public:
    Projector3D1Order(PicParams&, SmileiMPI* smpi);
    ~Projector3D1Order();

    //! Project global current densities (EMfields->Jx_/Jy_/Jz_)
    //! Not used for now
    void operator() (ElectroMagn* EMfields, Particles &particles, int ipart, double gf);

    // Projection by species, in Species::dynamics, ElectroMagn::initRhoJ
    void operator() (Field* Jx, Field* Jy, Field* Jz, Field* rho, Particles &particles, int ipart, double gf);

    //! Project global current charge (EMfields->rho_)
    //! Used in Species::dynamics if time_frozen
    void operator() (Field* rho, Particles &particles, int ipart, double weight);
    
    void operator() (Field* rho, Particles &particles, int ipart);


    //! Project local current densities if particles sorting activated in Species::dynamics
    void operator() (double* Jx, double* Jy, double* Jz, double* rho, Particles &particles, int ipart, double gf, unsigned int bin, unsigned int b_dim1);

    //! Project global current densities if Ionization in Species::dynamics,
    void operator() (Field* Jx, Field* Jy, Field* Jz, Particles &particles, int ipart, LocalFields Jion);

private:
    double one_third;
};

#endif
