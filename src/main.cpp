//--------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2021 Yoshiya Usui
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//--------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <complex>
#include <iomanip>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "MeshData.h"
#include "MeshDataTetraElement.h"
#include "MeshDataNonConformingHexaElement.h"
#include "ResistivityBlock.h"

enum RegionType{
	ELLIPSOID = 0,
	CUBOID,
	CYLINDROID,
};

struct Length{
	double xLength;
	double yLength;
	double zLength;
};

CommonParameters::locationXYZ m_center = { 0.0, 0.0, 0.0 };
double m_angle;
int m_numIteration = 0;
int m_regionType = ELLIPSOID;
double m_minResistivityForSelecting = 0.1;
double m_maxResistivityForSelecting = 1.0e4;
double m_modifiedResistivity = -1.0;
double m_modifiedMinResistivity = 0.1;
double m_modifiedMaxResistivity = 1.0e4;
Length m_length = { 0.0, 0.0, 0.0 };
ResistivityBlock m_resistivityBlock;

void run( const std::string& paramFile );
void readParameterFile( const std::string& paramFile );
void selectElements( const MeshData* const MeshData, std::set<int>& elementsSelected );
void selectResistivityBlocks();
bool inRegion( const CommonParameters::locationXYZ& coord );

int main( int argc, char* argv[] ){
	if( argc < 2 ){
		std::cerr << "You must specify parameter file  !!" << std::endl;
		exit(1);
	}
	run( argv[1] );
	return 0;
}

void run( const std::string& paramFile ){
	readParameterFile(paramFile);
	std::ifstream inFile( "mesh.dat", std::ios::in );
	if( inFile.fail() )
	{
		std::cerr << "File open error : mesh.dat !!" << std::endl;
		exit(1);
	}
	std::string meshType;
	inFile >> meshType;
	std::cout << "Mesh type: " << meshType << std::endl;
	MeshData* m_ptrMeshData = NULL; 
	if( meshType.substr(0,5).compare("TETRA") == 0 ){
		m_ptrMeshData = new MeshDataTetraElement;
	}else if( meshType.substr(0,5).compare("DHEXA") == 0 ){
		m_ptrMeshData = new MeshDataNonConformingHexaElement;
	}else{
		std::cerr << "Unsupported mesh type: " << meshType << std::endl;
	}
	m_ptrMeshData->inputMeshData();
	m_resistivityBlock.inputResisitivityBlock(m_numIteration);
	std::set<int> elementsSelected ;
	selectElements(m_ptrMeshData, elementsSelected);
	m_resistivityBlock.changeResistivityOfSelectedElements(elementsSelected, m_modifiedResistivity, m_modifiedMinResistivity, m_modifiedMaxResistivity );
	m_resistivityBlock.outputResisitivityBlock(m_ptrMeshData, m_numIteration);
	const bool isTetra = ( meshType.substr(0,5).compare("TETRA") == 0 ) ? true : false;
	m_resistivityBlock.outputResistivityValuesToBinary(isTetra, m_ptrMeshData, m_numIteration);
}

void readParameterFile( const std::string& paramFile ){

	std::ifstream ifs( paramFile.c_str(), std::ios::in );
	if( ifs.fail() ){
		std::cerr << "File open error : " << paramFile.c_str() << " !!" << std::endl;
		exit(1);
	}

	ifs >> m_numIteration;
	std::cout << "Iteration number : " << m_numIteration<< std::endl;

	ifs >> m_regionType;
	switch (m_regionType){
		case ELLIPSOID:
			std::cout << "Region type : Ellipsoid" << std::endl;
			break;
		case CUBOID:
			std::cout << "Region type : Cuboid" << std::endl;
			break;
		case CYLINDROID:
			std::cout << "Region type : Cylindroid" << std::endl;
			break;
		default:
			std::cout << "Region type is wrong : " << m_regionType << std::endl;
			exit(1);
	}

	switch (m_regionType){
		case ELLIPSOID:
			// Go through
		case CUBOID:
			// Go through
		case CYLINDROID:
			ifs >> m_length.xLength;
			std::cout << "Length of x axis [km] : " << m_length.xLength << std::endl;
			ifs >> m_length.yLength;
			std::cout << "Length of y axis [km] : " << m_length.yLength << std::endl;
			ifs >> m_length.zLength;
			std::cout << "Length of z axis [km] : " << m_length.zLength << std::endl;
			m_length.xLength *= 1000.0 * 0.5; 
			m_length.yLength *= 1000.0 * 0.5; 
			m_length.zLength *= 1000.0 * 0.5; 
			break;
		default:
			std::cout << "Region type is wrong : " << m_regionType << std::endl;
			exit(1);
	}

	ifs >> m_center.X;
	std::cout << "X coordinate of the center [km] : " << m_center.X << std::endl;
	ifs >> m_center.Y;
	std::cout << "Y coordinate of the center [km] : " << m_center.Y << std::endl;
	ifs >> m_center.Z;
	std::cout << "Z coordinate of the center [km] : " << m_center.Z << std::endl;
	ifs >> m_angle;
	std::cout << "Rotation angle [deg.] : " << m_angle << std::endl;
	m_center.X *= 1000.0;
	m_center.Y *= 1000.0;
	m_center.Z *= 1000.0;
	m_angle *= CommonParameters::deg2rad;

	ifs >> m_minResistivityForSelecting;
	std::cout << "Minimum resistivity for selecting parameter cells [Ohm-m] :  " << m_minResistivityForSelecting << std::endl;
	ifs >> m_maxResistivityForSelecting;
	std::cout << "Maximum resistivity for selecting parameter cells [Ohm-m] :  " << m_maxResistivityForSelecting << std::endl;

	ifs >> m_modifiedResistivity;
	std::cout << "Modified resistivity [Ohm-m] :  " << m_modifiedResistivity << std::endl;
	ifs >> m_modifiedMinResistivity;
	std::cout << "Modified minimum resistivity [Ohm-m] :  " << m_modifiedMinResistivity << std::endl;
	ifs >> m_modifiedMaxResistivity;
	std::cout << "Modified maximum resistivity [Ohm-m] :  " << m_modifiedMaxResistivity << std::endl;

	ifs.close();

}

void selectElements( const MeshData* const MeshData, std::set<int>& elementsSelected ){

	const int numElemTotal = MeshData->getNumElemTotal();
	for( int iElem = 0; iElem < numElemTotal; ++iElem ){
		const int iBlk = m_resistivityBlock.getBlockFromElement(iElem);
		if( !m_resistivityBlock.isFixedResistivityValue(iBlk) ){
			const CommonParameters::locationXYZ coord = MeshData->getElementCenter(iElem);
			const double resistivity = m_resistivityBlock.getResistivityValueFromBlockIndex(iBlk);
			if( inRegion(coord) && resistivity >= m_minResistivityForSelecting && resistivity <= m_maxResistivityForSelecting ){
				elementsSelected.insert(iElem);
			}
		}
	}

	std::cout << "Number of the selected elements : " << elementsSelected.size() << std::endl;

}

bool inRegion( const CommonParameters::locationXYZ& coord ){

	const CommonParameters::locationXYZ coordFromCenter = { coord.X - m_center.X, coord.Y - m_center.Y, coord.Z - m_center.Z }; 
	CommonParameters::locationXYZ coordRotated = { 0.0, 0.0, 0.0};
	coordRotated.X = coordFromCenter.X * cos( - m_angle ) - coordFromCenter.Y * sin( - m_angle );
	coordRotated.Y = coordFromCenter.X * sin( - m_angle ) + coordFromCenter.Y * cos( - m_angle );
	coordRotated.Z = coordFromCenter.Z;
	
	if( m_regionType == ELLIPSOID ){
		const double val = pow( coordRotated.X / m_length.xLength, 2 ) + pow( coordRotated.Y / m_length.yLength, 2 ) + pow( coordRotated.Z / m_length.zLength, 2 );
		if( val <= 1.0 ){
			return true;
		}
	}
	else if( m_regionType == CUBOID ){
		if( fabs(coordRotated.X) <= m_length.xLength && fabs(coordRotated.Y) <= m_length.yLength && fabs(coordRotated.Z) <= m_length.zLength ){
			return true;
		}
	}
	else if( m_regionType == CYLINDROID ){
		const double val = pow( coordRotated.X / m_length.xLength, 2 ) + pow( coordRotated.Y / m_length.yLength, 2 );
		if( val <= 1.0 && fabs(coordRotated.Z) <= m_length.zLength ){
			return true;
		}
	}
	else{
		std::cout << "Region type is wrong : " << m_regionType << std::endl;
		exit(1);
	}

	return false;

}
