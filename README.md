# GARP

Author: Michael James,

Organization: None

Last Updated: January 2013

## Overview

GARP (GEMPAK Analysis and Rendering Program) is an X-Windows/Motif
software application originally developed by the UCAR Cooperative
Program for Operational Meteorology, Education and Training (COMET),
for the display and analysis of meteorological data sets. Supported
data sets include model data, satellite imagery in NOAAPort GINI
format or MCIDAS area file format, NIDS and Nowrad radar data,
surface data and upper air data.

GARP was previously maintained by the Unidata Program Center at UCAR.

This software package is released under the MIT license contained
in the "LICENSE" file.

## Installation

GARP requires GEMPAK be installed and the appropriate Gemenviron or
Gemenviron.profile file be sourced.

The source tarball will unpack into the directory `garp`:

        cd garp
        make all
        make install

GARP executables will be installed in the GEMPAK binary executable
directory `$OS_BIN` and NTL tables in `$GEMTBL/config` will be updated
to include GARP as a menu selection in the program NTL.
