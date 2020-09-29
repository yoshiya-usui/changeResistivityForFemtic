//--------------------------------------------------------------------------
// This file is part of changeResistivity.
//
// changeResistivity is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// changeResistivity is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with changeResistivity. If not, see <http://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------
#ifndef DBLDEF_COMMON_PARAMETERS
#define DBLDEF_COMMON_PARAMETERS

#include <stdio.h>
#include <math.h>

namespace CommonParameters{

static const int EX_POLARIZATION = 0;
static const int EY_POLARIZATION = 1;

static const int TM_MODE = 0;
static const int TE_MODE = 1;

//// Flag specifing the way of numbering
//enum numbering{
//	XYZ=0,
//	YZX=1,
//	ZXY=2,
//};
//
struct locationXY{
	double X;
	double Y;
};

struct locationYZ{
	double Y;
	double Z;
};

struct locationZX{
	double Z;
	double X;
};

struct locationXYZ{
	double X;
	double Y;
	double Z;
};

struct DoubleComplexValues{
	double realPart;
	double imagPart;
};

struct InitComplexValues{
	int realPart;
	int imagPart;
};

struct DoubleMatrix2x2{
	double comp11;
	double comp12;
	double comp21;
	double comp22;
};

struct Vector3D{
	double X;
	double Y;
	double Z;
};

struct AreaCoords{
	double coord0;
	double coord1;
	double coord2;
};

struct VolumeCoords{
	double coord0;
	double coord1;
	double coord2;
	double coord3;
};

struct CoordPair{
	double first;
	double second;
};

// Circular constant
static const double PI = 3.14159265358979;

// Factor converting values from radians to degrees
static const double rad2deg = 180.0 / 3.14159265358979;

// Factor converting values from degrees to radians
static const double deg2rad = 3.14159265358979 / 180.0;

// Magnetic permeability
static const double mu = 12.566370614e-07;

// Value of source electric field
static const double sourceValueElectric = 1000.0;

// Electric permittivity
//static const double epsilon = 8.854187817e-12;
static const double epsilon = 0.0;

// Very small value
static const double EPS = 1e-12;

// Abscissas of one point Gauss quadrature
static const double abscissas1Point[1] = { 0.0 };

// Abscissas of two point Gauss quadrature
static const double abscissas2Point[2] = { -1.0/sqrt(3.0), 1.0/sqrt(3.0) };

// Abscissas of three point Gauss quadrature
static const double abscissas3Point[3] = { -0.774596669241483, 0.0, 0.774596669241483 };

// Weights of one point Gauss quadrature
static const double weights1Point[1] = { 2.0 };

// Weights of two point Gauss quadrature
static const double weights2Point[2] = { 1.0, 1.0 };

// Weights of three point Gauss quadrature
static const double weights3Point[3] = { 0.555555555555556, 0.888888888888889, 0.555555555555556 };

// Factor converting value from kilo-meter to meter
static const double convKilometerToMeter = 1000.0;

static char programName[]="femtic";

static char versionID[]="1.1 Beta";

}

#endif
