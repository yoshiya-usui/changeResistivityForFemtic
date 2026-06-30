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
#include "ResistivityBlockIsotropic.h"
#include "ResistivityBlockAnisotropic.h"

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
Length m_length = { 0.0, 0.0, 0.0 };
ResistivityBlock* m_ptrResistivityBlock = NULL;
ResistivityBlockIsotropic::CriteriaAndModifiedIsotropicResistivity m_criteriaAndModifiedIsotropicResistivity = { 0.1 , 1.0e4 ,  -1.0 , 0.1 ,1.0e4 };
ResistivityBlockAnisotropic::CriteriaAndModifiedAnisotropicResistivity m_criteriaAndModifiedAnisotropicResistivity;

void run( const std::string& paramFile, const bool isAnisotropicInversionUsed);
void readParameterFile(const bool isAnisotropicInversionUsed, const std::string& paramFile );
void selectElements( const MeshData* const MeshData, std::set<int>& elementsSelected );
bool inRegion( const CommonParameters::locationXYZ& coord );

int main( int argc, char* argv[] ){
	if( argc < 2 ){
		std::cerr << "You must specify parameter file  !!" << std::endl;
		exit(1);
	}
	bool isAnisotropicInversionUsed(false);
	for (int i = 2; i < argc; ++i) {
		if (strcmp(argv[i], "-aniso") == 0) {
			isAnisotropicInversionUsed = true;
		}
	}
	run( argv[1], isAnisotropicInversionUsed );
	return 0;
}

void run( const std::string& paramFile, const bool isAnisotropicInversionUsed ){
	readParameterFile(isAnisotropicInversionUsed, paramFile);
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
	std::set<int> elementsSelected ;
	selectElements(m_ptrMeshData, elementsSelected);
	if (isAnisotropicInversionUsed) {
		m_ptrResistivityBlock = new ResistivityBlockAnisotropic;
		m_ptrResistivityBlock->inputResistivityBlock(m_numIteration);
		dynamic_cast<ResistivityBlockAnisotropic*>(m_ptrResistivityBlock)->changeResistivityOfSelectedElements(elementsSelected, m_criteriaAndModifiedAnisotropicResistivity);
	}
	else {
		m_ptrResistivityBlock = new ResistivityBlockIsotropic;
		m_ptrResistivityBlock->inputResistivityBlock(m_numIteration);
		dynamic_cast<ResistivityBlockIsotropic*>(m_ptrResistivityBlock)->changeResistivityOfSelectedElements(elementsSelected, m_criteriaAndModifiedIsotropicResistivity);
	}
	m_ptrResistivityBlock->outputResistivityBlock(m_ptrMeshData, m_numIteration);
	const bool isTetra = ( meshType.substr(0,5).compare("TETRA") == 0 ) ? true : false;
	m_ptrResistivityBlock->outputResistivityValuesToBinary(isTetra, m_ptrMeshData, m_numIteration);
}

void readParameterFile( const bool isAnisotropicInversionUsed, const std::string& paramFile ){

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

	if (isAnisotropicInversionUsed) {
		// Rho_XX
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minRhoXXForSelecting;
		std::cout << "Minimum rhoXX [Ohm-m] for selecting the parameter cells to be modified: " 
			<< m_criteriaAndModifiedAnisotropicResistivity.minRhoXXForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxRhoXXForSelecting;
		std::cout << "Maximum rhoXX [Ohm-m] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxRhoXXForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modRhoXX;
		std::cout << "Modified rhoXX [Ohm-m]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modRhoXX << std::endl;
		// Rho_YY
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minRhoYYForSelecting;
		std::cout << "Minimum rhoYY [Ohm-m] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.minRhoYYForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxRhoYYForSelecting;
		std::cout << "Maximum rhoYY [Ohm-m] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxRhoYYForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modRhoYY;
		std::cout << "Modified rhoYY [Ohm-m]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modRhoYY << std::endl;
		// Rho_ZZ
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minRhoZZForSelecting;
		std::cout << "Minimum rhoZZ [Ohm-m] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.minRhoZZForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxRhoZZForSelecting;
		std::cout << "Maximum rhoYY [Ohm-m] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxRhoZZForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modRhoZZ;
		std::cout << "Modified rhoZZ [Ohm-m]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modRhoZZ << std::endl;
		// Strike
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minStrikeForSelecting;
		std::cout << "Minimum strike [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.minStrikeForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxStrikeForSelecting;
		std::cout << "Maximum strike [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxStrikeForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modStrike;
		std::cout << "Modified strike [deg.]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modStrike << std::endl;
		// Dip
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minDipForSelecting;
		std::cout << "Minimum dip [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.minDipForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxDipForSelecting;
		std::cout << "Maximum dip [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxDipForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modDip;
		std::cout << "Modified dip [deg.]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modDip << std::endl;
		// Slant
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.minSlantForSelecting;
		std::cout << "Minimum slant [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.minSlantForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.maxSlantForSelecting;
		std::cout << "Maximum slant [deg.] for selecting the parameter cells to be modified: "
			<< m_criteriaAndModifiedAnisotropicResistivity.maxSlantForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedAnisotropicResistivity.modSlant;
		std::cout << "Modified slant [deg.]: "
			<< m_criteriaAndModifiedAnisotropicResistivity.modSlant << std::endl;
	}
	else { 
		ifs >> m_criteriaAndModifiedIsotropicResistivity.minResistivityForSelecting;
		std::cout << "Minimum resistivity for selecting parameter cells [Ohm-m] :  " << m_criteriaAndModifiedIsotropicResistivity.minResistivityForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedIsotropicResistivity.maxResistivityForSelecting;
		std::cout << "Maximum resistivity for selecting parameter cells [Ohm-m] :  " << m_criteriaAndModifiedIsotropicResistivity.maxResistivityForSelecting << std::endl;
		ifs >> m_criteriaAndModifiedIsotropicResistivity.modResistivity;
		std::cout << "Modified resistivity [Ohm-m] :  " << m_criteriaAndModifiedIsotropicResistivity.modResistivity << std::endl;
		ifs >> m_criteriaAndModifiedIsotropicResistivity.modMinResistivity;
		std::cout << "Modified minimum resistivity [Ohm-m] :  " << m_criteriaAndModifiedIsotropicResistivity.modMinResistivity << std::endl;
		ifs >> m_criteriaAndModifiedIsotropicResistivity.modMaxResistivity;
		std::cout << "Modified maximum resistivity [Ohm-m] :  " << m_criteriaAndModifiedIsotropicResistivity.modMaxResistivity << std::endl;
	}

	ifs.close();

}

void selectElements( const MeshData* const MeshData, std::set<int>& elementsSelected ){

	const int numElemTotal = MeshData->getNumElemTotal();
	for( int iElem = 0; iElem < numElemTotal; ++iElem ){
		const CommonParameters::locationXYZ coord = MeshData->getElementCenter(iElem);
		if( inRegion(coord) ){
			elementsSelected.insert(iElem);
		}
	}
	std::cout << "Number of the elements in the target area: " << elementsSelected.size() << std::endl;

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
