
#ifndef PARTSOURCE2D_LOAD_H
#define PARTSOURCE2D_LOAD_H

#include <vector>
#include "PartSource2D.h"
#include "Timer.h"


using namespace std;

class PartSource2D_Load : public PartSource2D
{

public:
    //! Constructor for Collisions between two species
    PartSource2D_Load(
    PicParams& params,
    SmileiMPI* smpi,
    unsigned int load_species1,
    vector<double> mean_vel,
    string load_kind,
    int    load_number,
    int    every_time,
    double load_dn,
    double load_density,
    double load_temperature,
    double load_Pos_start,
    double load_Pos_end,
    double load_Pos_Ystart,
    double load_Pos_Yend,
    int    load_step_update);

    ~PartSource2D_Load();



    //! Method called in the main smilei loop to apply collisions at each timestep
    void emitLoad(PicParams&, SmileiMPI* smpi, std::vector<Species*>&,int, ElectroMagn* );

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

    // Count timer
    vector<Timer> timer;







private:
    double dt_ov_dx;
    double dt;
    double YZArea;

    double weight_const;
    // nominalDensity and nomPtclsPerCell is used to set the weight_const
    // weight_cosnt = nominalDensity * CellVolume / nomPtclsPerCell
    double nominalDensity;
    double nomPtclsPerCell;
    // emitting tempreature


};


#endif
