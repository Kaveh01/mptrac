# Massive-Parallel Trajectory Calculations (MPTRAC)

Massive-Parallel Trajectory Calculations (MPTRAC) is a Lagrangian particle dispersion model for the analysis of atmospheric transport processes in the troposphere and stratosphere.

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/slcs-jsc/mptrac)](https://github.com/slcs-jsc/mptrac/releases)
[![GitHub commits since latest release (by SemVer)](https://img.shields.io/github/commits-since/slcs-jsc/mptrac/latest)](https://github.com/slcs-jsc/mptrac/commits/master)
[![GitHub last commit](https://img.shields.io/github/last-commit/slcs-jsc/mptrac.svg)](https://github.com/slcs-jsc/mptrac/commits/master)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/slcs-jsc/mptrac.svg)](https://github.com/slcs-jsc/mptrac/tree/master/src)
[![GitHub top language](https://img.shields.io/github/languages/top/slcs-jsc/mptrac.svg)](https://github.com/slcs-jsc/mptrac/tree/master/src)
[![GitHub](https://img.shields.io/github/license/slcs-jsc/mptrac.svg)](https://github.com/slcs-jsc/mptrac/blob/master/COPYING)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4400597.svg)](https://doi.org/10.5281/zenodo.4400597)

## Features

* MPTRAC calculates air parcel trajectories by solving the kinematic equation of motion using given wind fields.
* Mesoscale diffusion and sub-grid scale wind fluctuations are simulated using a Markov chain model.
* Additional modules are implemented to simulate convection, sedimentation, radioactive decay, hydroxyl chemistry, dry deposition, and wet deposition.
* Various output methods for particle, ensemble, gridded, and station data. Gnuplot interface for direct visualization.
* MPTRAC features an MPI/OpenMP/OpenACC hybrid parallelization for efficient use on HPC and GPU systems.

## Getting started

### Prerequisites

This documentation describes the installation of MPTRAC on a Linux system. A number of standard tools (gcc, git, make, and wget) and software libraries are needed to install MPTRAC. The [GNU Scientific Library](https://www.gnu.org/software/gsl) is required for numerical calculations and the [Unidata netCDF library](http://www.unidata.ucar.edu/software/netcdf) is needed for file-I/O. A script is provided to download these libraries from the Internet to compile them for usage with MPTRAC.

Start by downloading the MPTRAC source code from the git repository:

    git clone https://github.com/slcs-jsc/mptrac.git

To update an existing installation use:

    git pull https://github.com/slcs-jsc/mptrac.git

### Installation

First, download and compile the GSL and netCDF libraries required by MPTRAC by running the library build script:

    cd mptrac/libs
    ./build.sh

Next, change to the source directory and edit the Makefile according to your needs.

    cd mptrac/src
    emacs Makefile

You may want to edit the LIBDIR and INCDIR paths to point to the directories where the GSL and netCDF libraries are located on your system. By default, LIBDIR and INCDIR will point to the directories created by the library build script.

To make use of the MPI parallelization of MPTRAC, the MPI flag needs to be uncommented in the Makefile. Further steps of the installation will require an MPI library to be installed or loaded as a module. To make use of the OpenACC parallelization, the GPU flag needs to be uncommented. The PGI Compiler Suite will be required for to compile the GPU code. The OpenMP parallelization of MPTRAC is always enabled.

Load any modules that are needed on your target platform, and try to compile the code:

    make [-j8]

The argument -j is optional. It can be used to specify the number of threads for parallel compilation.

After compilation, the binaries can be found in the mptrac/src/ directory.

By default, the binaries will be linked statically, i.e., they can be copied and run on other machines. However, sometimes static compilations causes problems, in particular in combination with MPI and OpenACC. In this case, remove the '-static' flag from the CFLAGS in the Makefile and compile again. To run dynamically linked binaries, the LD_LIBRARY_PATH needs to be set to include the mptrac/libs/build/lib directory.

By default rather strict compiler warnings are used to catch problems. Furthermore, all warning messages will be turned into errors and no binaries will be produced. This behavior is enforced by the flag '-Werror' in the CFLAGS. It should not be removed from the Makefile, unless you know what you are doing.

### Run the example

An example is provided, illustrating how to simulate the dispersion of volcanic ash from the eruption of the Puyehue-Cordón Caulle volcano, Chile, in June 2011.

It is recommended that you create a project directory for testing the example and also to store the results of other experiments:

    mkdir -p mptrac/projects
    cp -a mptrac/example mptrac/projects

This shows how to run the example:

    cd mptrac/projects/example
    ./run.sh

At first call, the run script will download meteorological input data from a data server. This step may take a while as the meteorological input data comprise several hundred MBytes. The input data are saved for later runs and need to be retrieved only once.

Please see the example script (run.sh) on how to invoke MPTRAC programs such as 'atm_init' and 'atm_split' to initialize trajectory seeds and 'trac' to calculate the trajectories.

The script generates a number of plots of the simulation output at different times after the eruption by means of 'gnuplot'. These plots should look similar to the output already provided in the repository.

This is an example showing the particle position and grid output for 5-8 June 2011:
<p align="center"><img src="docs/images/atm_diff_2011_06_06_00_00.tab.png" width="45%"/> &nbsp; <img src="docs/images/grid_diff_2011_06_06_00_00.tab.png" width="45%"/></p>
<p align="center"><img src="docs/images/atm_diff_2011_06_08_00_00.tab.png" width="45%"/> &nbsp; <img src="docs/images/grid_diff_2011_06_08_00_00.tab.png" width="45%"/></p>

## Further information

More detailed information is provided in the [GitHub wiki](https://github.com/slcs-jsc/mptrac/wiki), the [reference manual](https://github.com/slcs-jsc/mptrac/blob/master/docs/refman.pdf), and the [Sphinx documentation](http://griessbach1.pages.jsc.fz-juelich.de/mptrac-user-documentation).

These are the main references for citing the MPTRAC model in scientific publications:

* Hoffmann, L., T. Rößler, S. Griessbach, Y. Heng, and O. Stein, Lagrangian transport simulations of volcanic sulfur dioxide emissions: Impact of meteorological data products, J. Geophys. Res. Atmos., 121, 4651-4673, https://doi.org/10.1002/2015JD023749, 2016. 

* You can cite the source code of MPTRAC by using the DOI https://doi.org/10.5281/zenodo.4400597. This DOI represents all versions, and will always resolve to the latest one. Specific DOIs for each release of MPTRAC can be found on the zenodo web site.

A list of selected papers can be found in the [GitHub wiki](https://github.com/slcs-jsc/mptrac/wiki/References).

## Contributing

We are interested in sharing MPTRAC for operational or research applications.

Please do not hesitate to contact us, if you have any further questions or need support.

## License

MPTRAC is distributed under the [GNU General Public License v3.0](https://github.com/slcs-jsc/mptrac/blob/master/COPYING).

## Contact

Lars Hoffmann  

Jülich Supercomputing Centre, Forschungszentrum Jülich

e-mail: l.hoffmann@fz-juelich.de

website: https://www.fz-juelich.de/ias/jsc/slcs
