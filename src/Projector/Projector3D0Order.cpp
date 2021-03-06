#include "Projector3D0Order.h"

#include <cmath>
#include <iostream>

#include "ElectroMagn.h"
#include "Field3D.h"
#include "Particles.h"
#include "Tools.h"
#include "SmileiMPI_Cart3D.h"

using namespace std;


// ---------------------------------------------------------------------------------------------------------------------
// Constructor for Projector3D0Order
// ---------------------------------------------------------------------------------------------------------------------
Projector3D0Order::Projector3D0Order (PicParams& params, SmileiMPI* smpi) : Projector3D(params, smpi)
{
    SmileiMPI_Cart3D* smpi3D = static_cast<SmileiMPI_Cart3D*>(smpi);

    dx_inv_   = 1.0/params.cell_length[0];
    dx_ov_dt  = params.cell_length[0] / params.timestep;
    dy_inv_   = 1.0/params.cell_length[1];
    dy_ov_dt  = params.cell_length[1] / params.timestep;
    dz_inv_   = 1.0/params.cell_length[2];
    dz_ov_dt  = params.cell_length[2] / params.timestep;

    one_third = 1.0/3.0;

    i_domain_begin = smpi3D->getCellStartingGlobalIndex(0);
    j_domain_begin = smpi3D->getCellStartingGlobalIndex(1);
    k_domain_begin = smpi3D->getCellStartingGlobalIndex(2);

    DEBUG("cell_length "<< params.cell_length[0]);

}


// ---------------------------------------------------------------------------------------------------------------------
// Destructor for Projector3D0Order
// ---------------------------------------------------------------------------------------------------------------------
Projector3D0Order::~Projector3D0Order()
{
}


//! Below, in this order :
//!   Project global current densities (EMfields->Jx_/Jy_/Jz_), not used
//!   Projection by species
//!   Project global current charge
//!   Project local current densities (sort)
//!   Project global current densities (ionize)




// ---------------------------------------------------------------------------------------------------------------------
//! Project global current densities (EMfields->Jx_/Jy_/Jz_), not used
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (ElectroMagn* EMfields, Particles &particles, int ipart, double gf)
{

} // END Project global current densities, not used


// ---------------------------------------------------------------------------------------------------------------------
//!   Projection by species
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (Field* Jx, Field* Jy, Field* Jz, Field* rho, Particles &particles, int ipart, double gf)
{

}//END Projection by species



// ---------------------------------------------------------------------------------------------------------------------
//! Project global current charge
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (Field* rho, Particles &particles, int ipart, double weight)
{

    //Static cast of the total charge density
    Field3D* rho3D  = static_cast<Field3D*>(rho);

    //Declaration of local variables
    double delta, delta2;
    double rho_p = weight;   // charge density of the macro-particle
    double Sx[2], Sy[2], Sz[2];             // projection coefficient arrays

    //Locate particle on the primal grid & calculate the projection coefficients
    double       xpn = particles.position(0, ipart) * dx_inv_;  // normalized distance to the first node
    int ic  = floor(xpn);                   // index of the central node
    delta  = xpn - (double)ic;                       // normalized distance to the nearest grid point
    int iloc;
    if(delta < 0.5)
    {
        iloc = 0;
    }
    else
    {
        iloc = 1;
    }

    double       ypn = particles.position(1, ipart) * dy_inv_;  // normalized distance to the first node
    int jc   = floor(ypn);                  // index of the central node
    delta  = ypn - (double)jc;                       // normalized distance to the nearest grid point
    int jloc;
    if(delta < 0.5)
    {
        jloc = 0;
    }
    else
    {
        jloc = 1;
    }

    double       zpn = particles.position(2, ipart) * dz_inv_;  // normalized distance to the first node
    int kc   = floor(zpn);                  // index of the central node
    delta  = zpn - (double)kc;                       // normalized distance to the nearest grid point
    int kloc;
    if(delta < 0.5)
    {
        kloc = 0;
    }
    else
    {
        kloc = 1;
    }

    //cout << "Pos = " << particles.position(0, ipart) << " - i global = " << i << " - i local = " << i-index_domain_begin <<endl;

    int i = ic-i_domain_begin; // index of first point for projection in x
    int j = jc-j_domain_begin; // index of first point for projection in y
    int k = kc-k_domain_begin; // index of first point for projection in y

    // 1nd order projection for the total charge density

    DEBUGEXEC
    (
        if(i >= rho3D->dims_[0] || j >= rho3D->dims_[1] || k >= rho3D->dims_[2])
        {
            ERROR("Project Error, positions are: "<<particles.position(0, ipart)<<" "<<particles.position(1, ipart)<<" "<<particles.position(2, ipart));
            //ERROR("Project Error, ic, jc, kc are: "<<ic<<" "<<jc<<" "<<kc);
            //ERROR("Project Error, i, j, k are: "<<i<<" "<<j<<" "<<k);
            //ERROR("Project Error, i_domain_begin are: "<<i_domain_begin<<" "<<j_domain_begin<<" "<<k_domain_begin);
        }
    )
    (*rho3D)(i+iloc, j+jloc, k+kloc) += rho_p;


} // END Project global current charge



// ---------------------------------------------------------------------------------------------------------------------
//! Project global current charge
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (Field* rho, Particles &particles, int ipart)
{

} // END Project global current charge


// ---------------------------------------------------------------------------------------------------------------------
//! Project local current densities (sort)
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (double* Jx, double* Jy, double* Jz, double* rho, Particles &particles, int ipart, double gf, unsigned int bin, unsigned int b_dim1)
{

} // END Project local current densities (sort)


// ---------------------------------------------------------------------------------------------------------------------
//! Project global current densities (ionize)
// ---------------------------------------------------------------------------------------------------------------------
void Projector3D0Order::operator() (Field* Jx, Field* Jy, Field* Jz, Particles &particles, int ipart, LocalFields Jion)
{
    ERROR("Projection of ionization current not yet defined for 3D 2nd order");

} // END Project global current densities (ionize)
