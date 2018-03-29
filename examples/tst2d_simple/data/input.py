# ----------------------------------------------------------------------------------------
# 					SIMULATION PARAMETERS FOR THE PIC-CODE SMILEI
# ----------------------------------------------------------------------------------------
#
# Remember: never override the following names:
#           SmileiComponent, Species, Laser, Collisions, DiagProbe, DiagParticles,
#           DiagScalar, DiagPhase or ExtField
#
import math

method = 'explicit'

l0 = 1.0e-5  #(SI)
t0 = 1.0e-12
resx = 20.				# nb of cells in on laser wavelength
rest = 30.				# time of timestep in one optical cycle

wavelength_SI = 1.e-6



Lsim = [200.*l0,200.*l0]	# length of the simulation
Tsim = 10			# duration of the simulation

#> number of timestep of incrementing averaged electromagnetic fields
ntime_step_avg = 1000

#> Timestep to output some fields into hdf5 file
dump_step = 200000
timesteps_restore = dump_step

timesteps_collision = 20

timesteps_coulomb = 40

timesteps_DSMC = 40

is_calVDF = 0

# dim: Geometry of the simulation
#      1d3v = cartesian grid with 1d in space + 3d in velocity
#      2d3v = cartesian grid with 2d in space + 3d in velocity
#      3d3v = cartesian grid with 3d in space + 3d in velocity
#      2drz = cylindrical (r,z) grid with 3d3v particles
#
dim = '2d3v'

# order of interpolation
#
interpolation_order = 1

# SIMULATION BOX : for all space directions (use vector)
# cell_length: length of the cell
# sim_length: length of the simulation in units of the normalization wavelength
#
cell_length = [l0,l0]
sim_length  = Lsim

# SIMULATION TIME
# timestep: duration of the timestep
# sim_time: duration of the simulation in units of the normalization period
#
timestep = t0
n_time = Tsim

# ELECTROMAGNETIC BOUNDARY CONDITIONS
# bc_em_type_x/y/z : boundary conditions used for EM fields
#                    periodic = periodic BC (using MPI topology)
#                    silver-muller = injecting/absorbing BC
#                    reflective = consider the ghost-cells as a perfect conductor
#

bc_em_type_x = ['silver-muller']
bc_em_type_y = ['silver-muller']
bc_em_value_x = [0.0, 0.0]

B = 2.0
angle = 5.0 * math.pi / 180.0
Bx = B * math.sin(angle)
By = B * math.cos(angle)
Bz = 0.0
externB = [Bx, By, Bz]

ion_sound_velocity = math.sqrt( (20.0 * 1.6021766208e-19) / (2.0 * 1.67262158e-27) )
vx = ion_sound_velocity * math.sin(angle)
vy = ion_sound_velocity * math.cos(angle)
vz = 0.0


#Topology:
#number_of_procs: Number of MPI processes in each direction.
#clrw: width of a cluster in number of cell. Warning: clrw must divide nspace_win_x.
number_of_procs = [4, 5]


# RANDOM seed
# this is used to randomize the random number generator
random_seed = 0



# DEFINE ALL SPECIES
# species_type       = string, given name to the species (e.g. ion, electron, positron, test ...)
# initPosition_type  = string, "regular" or "random"
# initMomentum_type  = string "cold", "maxwell-juettner" or "rectangular"
# c_part_max         = float, factor on the memory reserved for the total number of particles
# mass               = float, particle mass in units of the electron mass
# dynamics_type      = string, type of species dynamics = "norm" or "rrLL"
# time_frozen        = float, time during which particles are frozen in units of the normalization time
# radiating          = boolean, if true, incoherent radiation calculated using the Larmor formula
# n_part_per_cell    = integer or function, number of particles/cell
# charge             = float or function, particle charge in units of the electron charge
# charge_density     = float or function, species charge density in units of the "critical" density
#     or nb_density for number density
# mean_velocity      = list of floats or functions, mean velocity in units of the speed of light
# temperature        = list of floats or functions, temperature in units of m_e c^2
# Predefined functions: constant, trapezoidal, gaussian, polygonal, cosine
#

Species(
	species_type = 'e',
	initPosition_type = 'random',
	initMomentum_type = 'maxwell',
	ionization_model = 'none',
	n_part_per_cell = 200,
	n_part_per_cell_for_weight = 200,
	c_part_max = 1.0,
	mass = 9.109382616e-31,
	charge = -1.6021766208e-19,
	nb_density = 1.0e19,
	temperature = [20],
	time_frozen = 0.,
	bc_part_type_west  = 'supp',
	bc_part_type_east  = 'supp',
	bc_part_type_south = 'refl',
	bc_part_type_north = 'refl'
)


Species(
	species_type = 'D1',
	initPosition_type = 'random',
	initMomentum_type = 'maxwell',
	ionization_model = 'none',
	n_part_per_cell = 200,
	n_part_per_cell_for_weight = 200,
	c_part_max = 1.0,
	mass = 2.0 * 1.67262158e-27,
	charge = 1.6021766208e-19,
	nb_density = 1.0e19,
	temperature = [20],
	time_frozen = 0.0,
	bc_part_type_west  = 'supp',
	bc_part_type_east  = 'supp',
	bc_part_type_south = 'refl',
	bc_part_type_north = 'refl'
)



Species(
	species_type = 'T1',
	initPosition_type = 'random',
	initMomentum_type = 'maxwell',
	ionization_model = 'none',
	n_part_per_cell = 0,
	n_part_per_cell_for_weight = 200,
	c_part_max = 1.0,
	mass = 3.0 * 1.67262158e-27,
	charge = 1.6021766208e-19,
	nb_density = 0.5e19,
	temperature = [20],
	time_frozen = 0.0,
	bc_part_type_west  = 'supp',
	bc_part_type_east  = 'supp',
	bc_part_type_south = 'refl',
	bc_part_type_north = 'refl'
)







# COLLISIONS
# species1    = list of strings, the names of the first species that collide
# species2    = list of strings, the names of the second species that collide
#               (can be the same as species1)
# coulomb_log = float, Coulomb logarithm. If negative or zero, then automatically computed.
'''
Collisions(
	species1 = ["e"],
	species2 = ["D1"],
	coulomb_log = 5,
	collisions_type = "coulomb"
)
Collisions(
	species1 = ["e"],
	species2 = ["e"],
	coulomb_log = 1,
	collisions_type = "coulomb"
)
Collisions(
	species1 = ["D1"],
	species2 = ["D1"],
	coulomb_log = 1,
	collisions_type = "coulomb"
)
'''