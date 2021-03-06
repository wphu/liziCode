#include "Collisions1D_Ionization.h"
#include "SmileiMPI.h"
#include "Field2D.h"

#include <cmath>
#include <iomanip>
#include <algorithm>
#include <ostream>
#include <sstream>

using namespace std;


// Constructor
Collisions1D_Ionization::Collisions1D_Ionization(PicParams& params, vector<Species*>& vecSpecies, SmileiMPI* smpi,
                       unsigned int n_col,
                       vector<unsigned int> sg1,
                       vector<unsigned int> sg2,
                       vector<unsigned int> sg3,
                       string CS_fileName)
: Collisions1D(params)
{

    n_collisions    = n_col;
    species_group1  = sg1;
    species_group2  = sg2;
    species_group3  = sg3;
    crossSection_fileName = CS_fileName;

    // Calculate total number of bins
    nbins = vecSpecies[0]->bmin.size();
    totbins = nbins;

    readCrossSection();
    energy_ionization_threshold = crossSection[0][0];

}

Collisions1D_Ionization::~Collisions1D_Ionization()
{

}


// Calculates the collisions for a given Collisions1D object
void Collisions1D_Ionization::collide(PicParams& params, SmileiMPI* smpi, ElectroMagn* fields, vector<Species*>& vecSpecies, Diagnostic* diag, int itime)
{
    vector<unsigned int> *sg1, *sg2, *sg3;

    vector<int> index1, index2;
    vector<int> n1, n2;
    vector<double> density1, density2;
    double n1_max, n2_max;
    vector<double> momentum_unit(3, 0.0), momentum_temp(3, 0.0);
    int idNew;
    int totNCollision = 0;
    vector<int> bmin1, bmax1, bmin2, bmax2, bmin3, bmax3;
    unsigned int npairs; // number of pairs of macro-particles
    double npairs_double;
    unsigned int i1, i2, i3;
    Species   *s1, *s2, *s3;
    Particles *p1, *p2, *p3;
    double m1, m2, m3, m12, W1, W2, W3;

    double  sigma_cr, sigma_cr_max, ke1, ke_primary, ke_secondary,
            ran, P_collision;
    double  v_square, v_magnitude, v_magnitude_primary, v_magnitude_secondary;

    int iBin_global;
    double ke_radiative;

    Diagnostic1D *diag1D = static_cast<Diagnostic1D*>(diag);

    sg1 = &species_group1;
    sg2 = &species_group2;
    sg3 = &species_group3;

    // electons                         atoms or primary ions              ionized ions
    s1 = vecSpecies[(*sg1)[0]];      s2 = vecSpecies[(*sg2)[0]];        s3 = vecSpecies[(*sg3)[0]];
    p1 = &(s1->particles);           p2 = &(s2->particles);             p3 = &(s3->particles);
    m1 = s1->species_param.mass;     m2 = s2->species_param.mass;       m3 = s3->species_param.mass;
    W1 = s1->species_param.weight;   W2 = s2->species_param.weight;     W3 = s3->species_param.weight;
    bmin1 = s1->bmin;                bmin2 = s2->bmin;                  bmin3 = s3->bmin;
    bmax1 = s1->bmax;                bmax2 = s2->bmax;                  bmax3 = s3->bmax;


    count_of_particles_to_insert_s1.resize(nbins);
    count_of_particles_to_insert_s3.resize(nbins);
    count_of_particles_to_erase_s2.resize(nbins);
    for(int ibin=0; ibin<nbins; ibin++)
    {
        count_of_particles_to_insert_s1[ibin] = 0;
        count_of_particles_to_insert_s3[ibin] = 0;
        count_of_particles_to_erase_s2[ibin] = 0;
    }


    n1.resize(nbins);
    density1.resize(nbins);
    n1_max = 0.0;
    for(unsigned int ibin=0 ; ibin<nbins ; ibin++)
    {
        n1[ibin] = bmax1[ibin] - bmin1[ibin];
    }

    n2.resize(nbins);
    density2.resize(nbins);
    n2_max = 0.0;
    for (unsigned int ibin=0 ; ibin<nbins ; ibin++)
    {
        n2[ibin] = bmax2[ibin] - bmin2[ibin];
        density2[ibin] = n2[ibin] * W2;
        if( density2[ibin] > n2_max ) { n2_max = density2[ibin]; };
    }

    totNCollision = 0;
    sigma_cr_max = maxCV(p1, m1);
    for (unsigned int ibin=0 ; ibin<nbins ; ibin++) {

        if(  smpi->getDomainLocalMin(0) + (ibin+1) * params.cell_length[0] < params.region_collision_zoom[0]
          || smpi->getDomainLocalMin(0) + ibin * params.cell_length[0] > params.region_collision_zoom[1] )
        {
            collision_zoom_factor = 1.0;
        }
        else
        {
            collision_zoom_factor = params.collision_zoom_factor;
        }

        //MESSAGE("nbins000"<<"  "<<ibin<<"  "<<bmin2[ibin]<<" "<<bmax2[ibin]);
        //>calculate the particle number of species1 in each cell, and the indexs of particles in the cell
        index1.resize( n1[ibin] );

        for(int iPart = 0; iPart < n1[ibin]; iPart++)
        {
            index1[iPart] = bmin1[ibin] + iPart;
        }
        random_shuffle(index1.begin(), index1.end());

        //>calculate the particle number of species2 in each cell, and the indexs of particles in the cell
        index2.resize( n2[ibin] );

        for(int iPart = 0; iPart < n2[ibin]; iPart++)
        {
            index2[iPart] = bmin2[ibin] + iPart;
        }
        random_shuffle(index2.begin(), index2.end());

        //smpi->barrier();
        //MESSAGE("nbinsaaaa"<<"  "<<ibin<<"  "<<n1[ibin]<<" "<<n2[ibin]);
        // Now start the real loop
        // See equations in http://dx.doi.org/10.1063/1.4742167
        // ----------------------------------------------------
        npairs_double = n1[ibin] * (1 - exp(-density2[ibin] * sigma_cr_max * timesteps_collision * timestep * collision_zoom_factor) );
        npairs = npairs_double;
        npairsRem[ibin] += ( npairs_double - npairs );
        if(npairsRem[ibin] >= 1.0)
        {
            npairsRem[ibin] = npairsRem[ibin] - 1.0;
            npairs++;
        }

        if(npairs > n1[ibin])
        {
            cout<<"npairs is larger than the particle number in a cell!!!"<<endl;
            cout<<"npairs, n1 are: "<<npairs<<" "<<n1[ibin]<<endl;
            npairs = n1[ibin];
        }
        if(npairs > n2[ibin])
        {
            cout<<"npairs is larger than the particle number in a cell!!!"<<endl;
            cout<<"npairs, n2 are: "<<npairs<<" "<<n2[ibin]<<endl;
            npairs = n2[ibin];
        }

        for(int i = 0; i < npairs; i++)
        {
            //MESSAGE("nparis111"<<"  "<<i);
            i1 = index1[i];
            i2 = index2[i];

            v_square = pow(p1->momentum(0,i1),2) + pow(p1->momentum(1,i1),2) + pow(p1->momentum(2,i1),2);
            v_magnitude = sqrt(v_square);
            //>kinetic energy of species1 (incident electrons)
            ke1 = 0.5 * m1 * v_square;
            ke_primary = ke1 - energy_ionization_threshold * const_e;

            //> the energy of the secondary electron
            ran = (double)rand() / RAND_MAX;
            ke_secondary = 10.0 * tan(ran * atan( (ke_primary/const_e) / 20.0));
            ke_secondary *= const_e;
            //> the energy of the primary electron
            ke_primary -= ke_secondary;
            v_magnitude_primary = sqrt( 2.0 * ke_primary / m1 );
            v_magnitude_secondary = sqrt( 2.0 * ke_secondary / m1 );

            sigma_cr = v_magnitude * interpCrossSection( ke1 / const_e );
            P_collision = sigma_cr / sigma_cr_max;
            // Generate a random number between 0 and 1
            double ran_p = (double)rand() / RAND_MAX;

            if(ran_p < P_collision){
                count_of_particles_to_erase_s2[ibin]++;
                //>calculate the scatter velocity of primary electron
                momentum_unit[0] = p1->momentum(0,i1) / v_magnitude;
                momentum_unit[1] = p1->momentum(1,i1) / v_magnitude;
                momentum_unit[2] = p1->momentum(2,i1) / v_magnitude;
                //MESSAGE("v_magnitude"<<"  "<<v_magnitude_primary<<"  "<<v_magnitude_secondary);
                //MESSAGE("momentum1"<<" "<<p1->momentum(0, i1)<<"  "<<p1->momentum(1, i1)<<"  "<<p1->momentum(2, i1));
                calculate_scatter_velocity(ke_primary/const_e, v_magnitude_primary, m1, m2, momentum_unit, momentum_temp);
                p1->momentum(0,i1) = momentum_temp[0];
                p1->momentum(1,i1) = momentum_temp[1];
                p1->momentum(2,i1) = momentum_temp[2];

                //>calculate the scatter velocity of secondary electron
                calculate_scatter_velocity(ke_secondary/const_e, v_magnitude_secondary, m1, m2, momentum_unit, momentum_temp);
                //>create new particle in the end of p1, we should sort_part when all bins are done!!!
                p1->cp_particle(i1, new_particles1);

                idNew = new_particles1.size() - 1;
                new_particles1.momentum(0, idNew) = momentum_temp[0];
                new_particles1.momentum(1, idNew) = momentum_temp[1];
                new_particles1.momentum(2, idNew) = momentum_temp[2];
                count_of_particles_to_insert_s1[ibin]++;


                DEBUGEXEC(
                    for(int idirection = 0; idirection < 3; idirection++)
                    {
                        if( isnan(p1->momentum(idirection,i1)) || isinf(p1->momentum(idirection,i1)) )
                        {
                            cout<<"Ionization Error1: Species: "<<s1->species_param.species_type<<" momentum "<<p1->momentum(idirection,i1)<<endl;
                        }

                        if( isnan(p1->momentum(idirection,idNew)) || isinf(p1->momentum(idirection,idNew)) )
                        {
                            cout<<"Ionization Error2: Species: "<<s1->species_param.species_type<<" momentum "<<p1->momentum(idirection,idNew)<<endl;
                        }
                    }
                );

                p2->cp_particle(i2, new_particles3);
                //>create the ionized ion (species3)
                idNew = new_particles3.size() - 1;
                new_particles3.charge(idNew) = s3->species_param.charge;
                count_of_particles_to_insert_s3[ibin]++;

                indexes_of_particles_to_erase_s2.push_back(i2);
                totNCollision++;

                iBin_global = smpi->getDomainLocalMin(0) / params.cell_length[0] + ibin;
                diag1D->radiative_energy_collision[n_collisions][iBin_global] += energy_ionization_threshold;
            } // end if
        }

    } // end loop on bins


    s2->erase_particles_from_bins(indexes_of_particles_to_erase_s2);
    indexes_of_particles_to_erase_s2.clear();

    s1->insert_particles_to_bins(new_particles1, count_of_particles_to_insert_s1);
    s3->insert_particles_to_bins(new_particles3, count_of_particles_to_insert_s3);
    new_particles1.clear();
    new_particles3.clear();

}

double Collisions1D_Ionization::maxCV(Particles* particles, double eMass)
{
    int nPart = particles->size();
    double v_square;
    double v_magnitude;
    double ke;
    double maxCrossSectionV = 0.0;
    double crossSectionV;

    maxCrossSectionV = 0.0;
    for(unsigned int iPart = 0; iPart < nPart; iPart++)
    {
        v_square = particles->momentum(0,iPart) * particles->momentum(0,iPart) +
                          particles->momentum(1,iPart) * particles->momentum(1,iPart) +
                          particles->momentum(2,iPart) * particles->momentum(2,iPart);
        v_magnitude = sqrt(v_square);
        // ke is energy (eV)
        ke = 0.5 * eMass * v_square / const_e;
        crossSectionV = v_magnitude * interpCrossSection( ke );
        if(crossSectionV > maxCrossSectionV) {maxCrossSectionV = crossSectionV;};
    }
    return maxCrossSectionV;
}





double Collisions1D_Ionization::cross_section(double ke)
{

}
