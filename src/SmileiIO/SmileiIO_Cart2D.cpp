/*
 * SmileiIO_Cart2D.cpp
 *
 *  Created on: 3 juil. 2013
 */

#include "SmileiIO_Cart2D.h"

#include <sstream>

#include "PicParams.h"
#include "SmileiMPI_Cart2D.h"
#include "Field2D.h"
#include "ElectroMagn.h"
#include "Species.h"

using namespace std;

SmileiIO_Cart2D::SmileiIO_Cart2D( PicParams& params, SmileiMPI* smpi, ElectroMagn* fields, vector<Species*>& vecSpecies, Diagnostic* diag )
: SmileiIO( params, smpi )
{
    Diagnostic2D* diag2D = static_cast<Diagnostic2D*>(diag);
    reloadP(params, smpi, vecSpecies);
    if(smpi->isMaster())
    {
        // create data patterns
        createFieldsPattern(params, fields);
        createDiagsPattern(params, diag2D);
        //createPartsPattern(params, fields, vecSpecies);
    }
}

SmileiIO_Cart2D::~SmileiIO_Cart2D()
{
}

//> create hdf5 file, datespace, dateset and so on
void SmileiIO_Cart2D::createFieldsPattern( PicParams& params, ElectroMagn* fields )
{
    fieldsGroup.dims_global[2] = params.n_space_global[1] + 1;
    fieldsGroup.dims_global[1] = params.n_space_global[0] + 1;
    fieldsGroup.dims_global[0] = 1;
 

    fieldsGroup.ndims_[0] = fieldsGroup.dims_global[0];
    fieldsGroup.ndims_[1] = fieldsGroup.dims_global[1];
    fieldsGroup.ndims_[2] = fieldsGroup.dims_global[2];

    fieldsGroup.offset[0] = 0;
    fieldsGroup.offset[1] = 0;
    fieldsGroup.offset[2] = 0;

    fieldsGroup.stride[0] = 1;
    fieldsGroup.stride[1] = 1;
    fieldsGroup.stride[2] = 1;

    fieldsGroup.block[0] = 1;
    fieldsGroup.block[1] = 1;
    fieldsGroup.block[2] = 1;

    // For attribute
    fieldsGroup.aDims = 3;

    createFieldsGroup(fields);


} // END createPattern




void SmileiIO_Cart2D::createPartsPattern( PicParams& params, ElectroMagn* fields, vector<Species*>& vecSpecies )
{
    // For particles, size ofdims_global should be 5: dims_global[nx][ny][nz][nvelocity][ntime]
    // But to be simple, the size is set 4, nz dimension is deleted.
    ptclsGroup.dims_global[3] = vx_dim;
    ptclsGroup.dims_global[2] = params.n_space_global[0];
    ptclsGroup.dims_global[1] = params.n_space_global[1];
    ptclsGroup.dims_global[0] = params.n_time / params.dump_step;

    ptclsGroup.ndims_[0] = ptclsGroup.dims_global[0];
    ptclsGroup.ndims_[1] = ptclsGroup.dims_global[1];
    ptclsGroup.ndims_[2] = ptclsGroup.dims_global[2];
    ptclsGroup.ndims_[3] = ptclsGroup.dims_global[3];

    ptclsGroup.offset[0] = 0;
    ptclsGroup.offset[1] = 0;
    ptclsGroup.offset[2] = 0;
    ptclsGroup.offset[3] = 0;

    ptclsGroup.stride[0] = 1;
    ptclsGroup.stride[1] = 1;
    ptclsGroup.stride[2] = 1;
    ptclsGroup.stride[3] = 1;

    ptclsGroup.block[0] = 1;
    ptclsGroup.block[1] = 1;
    ptclsGroup.block[2] = 1;
    ptclsGroup.block[3] = 1;

    ptclsGroup.aDims = 4;

    createPartsGroup(vecSpecies);

}


void SmileiIO_Cart2D::createDiagsPattern(PicParams& params, Diagnostic2D* diag2D)
{
    string diag_name;
    const char* h5_name;
    hid_t dataset_id;
    int n_dim_data = 3;

    // =======set stride and block, and close dataset and group=================
    diagsGroup.stride[0] = 1;
    diagsGroup.stride[1] = 1;
    diagsGroup.stride[2] = 1;


    diagsGroup.block[0] = 1;
    diagsGroup.block[1] = 1;
    diagsGroup.block[2] = 1;

    diagsGroup.count[0]  = 1;
    diagsGroup.count[1]  = diag2D->dim_global[0];
    diagsGroup.count[2]  = diag2D->dim_global[1];


    // ======= create diagsGroup ================================
    diag_name = "particleFlux";
    diagsGroup.dataset_stringName.push_back(diag_name);

    diag_name = "heatFlux";
    diagsGroup.dataset_stringName.push_back(diag_name);

    diag_name = "averageAngle";
    diagsGroup.dataset_stringName.push_back(diag_name);

    diag_name = "psiRate";
    diagsGroup.dataset_stringName.push_back(diag_name);
    
    diagsGroup.dataset_id.resize( diagsGroup.dataset_stringName.size() );
}


void SmileiIO_Cart2D::initVDF( PicParams& params, SmileiMPI* smpi, ElectroMagn* fields, vector<Species*>& vecSpecies )
{
    vx_dim = 200;

    vector<unsigned int> dims_VDF;
    vector<unsigned int> dims_VDF_global;
    dims_VDF.resize(4);
    dims_VDF_global.resize(4);

    dims_VDF[0] = params.n_space[0];
    dims_VDF[1] = params.n_space[1];
    dims_VDF[2] = 1;
    dims_VDF[3] = vx_dim;

    dims_VDF_global[0] = params.n_space_global[0];
    dims_VDF_global[1] = params.n_space_global[1];
    dims_VDF_global[2] = 1;
    dims_VDF_global[3] = vx_dim;

    for(int isp=0; isp<vecSpecies.size(); isp++)
    {
        vx_VDF.push_back(new Array4D());
        vx_VDF[isp]->allocateDims(dims_VDF);

        vx_VDF_global.push_back(new Array4D());
        vx_VDF_global[isp]->allocateDims(dims_VDF_global);

        vx_VDF_tot_global.push_back(new Array4D());
        vx_VDF_tot_global[isp]->allocateDims(dims_VDF_global);
    }

}



void SmileiIO_Cart2D::calVDF( PicParams& params, SmileiMPI* smpi, ElectroMagn* fields, vector<Species*>& vecSpecies)
{
    Species *s;
    Particles *p;
    /*

    for(int isp=0; isp<vx_VDF.size(); isp++)
    {
        s = vecSpecies[isp];
        p = &(s->particles);

        vector<double> x_cell(3,0);
        x_cell[0] = 0;
        x_cell[1] = 0;
        x_cell[2] = 0;
        vxMax = 3*sqrt(2.0 * s->species_param.thermT[0] * params.const_e / s->species_param.mass);
        //WARNING("thermalVelocity" <<  s->species_param.thermT[0] );
        vxMin = -vxMax;
        vx_d = (vxMax - vxMin) / vx_dim;
        int vx_dim2 = vx_dim / 2;

        vx_VDF[isp]->put_to(0.0);
        for(int ibin = 0; ibin < ( s->bmin.size() ); ibin++)
        {
            for(int iPart = s->bmin[ibin]; iPart < s->bmax[ibin]; iPart++)
            {
                int ivx = p->momentum(0,iPart) / vx_d + vx_dim2;
                if( ivx < 0 ) {
                    ivx = 0;
                }
                if( ivx >= vx_dim ) {
                    ivx = vx_dim - 1;
                }
                (*vx_VDF[isp])(ibin,0,0,ivx) += 1.0;
            }
        }
        smpi->gatherVDF(vx_VDF_global[isp], vx_VDF[isp]);

        vx_VDF_tot_global[isp]->put_to(0.0);
        for (int ibin = 0; ibin < vx_VDF_global[isp]->dims_[0]; ibin++)
        {
            for (int ivx = 0; ivx < vx_VDF_global[isp]->dims_[3]; ivx++)
            {
                (*vx_VDF_tot_global[isp])(0,0,0,ivx) += (*vx_VDF_global[isp])(ibin,0,0,ivx);
            }

        }


    }
    */
}



//! write potential, rho and so on into hdf5 file every some timesteps
void SmileiIO_Cart2D::write( PicParams& params, SmileiMPI* smpi, ElectroMagn* fields, vector<Species*>& vecSpecies, Diagnostic* diag, int itime)
{
    const char* h5_name;
    int iDiag;
    int n_dim_data = 3;
    Diagnostic2D* diag2D = static_cast<Diagnostic2D*>(diag);
    if(params.is_calVDF)
    {
        //calVDF( params, smpi, fields, vecSpecies, itime);
    }

    if( itime % params.dump_step == 0 && smpi->isMaster() )
    {
        ndims_t = itime / params.dump_step - 1;
        long long ndims_t_temp = ndims_t;

        // create file at current output step
        data_file_name = "data/data" + to_string(ndims_t_temp) + ".h5";
        data_file_id = H5Fcreate( data_file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        // =============write fields============================================
        fieldsGroup.group_id = H5Gcreate(data_file_id, "/Fields", H5P_DEFAULT, H5P_DEFAULT,H5P_DEFAULT);
        for(int i = 0; i < fieldsGroup.dataset_stringName.size(); i++)
        {
            fieldsGroup.dataspace_id = H5Screate_simple(n_dim_data, fieldsGroup.dims_global, NULL);
            h5_name = fieldsGroup.dataset_stringName[i].c_str();
            fieldsGroup.dataset_id[i] = H5Dcreate2(fieldsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, fieldsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            fieldsGroup.status = H5Dwrite(fieldsGroup.dataset_id[i], H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, fieldsGroup.dataset_data[i]);
            fieldsGroup.status = H5Sclose(fieldsGroup.dataspace_id);
            fieldsGroup.status = H5Dclose(fieldsGroup.dataset_id[i]);
        }
        fieldsGroup.status = H5Gclose(fieldsGroup.group_id);


        // =============write Diagnostics ============================================
        diagsGroup.group_id = H5Gcreate(data_file_id, "/Diagnostic", H5P_DEFAULT, H5P_DEFAULT,H5P_DEFAULT);
        // particle flux
        iDiag = 0;
        diagsGroup.dims_global[0] = diag2D->particleFlux_global.size();
        diagsGroup.dims_global[1] = diag2D->dim_global[0];
        diagsGroup.dims_global[2] = diag2D->dim_global[1];
        diagsGroup.dataspace_id = H5Screate_simple(n_dim_data, diagsGroup.dims_global, NULL);
        h5_name = diagsGroup.dataset_stringName[iDiag].c_str();
        diagsGroup.dataset_id[iDiag] = H5Dcreate2(diagsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, diagsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        for(int iSpec = 0; iSpec < diag2D->n_species; iSpec++)
        {
            diagsGroup.offset[0] = iSpec;
            diagsGroup.offset[1] = 0;
            diagsGroup.offset[2] = 0;
            diagsGroup.memspace_id = H5Screate_simple(n_dim_data, diagsGroup.count, NULL);
            diagsGroup.status = H5Sselect_hyperslab(diagsGroup.dataspace_id, H5S_SELECT_SET, diagsGroup.offset,
                                                diagsGroup.stride, diagsGroup.count, diagsGroup.block);
            diagsGroup.status = H5Dwrite(diagsGroup.dataset_id[iDiag], H5T_NATIVE_DOUBLE, diagsGroup.memspace_id,
                                    diagsGroup.dataspace_id, H5P_DEFAULT, (diag2D->particleFlux_global[iSpec])->data_);
            diagsGroup.status = H5Sclose(diagsGroup.memspace_id);
            diagsGroup.status = H5Dclose(diagsGroup.dataset_id[iDiag]);
        }
        diagsGroup.status = H5Sclose(diagsGroup.dataspace_id);

        // heat flux
        iDiag++;
        diagsGroup.dataspace_id = H5Screate_simple(n_dim_data, diagsGroup.dims_global, NULL);
        h5_name = diagsGroup.dataset_stringName[iDiag].c_str();
        diagsGroup.dataset_id[iDiag] = H5Dcreate2(diagsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, diagsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        for(int iSpec = 0; iSpec < diag2D->n_species; iSpec++)
        {
            diagsGroup.offset[0] = iSpec;
            diagsGroup.offset[1] = 0;
            diagsGroup.offset[2] = 0;
            diagsGroup.memspace_id = H5Screate_simple(n_dim_data, diagsGroup.count, NULL);
            diagsGroup.status = H5Sselect_hyperslab(diagsGroup.dataspace_id, H5S_SELECT_SET, diagsGroup.offset,
                                                diagsGroup.stride, diagsGroup.count, diagsGroup.block);
            diagsGroup.status = H5Dwrite(diagsGroup.dataset_id[iDiag], H5T_NATIVE_DOUBLE, diagsGroup.memspace_id,
                                    diagsGroup.dataspace_id, H5P_DEFAULT, (diag2D->heatFlux_global[iSpec])->data_);
            diagsGroup.status = H5Sclose(diagsGroup.memspace_id);
            diagsGroup.status = H5Dclose(diagsGroup.dataset_id[iDiag]);
        }
        diagsGroup.status = H5Sclose(diagsGroup.dataspace_id);

        // average angle
        iDiag++;
        diagsGroup.dataspace_id = H5Screate_simple(n_dim_data, diagsGroup.dims_global, NULL);
        h5_name = diagsGroup.dataset_stringName[iDiag].c_str();
        diagsGroup.dataset_id[iDiag] = H5Dcreate2(diagsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, diagsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        for(int iSpec = 0; iSpec < diag2D->n_species; iSpec++)
        {
            diagsGroup.offset[0] = iSpec;
            diagsGroup.offset[1] = 0;
            diagsGroup.offset[2] = 0;
            diagsGroup.memspace_id = H5Screate_simple(n_dim_data, diagsGroup.count, NULL);
            diagsGroup.status = H5Sselect_hyperslab(diagsGroup.dataspace_id, H5S_SELECT_SET, diagsGroup.offset,
                                                diagsGroup.stride, diagsGroup.count, diagsGroup.block);
            diagsGroup.status = H5Dwrite(diagsGroup.dataset_id[iDiag], H5T_NATIVE_DOUBLE, diagsGroup.memspace_id,
                                    diagsGroup.dataspace_id, H5P_DEFAULT, (diag2D->averageAngle_global[iSpec])->data_);
            diagsGroup.status = H5Sclose(diagsGroup.memspace_id);
            diagsGroup.status = H5Dclose(diagsGroup.dataset_id[iDiag]);
        }
        diagsGroup.status = H5Sclose(diagsGroup.dataspace_id);

        // psiRate
        iDiag++;
        diagsGroup.dataspace_id = H5Screate_simple(n_dim_data, diagsGroup.dims_global, NULL);
        h5_name = diagsGroup.dataset_stringName[iDiag].c_str();
        diagsGroup.dataset_id[iDiag] = H5Dcreate2(diagsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, diagsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        for(int iPsi = 0; iPsi < diag2D->psiRate_global.size(); iPsi++)
        {
            diagsGroup.offset[0] = iPsi;
            diagsGroup.offset[1] = 0;
            diagsGroup.offset[2] = 0;
            diagsGroup.memspace_id = H5Screate_simple(n_dim_data, diagsGroup.count, NULL);
            diagsGroup.status = H5Sselect_hyperslab(diagsGroup.dataspace_id, H5S_SELECT_SET, diagsGroup.offset,
                                                diagsGroup.stride, diagsGroup.count, diagsGroup.block);
            diagsGroup.status = H5Dwrite(diagsGroup.dataset_id[iDiag], H5T_NATIVE_DOUBLE, diagsGroup.memspace_id,
                                    diagsGroup.dataspace_id, H5P_DEFAULT, (diag2D->psiRate_global[iPsi])->data_);
            diagsGroup.status = H5Sclose(diagsGroup.memspace_id);
            diagsGroup.status = H5Dclose(diagsGroup.dataset_id[iDiag]);
        }
        diagsGroup.status = H5Sclose(diagsGroup.dataspace_id);
        diagsGroup.status = H5Gclose(diagsGroup.group_id);

        /*
        // write particle velocity distribution function
        ptclsGroup.group_id = H5Gcreate(data_file_id, "/VDF", H5P_DEFAULT, H5P_DEFAULT,H5P_DEFAULT);
        for(int i = 0; i < ptclsGroup.dataset_id.size(); i++)
        {
            ptclsGroup.dataspace_id = H5Screate_simple(n_dim_data, ptclsGroup.dims_global, NULL);
            h5_name = ptclsGroup.dataset_stringName[i].c_str();
            ptclsGroup.dataset_id[i] = H5Dcreate2(ptclsGroup.group_id, h5_name, H5T_NATIVE_DOUBLE, ptclsGroup.dataspace_id,
                                                    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            ptclsGroup.status = H5Dwrite(ptclsGroup.dataset_id[i], H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ptclsGroup.dataset_data[i]);
            ptclsGroup.status = H5Sclose(ptclsGroup.dataspace_id);
            ptclsGroup.status = H5Dclose(ptclsGroup.dataset_id[i]);
        }
        ptclsGroup.status = H5Gclose( ptclsGroup.group_id );
        */
        status = H5Fclose(data_file_id);
    }



} // END write


void SmileiIO_Cart2D::writeGrid(Grid* grid)
{
    hid_t       grid_dataspace_id;
    hid_t       grid_dataset_id;
    herr_t      grid_status;
    string      grid_dataset_name;

    int grid_ndim = 3;
    hsize_t grid_dims_global[3];

    Grid2D* grid2D = static_cast<Grid2D*>(grid);
    grid_file_name  = "data/grid.h5";
    grid_file_id    = H5Fcreate( grid_file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    grid_dims_global[0] = grid2D->globalDims_[0];
    grid_dims_global[1] = grid2D->globalDims_[0];
    grid_dims_global[2] = grid2D->globalDims_[1];


    // =============write grid============================================
    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "isWall";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_INT, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, grid2D->iswall_global_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "bndr";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_INT, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, grid2D->bndr_global_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "bndrVal";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_INT, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, grid2D->bndrVal_global_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "normal_x";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_DOUBLE, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (grid2D->normal_x_global)->data_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "normal_y";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_DOUBLE, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (grid2D->normal_y_global)->data_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_dataspace_id = H5Screate_simple(grid_ndim, grid_dims_global, NULL);
    grid_dataset_name = "normal_z";
    grid_dataset_id = H5Dcreate2(grid_file_id, grid_dataset_name.c_str(), H5T_NATIVE_DOUBLE, grid_dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    grid_status = H5Dwrite(grid_dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (grid2D->normal_z_global)->data_);
    grid_status = H5Sclose(grid_dataspace_id);
    grid_status = H5Dclose(grid_dataset_id);

    grid_status = H5Fclose(grid_file_id);
}