/*
  This file is part of MPTRAC.
  
  MPTRAC is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  MPTRAC is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with MPTRAC. If not, see <http://www.gnu.org/licenses/>.
  
  Copyright (C) 2013-2019 Forschungszentrum Juelich GmbH
*/

/*! 
  \file
  Extract map from meteorological data.
*/

#include "libtrac.h"

/* ------------------------------------------------------------
   Dimensions...
   ------------------------------------------------------------ */

/*! Maximum number of longitudes. */
#define NX 1441

/*! Maximum number of latitudes. */
#define NY 721

/* ------------------------------------------------------------
   Functions...
   ------------------------------------------------------------ */

void add_text_attribute(
  int ncid,
  char *varname,
  char *attrname,
  char *text);

/* ------------------------------------------------------------
   Main...
   ------------------------------------------------------------ */

int main(
  int argc,
  char *argv[]) {

  ctl_t ctl;

  met_t *met;

  static double pt[NX * NY], zt[NX * NY], tt[NX * NY], lon, lon0, lon1,
    lons[NX], dlon, lat, lat0, lat1, lats[NY], dlat;

  static int init, i, ix, iy, nx, ny, nt, ncid, dims[3], timid, lonid, latid,
    clppid, clptid, clpzid, dynpid, dyntid, dynzid, wmo1pid, wmo1tid, wmo1zid,
    wmo2pid, wmo2tid, wmo2zid;

  static size_t count[10], start[10];

  /* Allocate... */
  ALLOC(met, met_t, 1);

  /* Check arguments... */
  if (argc < 4)
    ERRMSG("Give parameters: <ctl> <tropo.nc> <met0> [ <met1> ... ]");

  /* Read control parameters... */
  read_ctl(argv[1], argc, argv, &ctl);
  lon0 = scan_ctl(argv[1], argc, argv, "MAP_LON0", -1, "-180", NULL);
  lon1 = scan_ctl(argv[1], argc, argv, "MAP_LON1", -1, "180", NULL);
  dlon = scan_ctl(argv[1], argc, argv, "MAP_DLON", -1, "-999", NULL);
  lat0 = scan_ctl(argv[1], argc, argv, "MAP_LAT0", -1, "-90", NULL);
  lat1 = scan_ctl(argv[1], argc, argv, "MAP_LAT1", -1, "90", NULL);
  dlat = scan_ctl(argv[1], argc, argv, "MAP_DLAT", -1, "-999", NULL);

  /* Loop over files... */
  for (i = 3; i < argc; i++) {

    /* Read meteorological data... */
    ctl.met_tropo = 0;
    if (!read_met(&ctl, argv[i], met))
      continue;

    /* Set horizontal grid... */
    if (!init) {
      init = 1;

      /* get grid... */
      if (dlon <= 0)
	dlon = fabs(met->lon[1] - met->lon[0]);
      if (dlat <= 0)
	dlat = fabs(met->lat[1] - met->lat[0]);
      if (lon0 < -360 && lon1 > 360) {
	lon0 = gsl_stats_min(met->lon, 1, (size_t) met->nx);
	lon1 = gsl_stats_max(met->lon, 1, (size_t) met->nx);
      }
      nx = ny = 0;
      for (lon = lon0; lon <= lon1; lon += dlon) {
	lons[nx] = lon;
	if ((++nx) > NX)
	  ERRMSG("Too many longitudes!");
      }
      if (lat0 < -90 && lat1 > 90) {
	lat0 = gsl_stats_min(met->lat, 1, (size_t) met->ny);
	lat1 = gsl_stats_max(met->lat, 1, (size_t) met->ny);
      }
      for (lat = lat0; lat <= lat1; lat += dlat) {
	lats[ny] = lat;
	if ((++ny) > NY)
	  ERRMSG("Too many latitudes!");
      }

      /* Create netCDF file... */
      printf("Write tropopause data file: %s\n", argv[2]);
      NC(nc_create(argv[2], NC_CLOBBER, &ncid));

      /* Create dimensions... */
      NC(nc_def_dim(ncid, "time", (size_t) NC_UNLIMITED, &dims[0]));
      NC(nc_def_dim(ncid, "lat", (size_t) ny, &dims[1]));
      NC(nc_def_dim(ncid, "lon", (size_t) nx, &dims[2]));

      /* Create variables... */
      NC(nc_def_var(ncid, "time", NC_DOUBLE, 1, &dims[0], &timid));
      NC(nc_def_var(ncid, "lat", NC_DOUBLE, 1, &dims[1], &latid));
      NC(nc_def_var(ncid, "lon", NC_DOUBLE, 1, &dims[2], &lonid));
      NC(nc_def_var(ncid, "clp_p", NC_FLOAT, 3, &dims[0], &clppid));
      NC(nc_def_var(ncid, "clp_t", NC_FLOAT, 3, &dims[0], &clptid));
      NC(nc_def_var(ncid, "clp_z", NC_FLOAT, 3, &dims[0], &clpzid));
      NC(nc_def_var(ncid, "dyn_p", NC_FLOAT, 3, &dims[0], &dynpid));
      NC(nc_def_var(ncid, "dyn_t", NC_FLOAT, 3, &dims[0], &dyntid));
      NC(nc_def_var(ncid, "dyn_z", NC_FLOAT, 3, &dims[0], &dynzid));
      NC(nc_def_var(ncid, "wmo_1st_p", NC_FLOAT, 3, &dims[0], &wmo1pid));
      NC(nc_def_var(ncid, "wmo_1st_t", NC_FLOAT, 3, &dims[0], &wmo1tid));
      NC(nc_def_var(ncid, "wmo_1st_z", NC_FLOAT, 3, &dims[0], &wmo1zid));
      NC(nc_def_var(ncid, "wmo_2nd_p", NC_FLOAT, 3, &dims[0], &wmo2pid));
      NC(nc_def_var(ncid, "wmo_2nd_t", NC_FLOAT, 3, &dims[0], &wmo2tid));
      NC(nc_def_var(ncid, "wmo_2nd_z", NC_FLOAT, 3, &dims[0], &wmo2zid));

      /* Set attributes... */
      add_text_attribute(ncid, "time", "units", "s");
      add_text_attribute(ncid, "time", "long_name",
			 "seconds sind 2000-01-01, 00:00 UTC");
      add_text_attribute(ncid, "lon", "units", "degrees_east");
      add_text_attribute(ncid, "lon", "long_name", "longitude");
      add_text_attribute(ncid, "lat", "units", "degrees_north");
      add_text_attribute(ncid, "lat", "long_name", "latitude");

      add_text_attribute(ncid, "clp_p", "units", "hPa");
      add_text_attribute(ncid, "clp_p", "long_name", "cold point pressure");
      add_text_attribute(ncid, "clp_t", "units", "K");
      add_text_attribute(ncid, "clp_t", "long_name",
			 "cold point temperature");
      add_text_attribute(ncid, "clp_z", "units", "km");
      add_text_attribute(ncid, "clp_z", "long_name", "cold point height");

      add_text_attribute(ncid, "dyn_p", "units", "hPa");
      add_text_attribute(ncid, "dyn_p", "long_name",
			 "dynamical tropopause pressure");
      add_text_attribute(ncid, "dyn_t", "units", "K");
      add_text_attribute(ncid, "dyn_t", "long_name",
			 "dynamical tropopause temperature");
      add_text_attribute(ncid, "dyn_z", "units", "km");
      add_text_attribute(ncid, "dyn_z", "long_name",
			 "dynamical tropopause height");

      add_text_attribute(ncid, "wmo_1st_p", "units", "hPa");
      add_text_attribute(ncid, "wmo_1st_p", "long_name",
			 "WMO 1st tropopause pressure");
      add_text_attribute(ncid, "wmo_1st_t", "units", "K");
      add_text_attribute(ncid, "wmo_1st_t", "long_name",
			 "WMO 1st tropopause temperature");
      add_text_attribute(ncid, "wmo_1st_z", "units", "km");
      add_text_attribute(ncid, "wmo_1st_z", "long_name",
			 "WMO 1st tropopause height");

      add_text_attribute(ncid, "wmo_2nd_p", "units", "hPa");
      add_text_attribute(ncid, "wmo_2nd_p", "long_name",
			 "WMO 2nd tropopause pressure");
      add_text_attribute(ncid, "wmo_2nd_t", "units", "K");
      add_text_attribute(ncid, "wmo_2nd_t", "long_name",
			 "WMO 2nd tropopause temperature");
      add_text_attribute(ncid, "wmo_2nd_z", "units", "km");
      add_text_attribute(ncid, "wmo_2nd_z", "long_name",
			 "WMO 2nd tropopause height");

      /* End definition... */
      NC(nc_enddef(ncid));

      /* Write longitude and latitude... */
      NC(nc_put_var_double(ncid, latid, lats));
      NC(nc_put_var_double(ncid, lonid, lons));
    }

    /* Write time... */
    start[0] = (size_t) nt;
    count[0] = 1;
    start[1] = 0;
    count[1] = (size_t) ny;
    start[2] = 0;
    count[2] = (size_t) nx;
    NC(nc_put_vara_double(ncid, timid, start, count, &met->time));

    /* Get cold point... */
    ctl.met_tropo = 2;
    read_met_tropo(&ctl, met);
#pragma omp parallel for default(shared) private(ix,iy)
    for (ix = 0; ix < nx; ix++)
      for (iy = 0; iy < ny; iy++) {
	intpol_met_space(met, 100, lons[ix], lats[iy], NULL,
			 &pt[iy * nx + ix], NULL, NULL, NULL,
			 NULL, NULL, NULL, NULL, NULL);
	intpol_met_space(met, pt[iy * nx + ix], lons[ix], lats[iy],
			 NULL, NULL, &zt[iy * nx + ix], &tt[iy * nx + ix],
			 NULL, NULL, NULL, NULL, NULL, NULL);
      }

    /* Write data... */
    NC(nc_put_vara_double(ncid, clppid, start, count, pt));
    NC(nc_put_vara_double(ncid, clptid, start, count, tt));
    NC(nc_put_vara_double(ncid, clpzid, start, count, zt));

    /* Get dynamical tropopause... */
    ctl.met_tropo = 5;
    read_met_tropo(&ctl, met);
#pragma omp parallel for default(shared) private(ix,iy)
    for (ix = 0; ix < nx; ix++)
      for (iy = 0; iy < ny; iy++) {
	intpol_met_space(met, 100, lons[ix], lats[iy], NULL,
			 &pt[iy * nx + ix], NULL, NULL, NULL,
			 NULL, NULL, NULL, NULL, NULL);
	intpol_met_space(met, pt[iy * nx + ix], lons[ix], lats[iy],
			 NULL, NULL, &zt[iy * nx + ix], &tt[iy * nx + ix],
			 NULL, NULL, NULL, NULL, NULL, NULL);
      }

    /* Write data... */
    NC(nc_put_vara_double(ncid, dynpid, start, count, pt));
    NC(nc_put_vara_double(ncid, dyntid, start, count, tt));
    NC(nc_put_vara_double(ncid, dynzid, start, count, zt));

    /* Get WMO 1st tropopause... */
    ctl.met_tropo = 3;
    read_met_tropo(&ctl, met);
#pragma omp parallel for default(shared) private(ix,iy)
    for (ix = 0; ix < nx; ix++)
      for (iy = 0; iy < ny; iy++) {
	intpol_met_space(met, 100, lons[ix], lats[iy], NULL,
			 &pt[iy * nx + ix], NULL, NULL, NULL,
			 NULL, NULL, NULL, NULL, NULL);
	intpol_met_space(met, pt[iy * nx + ix], lons[ix], lats[iy],
			 NULL, NULL, &zt[iy * nx + ix], &tt[iy * nx + ix],
			 NULL, NULL, NULL, NULL, NULL, NULL);
      }

    /* Write data... */
    NC(nc_put_vara_double(ncid, wmo1pid, start, count, pt));
    NC(nc_put_vara_double(ncid, wmo1tid, start, count, tt));
    NC(nc_put_vara_double(ncid, wmo1zid, start, count, zt));

    /* Get WMO 2nd tropopause... */
    ctl.met_tropo = 4;
    read_met_tropo(&ctl, met);
#pragma omp parallel for default(shared) private(ix,iy)
    for (ix = 0; ix < nx; ix++)
      for (iy = 0; iy < ny; iy++) {
	intpol_met_space(met, 100, lons[ix], lats[iy], NULL,
			 &pt[iy * nx + ix], NULL, NULL, NULL,
			 NULL, NULL, NULL, NULL, NULL);
	intpol_met_space(met, pt[iy * nx + ix], lons[ix], lats[iy],
			 NULL, NULL, &zt[iy * nx + ix], &tt[iy * nx + ix],
			 NULL, NULL, NULL, NULL, NULL, NULL);
      }

    /* Write data... */
    NC(nc_put_vara_double(ncid, wmo2pid, start, count, pt));
    NC(nc_put_vara_double(ncid, wmo2tid, start, count, tt));
    NC(nc_put_vara_double(ncid, wmo2zid, start, count, zt));

    /* Increment time step counter... */
    nt++;
  }

  /* Close file... */
  NC(nc_close(ncid));

  /* Free... */
  free(met);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

void add_text_attribute(
  int ncid,
  char *varname,
  char *attrname,
  char *text) {

  int varid;

  NC(nc_inq_varid(ncid, varname, &varid));
  NC(nc_put_att_text(ncid, varid, attrname, strlen(text), text));
}