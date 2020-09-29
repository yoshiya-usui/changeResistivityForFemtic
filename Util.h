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
#ifndef DBLDEF_UTIL
#define DBLDEF_UTIL

#include <iostream>
#include <string>
#include <algorithm>

#include "CommonParameters.h"

// Flag specifing comparative operators
enum comparativeOperators{
	EQUAL=0,
	LARGE_LEFT_HAND_SIDE,
	SMALL_LEFT_HAND_SIDE,
};

// Sort elements by its key value with quick sort
void quickSort( const int numOfIDs, int* ids, const double* values );

// Sort elements by values of its three keys with quick sort
void quickSortThreeKeys( const int numOfIDs, int* ids,
	const double* firstKeyValues, const double* secondKeyValues, const double* thirdKeyValues );

// Compare values by its three keys
int compareValueByThreeKeys( const int lhsID, const int rhsID, const double* firstKeyValues, const double* secondKeyValues, const double* thirdKeyValues );

// Calculate matrix product for 2 x 2 double matrix
void calcProductFor2x2DoubleMatrix( const CommonParameters::DoubleMatrix2x2& matInA, const CommonParameters::DoubleMatrix2x2& matInB, CommonParameters::DoubleMatrix2x2& matOut );

// Calculate 3D vectors
CommonParameters::Vector3D calcVector3D( const CommonParameters::locationXYZ& startCoords, const CommonParameters::locationXYZ& endCoords );

// Calculate outer product of 3D vectors
CommonParameters::Vector3D calcOuterProduct( const CommonParameters::Vector3D& vec1, const CommonParameters::Vector3D& vec2 );

// Calculate inner product of 3D vectors
double calcInnerProduct( const CommonParameters::Vector3D& vec1, const CommonParameters::Vector3D& vec2 );

#endif
