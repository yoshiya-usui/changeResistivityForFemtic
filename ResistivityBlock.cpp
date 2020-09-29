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
#include "ResistivityBlock.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Constructer
ResistivityBlock::ResistivityBlock():
	m_elementID2blockID(NULL),
	m_blockID2modelID(NULL),
	m_modelID2blockID(NULL),
	m_numResistivityBlockTotal(0),
	m_numResistivityBlockNotFixed(0),
	m_resistivityValues(NULL),
	m_resistivityValuesMin(NULL),
	m_resistivityValuesMax(NULL),
	m_weightingConstants(NULL),
	m_isolated(NULL),
	m_fixResistivityValues(NULL),
	m_blockID2Elements(NULL),
	m_gravityCenters(NULL)
{}

// Destructer
ResistivityBlock::~ResistivityBlock(){

	if( m_elementID2blockID != NULL){
		delete[] m_elementID2blockID;
		m_elementID2blockID = NULL;
	}

	if( m_blockID2modelID != NULL){
		delete[] m_blockID2modelID;
		m_blockID2modelID = NULL;
	}

	if( m_modelID2blockID != NULL){
		delete[] m_modelID2blockID;
		m_modelID2blockID = NULL;
	}

	if( m_resistivityValues != NULL){
		delete[] m_resistivityValues;
		m_resistivityValues = NULL;
	}

	if( m_resistivityValuesMin != NULL){
		delete[] m_resistivityValuesMin;
		m_resistivityValuesMin = NULL;
	}

	if( m_resistivityValuesMax != NULL){
		delete[] m_resistivityValuesMax;
		m_resistivityValuesMax = NULL;
	}

	if( m_weightingConstants != NULL){
		delete[] m_weightingConstants;
		m_weightingConstants = NULL;
	}

	if( m_isolated != NULL){
		delete[] m_isolated;
		m_isolated = NULL;
	}

	if( m_fixResistivityValues != NULL){
		delete[] m_fixResistivityValues;
		m_fixResistivityValues = NULL;
	}

	if( m_blockID2Elements != NULL){
		delete[] m_blockID2Elements;
		m_blockID2Elements = NULL;
	}

	if( m_gravityCenters != NULL){
		delete[] m_gravityCenters;
		m_gravityCenters = NULL;
	}

}

// Read data of resisitivity block model from input file
void ResistivityBlock::inputResisitivityBlock(const int iterNum){

	std::ostringstream inputFile;
	inputFile << "resistivity_block_iter" << iterNum << ".dat";
	std::ifstream inFile( inputFile.str().c_str(), std::ios::in );

	if( inFile.fail() )
	{
		std::cerr << "File open error : " << inputFile.str().c_str() << " !!" << std::endl;
		exit(1);
	}

	int nElem(0);
	inFile >> nElem;
	if( m_elementID2blockID != NULL ){
		delete[] m_elementID2blockID;
		m_elementID2blockID = NULL;
	}
	m_elementID2blockID = new int[ nElem ];

	int nBlk(0);
	inFile >> nBlk;	
	m_numResistivityBlockTotal = nBlk;

	if( m_blockID2modelID != NULL){
		delete[] m_blockID2modelID;
		m_blockID2modelID = NULL;
	}
	m_blockID2modelID = new int[ m_numResistivityBlockTotal ];

	if( m_resistivityValues != NULL ){
		delete[] m_resistivityValues;
		m_resistivityValues = NULL;
	}
	m_resistivityValues = new double[ m_numResistivityBlockTotal ];

	if( m_resistivityValuesMin != NULL){
		delete[] m_resistivityValuesMin;
		m_resistivityValuesMin = NULL;
	}
	m_resistivityValuesMin = new double[ m_numResistivityBlockTotal ];

	if( m_resistivityValuesMax != NULL){
		delete[] m_resistivityValuesMax;
		m_resistivityValuesMax = NULL;
	}
	m_resistivityValuesMax = new double[ m_numResistivityBlockTotal ];

	if( m_weightingConstants != NULL){
		delete[] m_weightingConstants;
		m_weightingConstants = NULL;
	}
	m_weightingConstants = new double[ m_numResistivityBlockTotal ];

	if( m_fixResistivityValues != NULL ){
		delete[] m_fixResistivityValues;
		m_fixResistivityValues = NULL;
	}

	if( m_isolated != NULL){
		delete[] m_isolated;
		m_isolated = NULL;
	}
	m_isolated = new bool[ m_numResistivityBlockTotal ];

	if( m_fixResistivityValues != NULL ){
		delete[] m_fixResistivityValues;
		m_fixResistivityValues = NULL;
	}
	m_fixResistivityValues = new bool[ m_numResistivityBlockTotal ];

	for( int i = 0; i < m_numResistivityBlockTotal; ++i ){
		m_resistivityValues[i] = 0.0;
		m_resistivityValuesMin[i] = 0.0;
		m_resistivityValuesMax[i] = 0.0;
		m_weightingConstants[i] = 1.0;
		m_fixResistivityValues[i] = false;
		m_isolated[i] = false;
	}

#ifdef _DEBUG_WRITE
	std::cout << nElem << " " << m_numResistivityBlockTotal << std::endl; // For debug
#endif
	
	for( int iElem = 0; iElem < nElem; ++iElem ){
		int idum(0);
		int iblk(0);// Resistivity block ID
		inFile >> idum >> iblk;
		m_elementID2blockID[ iElem ] = iblk;

		if( iblk >= m_numResistivityBlockTotal || iblk < 0 )	{
			std::cerr << "Error : Resistivity block ID " << iblk << " of element " << iElem << " is improper !!" << std::endl;
			exit(1);
		}		

#ifdef _DEBUG_WRITE
		std::cout << iElem << " " << iblk << std::endl; // For debug
#endif
	}
	
	m_numResistivityBlockNotFixed = 0;
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
		int idum(0);
#ifdef _OLD
		int ifix(0);
		inFile >> idum >> m_resistivityValues[iBlk] >> ifix;
#else
		int itype(0);
		inFile >> idum >> m_resistivityValues[iBlk] >> m_resistivityValuesMin[iBlk] >> m_resistivityValuesMax[iBlk] >> m_weightingConstants[iBlk] >> itype;
#endif

		if( idum != iBlk ){
			std::cerr << "Error : Block ID is wrong !!" << std::endl;
			exit(1);
		}

#ifdef _OLD
		if( ifix == 1 ){
			m_fixResistivityValues[iBlk] = true;
			m_blockID2modelID[iBlk] = -1;
		}else{
			m_blockID2modelID[iBlk] = m_numResistivityBlockNotFixed++;
		}
#else
		switch(itype){
			case ResistivityBlock::FREE_AND_CONSTRAINED:// Go through
			case ResistivityBlock::FREE_AND_ISOLATED:
				m_blockID2modelID[iBlk] = m_numResistivityBlockNotFixed++;
				break;
			case ResistivityBlock::FIXED_AND_ISOLATED:// Go through
			case ResistivityBlock::FIXED_AND_CONSTRAINED:
				m_fixResistivityValues[iBlk] = true;
				m_blockID2modelID[iBlk] = -1;
				break;
			default:
				std::cerr << "Error : Type of resistivity block is unknown !! : " << itype << std::endl;
				exit(1);
				break;
		}

		switch(itype){
			case ResistivityBlock::FREE_AND_CONSTRAINED:// Go through
			case ResistivityBlock::FIXED_AND_CONSTRAINED:
				m_isolated[iBlk] = false;
				break;
			case ResistivityBlock::FIXED_AND_ISOLATED:// Go through
			case ResistivityBlock::FREE_AND_ISOLATED:
				m_isolated[iBlk] = true;
				break;
			default:
				std::cerr << "Error : Type of resistivity block is unknown !! : " << itype << std::endl;
				exit(1);
				break;
		}
#endif
	}

	if( !m_fixResistivityValues[0] ){
		std::cerr << "Error : Resistivity block 0 must be the air. And, its resistivity must be fixed." << std::endl;
		exit(1);
	}

	inFile.close();

	if( m_modelID2blockID != NULL){
		delete[] m_modelID2blockID;
		m_modelID2blockID = NULL;
	}
	m_modelID2blockID = new int[ m_numResistivityBlockNotFixed ];

	int icount(0);
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){

		if( !m_fixResistivityValues[iBlk] ){
			m_modelID2blockID[icount] = iBlk;
			++icount;
		}

	}

	if( icount != m_numResistivityBlockNotFixed ){
		std::cerr << "Error : icount is not equal to m_numResistivityBlockNotFixed. icount = " << icount << " m_numResistivityBlockNotFixed = " << m_numResistivityBlockNotFixed << std::endl;
		exit(1);
	}

#ifdef _DEBUG_WRITE
	for( int iMdl = 0; iMdl < m_numResistivityBlockNotFixed; ++iMdl ){
		std::cout << " iMdl m_modelID2blockID[iMdl] : " << iMdl << " " << m_modelID2blockID[iMdl] << std::endl;
	}
#endif

	if( m_blockID2Elements != NULL){
		delete[] m_blockID2Elements;
		m_blockID2Elements = NULL;
	}
	m_blockID2Elements= new std::vector< std::pair<int,double> >[m_numResistivityBlockTotal];

	for( int iElem = 0; iElem < nElem; ++iElem ){
		m_blockID2Elements[ m_elementID2blockID[ iElem ] ].push_back( std::make_pair(iElem,1.0) );
	}
#ifndef _LINUX
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
		m_blockID2Elements[ iBlk ].shrink_to_fit();
	}
#endif

	if( m_numResistivityBlockNotFixed <= 0 ){
		std::cerr << "Error : Total number of modifiable resisitivity value is zero or negative !! : " << m_numResistivityBlockNotFixed << std::endl;
		exit(1);
	}

#ifdef _DEBUG_WRITE
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
		const int num = static_cast<int>( m_blockID2Elements[ iBlk ].size() ); 
		for( int i = 0; i < num; ++i ){
			std::cout << " m_blockID2Elements[ " << iBlk << " ][ " << i << "] : " << m_blockID2Elements[iBlk][i].first << std::endl;
		}
	}
#endif

}

// Get resistivity values from resisitivity block ID
double ResistivityBlock::getResistivityValuesFromBlockID( const int iblk ) const{

	//if( iblk < 0 || iblk >= m_numResistivityBlockTotal ){
	//	OutputFiles::m_logFile << "Error : Specified block ID is out of range. iblk = " << iblk << std::endl;
	//	exit(1);
	//}
	assert( iblk >= 0 );
	assert( iblk < m_numResistivityBlockTotal );

	if( m_resistivityValues[ iblk ] < 0 ){
		return 1.0e+20;
	}

	return m_resistivityValues[ iblk ];
}

//// Get resisitivity block ID from element ID
//inline int ResistivityBlock::getBlockIDFromElemID( const int ielem ) const{
//	return m_elementID2blockID[ ielem ];
//}

// Get conductivity values from resisitivity block ID
double ResistivityBlock::getConductivityValuesFromBlockID( const int iblk ) const{

	//if( iblk < 0 || iblk >= m_numResistivityBlockTotal ){
	//	OutputFiles::m_logFile << "Error : Specified block ID is out of range. iblk = " << iblk << std::endl;
	//	exit(1);
	//}
	assert( iblk >= 0 );
	assert( iblk < m_numResistivityBlockTotal );

	if( m_resistivityValues[ iblk ] < 0 ){
		return 0.0;
	}

	return 1.0/m_resistivityValues[ iblk ];
}

// Get resistivity values from element ID
double ResistivityBlock::getResistivityValuesFromElemID( const int ielem ) const{

	//const int iblk = m_elementID2blockID[ ielem ];
	const int iblk = getBlockIDFromElemID(ielem);
	return getResistivityValuesFromBlockID( iblk );
}

// Get conductivity values from element ID
double ResistivityBlock::getConductivityValuesFromElemID( const int ielem ) const{

	//const int iblk = m_elementID2blockID[ ielem ];
	const int iblk = getBlockIDFromElemID(ielem);
	return getConductivityValuesFromBlockID( iblk );
}

// Get model ID from block ID
int ResistivityBlock::getModelIDFromBlockID( const int iblk ) const{

	//if( iblk < 0 || iblk >= m_numResistivityBlockTotal ){
	//	OutputFiles::m_logFile << "Error : Specified block ID is out of range. iblk = " << iblk << std::endl;
	//	exit(1);
	//}
	assert( iblk >= 0 );
	assert( iblk < m_numResistivityBlockTotal );

	return m_blockID2modelID[ iblk ];
}

// Get block ID from model ID
int ResistivityBlock::getBlockIDFromModelID( const int imdl ) const{

	//if( imdl < 0 || imdl >= m_numResistivityBlockNotFixed ){
	//	OutputFiles::m_logFile << "Error : Specified model ID is out of range. imdl = " << imdl << std::endl;
	//	exit(1);
	//}
	assert( imdl >= 0 );
	assert( imdl < m_numResistivityBlockNotFixed );

	return m_modelID2blockID[ imdl ];

}

// Get total number of resistivity blocks
int ResistivityBlock::getNumResistivityBlockTotal() const{
	return m_numResistivityBlockTotal;
}

// Get number of resistivity blocks whose resistivity values are fixed
int ResistivityBlock::getNumResistivityBlockNotFixed() const{
	return m_numResistivityBlockNotFixed;
}

// Get flag specifing whether resistivity value of each block is fixed or not
bool ResistivityBlock::isFixedResistivityValue( const int iblk ) const{

	//if( iblk < 0 || iblk >= m_numResistivityBlockTotal ){
	//	OutputFiles::m_logFile << "Error : Specified block ID is out of range. iblk = " << iblk << std::endl;
	//	exit(1);
	//}
	assert( iblk >= 0 );
	assert( iblk < m_numResistivityBlockTotal );

	return m_fixResistivityValues[iblk];
}

// Get arrays of elements belonging to each resistivity block
const std::vector< std::pair<int,double> >&  ResistivityBlock::getBlockID2Elements( const int iBlk ) const{

	return m_blockID2Elements[iBlk];

}

// Set resistivity values from resisitivity block ID
void ResistivityBlock::setResistivityValuesFromBlockID( const int iblk, const double val ){
	m_resistivityValues[iblk] = val;
}

// Set minimum resistivity values from resisitivity block ID
void ResistivityBlock::setResistivityValuesMinFromBlockID( const int iblk, const double val ){
	m_resistivityValuesMin[iblk] = val;
}

// Set maximum resistivity values from resisitivity block ID
void ResistivityBlock::setResistivityValuesMaxFromBlockID( const int iblk, const double val ){
	m_resistivityValuesMax[iblk] = val;
}

// Set flag specifing whether resistivity value of each block is fixed or not
void ResistivityBlock::fixedResistivityValue( const int iblk ){
	m_fixResistivityValues[iblk] = true;
}

// Calculate gravity centers of resistivity blocks
void ResistivityBlock::calcGravityCenters( const MeshDataTetraElement& MeshData ){

	const int numNeighborElement =MeshData.getNumNeighborElement();
	const int nElem = MeshData.getNumElemTotal();

	m_gravityCenters = new CommonParameters::locationXYZ[m_numResistivityBlockTotal];
	double* counter = new double[m_numResistivityBlockTotal];
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
		m_gravityCenters[iBlk].X = 0.0;
		m_gravityCenters[iBlk].Y = 0.0;
		m_gravityCenters[iBlk].Z = 0.0;
		counter[iBlk] = 0;
	}

	for( int iElem = 0; iElem < nElem; ++iElem ){
		const int iBlk = getBlockIDFromElemID( iElem );
		const CommonParameters::locationXYZ center = MeshData.getGravityCenter(iElem);
		const double volume = MeshData.calcVolume(iElem);
		m_gravityCenters[iBlk].X += center.X * volume;
		m_gravityCenters[iBlk].Y += center.Y * volume;
		m_gravityCenters[iBlk].Z += center.Z * volume;
		counter[iBlk] += volume;
	}

	const double EPS = 1.0e-20;
	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
		if( counter[iBlk] <= EPS ){
			std::cerr << "Number of the element belonging to block " << iBlk << " is less than " << EPS << " !!" << std::endl;
			exit(1);
		}
		const double div = 1.0 / counter[iBlk];
		m_gravityCenters[iBlk].X *= div;
		m_gravityCenters[iBlk].Y *= div;
		m_gravityCenters[iBlk].Z *= div;
	}

	delete [] counter;

}

// Get gravity center of resistivity blocks
CommonParameters::locationXYZ ResistivityBlock::getGravityCenter( int iBlk )const{

	return m_gravityCenters[iBlk];

}

// Output data of resisitivity block model to file
void ResistivityBlock::outputResisitivityBlock( const MeshDataTetraElement& MeshData, const int iterNum ) const{

	std::ostringstream fileName;
	fileName << "resistivity_block_iter" << iterNum << ".mod.dat";

	FILE *fp;
	if( (fp = fopen( fileName.str().c_str(), "w")) == NULL ) {
		std::cerr  << "File open error !! : " << fileName.str() << std::endl;
		exit(1);
	}

	const int numElemTotal = MeshData.getNumElemTotal();
	fprintf(fp, "%10d%10d\n",numElemTotal, m_numResistivityBlockTotal );

	for( int iElem = 0; iElem < numElemTotal; ++iElem ){
		fprintf(fp, "%10d%10d\n", iElem, m_elementID2blockID[iElem] );
	}

	for( int iBlk = 0; iBlk < m_numResistivityBlockTotal; ++iBlk ){
#ifdef _OLD
		fprintf(fp, "%10d%5s%15e%10d\n", iBlk, "     ",
			m_resistivityValues[iBlk], static_cast<int>( m_fixResistivityValues[iBlk] ) );
#else
		fprintf(fp, "%10d%5s%15e%15e%15e%15e%10d\n", iBlk, "     ",
			m_resistivityValues[iBlk], m_resistivityValuesMin[iBlk], m_resistivityValuesMax[iBlk], m_weightingConstants[iBlk],
			getTypeOfResistivityBlock(m_fixResistivityValues[iBlk], m_isolated[iBlk]) );
#endif
	}
	
	fclose(fp);

}

// Output resistivity values to binary file
void ResistivityBlock::outputResistivityValuesToBinary( const MeshDataTetraElement& MeshData, const int iterNum ) const{

	std::ostringstream oss;
	oss << "ResistivityMod.iter" << iterNum;
	std::ofstream fout;
	fout.open( oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc );

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Resistivity[Ohm-m]";
	strcpy( line, ossTitle.str().c_str() );
	fout.write( line, 80 );

	strcpy( line, "part" );
	fout.write( line, 80 );

	int ibuf(1);
	fout.write( (char*) &ibuf, sizeof( int ) );

	strcpy( line, "tetra4" );
	fout.write( line, 80 );

	const int nElem = MeshData.getNumElemTotal();

	for( int iElem = 0 ; iElem < nElem; ++iElem ){
		float dbuf = static_cast<float>( m_resistivityValues[ m_elementID2blockID[ iElem ] ] );
		fout.write( (char*) &dbuf, sizeof( float ) );
	}

	fout.close();

}

// Get type of resistivity block
int ResistivityBlock::getTypeOfResistivityBlock( const bool fixed, const bool isolated ) const{

	if( fixed ){
		if( isolated ){
			return ResistivityBlock::FIXED_AND_ISOLATED;
		}
		else{// Constrained
			return ResistivityBlock::FIXED_AND_CONSTRAINED;
		}
	}
	else{// Free
		if( isolated ){
			return ResistivityBlock::FREE_AND_ISOLATED;
		}
		else{// Constrained
			return ResistivityBlock::FREE_AND_CONSTRAINED;
		}
	}

}