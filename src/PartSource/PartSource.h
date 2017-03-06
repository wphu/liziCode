/*
PartSource class
*/

#ifndef PARTSOURCE_H
#define PARTSOURCE_H

#include <vector>
#include <string>

#include "PicParams.h"
#include "SmileiMPI.h"
#include "Species.h"

using namespace std;
class PartSource
{

public:
    //! Constructor for PSI between two species
    PartSource(PicParams& params, SmileiMPI* smpi)
    {
            count_of_particles_to_insert_s1.resize(params.n_space[0]);
            count_of_particles_to_insert.resize(params.n_space[0]);
            numPart_in_each_bin.resize(params.n_space[0]);

            new_particles.initialize(0, params);
            mean_velocity.resize(3, 0.0);
    };
    virtual ~PartSource(){};

    unsigned int species1;
    Particles new_particles;
    vector<int> count_of_particles_to_insert_s1;

    vector<int> numPart_in_each_bin;
    vector<int> count_of_particles_to_insert;
    vector<int> indexes_of_particles_to_erase;

    // Particle number in one cell, different from particle number in one bin for 2D case
    int numPart_in_each_cell;

    //! Method called in the main smilei loop to apply PartSource at each timestep
    virtual void emitLoad(PicParams&, SmileiMPI* smpi, std::vector<Species*>&,int, ElectroMagn* ){};

    vector<double> mean_velocity;


    // =================Parameters for emitting particles=================
    //! Identification number of the PartSource object
    int n_PartSource;

    // emit kind, regular or fieldEmit for injection PSI
    string emitKind;

    // PSI position : only left and right for 1D case
    string emitPos;

    // position offset of injected or sputtered particles
    double posOffset;
    // the energy/temperature of the new particles
    double emitTemp;
    double weight_const;

    // relevant PSI, emitting number of two species may be relevant
    // such as nPartEmit(A) = relCoff * nPartEmit(B)
    PartSource *relPartSource;
    string relSpecies;
    void setRelPartSource(PartSource* relevantPartSource)
    {
        relPartSource = relevantPartSource;
    }


    // =================Parameters for loading particles=================
    string loadKind;
    double loadDensity;
    double loadTemperature;
    // load density per second [m-3 s-1]
    double loadDn;
    int loadStep;
    // Number of particles loaded in one cell at one loadStep
    int loadNumber;
    // loadRem = loadStep * loadDn *... - loadNumber
    double loadRem;
    double loadRemTot;

    // Position for loading particles in the current MPI region!!!
    double loadPos_start;
    double loadPos_end;
    int loadBin_start;
    int loadBin_end;

    double loadPos_Ystart;
    double loadPos_Yend;
    int loadBin_Ystart;
    int loadBin_Yend;

    int everyTime;

private:

    double const_e;

};

#endif
