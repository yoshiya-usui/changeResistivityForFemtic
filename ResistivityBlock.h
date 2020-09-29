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
#ifndef DBLDEF_RESISTIVITY_BLOCK
#define DBLDEF_RESISTIVITY_BLOCK

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "MeshDataTetraElement.h"

// Class of resistivity blocks
class ResistivityBlock{

public:

	enum ResistivityBlockTypes{
		FREE_AND_CONSTRAINED = 0,
		FIXED_AND_ISOLATED,
		FIXED_AND_CONSTRAINED,
		FREE_AND_ISOLATED,
	};

	// Constructer
	ResistivityBlock();

	// Destructer
	~ResistivityBlock();

	// Read data of resisitivity block model from input file
	void inputResisitivityBlock(const int iterNum);

	// Get resisitivity block ID from element ID
	inline int getBlockIDFromElemID( const int ielem ) const{
		return m_elementID2blockID[ ielem ];
	};

	// Get resistivity values from resisitivity block ID
	double getResistivityValuesFromBlockID( const int iblk ) const;

	// Get conductivity values from resisitivity block ID
	double getConductivityValuesFromBlockID( const int iblk ) const;

	// Get resistivity values from element ID
	double getResistivityValuesFromElemID( const int ielem ) const;

	// Get conductivity values from element ID
	double getConductivityValuesFromElemID( const int ielem ) const;

	// Get model ID from block ID
	int getModelIDFromBlockID( const int iblk ) const;

	// Get block ID from model ID
	int getBlockIDFromModelID( const int imdl ) const;

	// Get total number of resistivity blocks
	int getNumResistivityBlockTotal() const;

	// Get number of resistivity blocks whose resistivity values are not fixed
	int getNumResistivityBlockNotFixed() const;

	// Get flag specifing whether resistivity value of each block is fixed or not
	bool isFixedResistivityValue( const int iblk ) const;

	// Calculate volume of the specified resistivity block
	double calcVolumeOfBlock( int iblk ) const;

	// Get arrays of elements belonging to each resistivity model
	const std::vector< std::pair<int,double> >&  getBlockID2Elements( const int iBlk ) const;
	
	// Set resistivity values from resisitivity block ID
	void setResistivityValuesFromBlockID( const int iblk, const double val );

	// Set minimum resistivity values from resisitivity block ID
	void setResistivityValuesMinFromBlockID( const int iblk, const double val );

	// Set maximum resistivity values from resisitivity block ID
	void setResistivityValuesMaxFromBlockID( const int iblk, const double val );

	// Set flag specifing whether resistivity value of each block is fixed or not
	void fixedResistivityValue( const int iblk );

	// Calculate gravity centers of resistivity blocks
	void calcGravityCenters( const MeshDataTetraElement& MeshData );

	// Get gravity center of resistivity blocks
	CommonParameters::locationXYZ getGravityCenter( int iblk )const;

	// Output data of resisitivity block model to file
	void outputResisitivityBlock( const MeshDataTetraElement& MeshData, const int iterNum ) const;

	// Output resistivity values to binary file
	void outputResistivityValuesToBinary( const MeshDataTetraElement& MeshData, const int iterNum ) const;

private:
	// Copy constructer
	ResistivityBlock(const ResistivityBlock& rhs){
		std::cerr << "Error : Copy constructer of the class ResistivityBlock is not implemented." << std::endl;
		exit(1);
	};

	// Assignment operator
	ResistivityBlock& operator=(const ResistivityBlock& rhs){
		std::cerr << "Error : Assignment operator of the class ResistivityBlock is not implemented." << std::endl;
		exit(1);
	};

	// Array of the resistivity block IDs of each element
	int* m_elementID2blockID;

	// Array convert IDs of resistivity block to model IDs 
	int* m_blockID2modelID;

	// Array convert model IDs to IDs of resistivity block  
	int* m_modelID2blockID;

	// Total number of resistivity blocks
	int m_numResistivityBlockTotal;

	// Number of resistivity blocks whose resistivity values are fixed
	int m_numResistivityBlockNotFixed;

	// Array of resistivity values of each block
	double* m_resistivityValues;

	// Array of minimum resistivity values of each block
	double* m_resistivityValuesMin;

	// Array of maximum resistivity values of each block
	double* m_resistivityValuesMax;

	// Positive constant parameter n
	double* m_weightingConstants;

	// Flag specifing whether resistivity block is excluded from roughing matrix
	bool* m_isolated;

	// Flag specifing whether resistivity value of each block is fixed or not
	bool* m_fixResistivityValues;

	// Arrays of elements belonging to each resistivity model
	std::vector< std::pair<int,double> >* m_blockID2Elements;

	// Gravity centers of resistivity blocks
	CommonParameters::locationXYZ* m_gravityCenters;

	// Get type of resistivity block
	int getTypeOfResistivityBlock( const bool fixed, const bool isolated ) const;

};

#endif
