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
#ifndef DBLDEF_RESISTIVITY_BLOCK
#define DBLDEF_RESISTIVITY_BLOCK

#include <iostream>
#include <set>
#include <vector>
#include <map>
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

	struct ResistivityBlockInformation{
		// Array of resistivity values of each block
		double resistivityValue;
		// Array of minimum resistivity values of each block
		double resistivityValueMin;
		// Array of maximum resistivity values of each block
		double resistivityValueMax;
		// Positive constant parameter n
		double weightingConstant;
		// Type of resistivity block
		int type;
	};

	// Constructer
	ResistivityBlock();

	// Destructer
	~ResistivityBlock();

	// Change resistivity of the selected elements
	void changeResistivityOfSelectedElements( const std::set<int>& elementsSelected, const double resistivityMod ,
		const double resistivityModMin, const double resistivityMax );

	// Read data of resisitivity block model from input file
	void inputResisitivityBlock(const int iterNum);

	// Get resisitivity block index from element index
	int getBlockFromElement( const int iElem ) const;

	// Get resistivity value from resisitivity block index
	double getResistivityValueFromBlockIndex( const int iBlk ) const;

	// Get total number of resistivity blocks
	int getNumResistivityBlockTotal() const;

	// Get flag specifing whether resistivity value of resistivity block is fixed or not
	bool isFixedResistivityValue( const int iBlk ) const;

	// Get element indexes from resistivity block index
	const std::set<int> getElementsFromBlock( const int iBlk ) const;

	// Output data of resisitivity block model to file
	void outputResisitivityBlock( const MeshData* const MeshData, const int iterNum ) const;

	// Output resistivity values to binary file
	void outputResistivityValuesToBinary( const bool isTetra, const MeshData* const MeshData, const int iterNum ) const;

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

	// Array mapping element indexess to resistivity block indexes
	std::map<int, int> m_elementToBlocks;

	// Array mapping resistivity block indexes to element indexes
	std::vector< std::set<int> > m_blockToElements;

	// Arrays of resistivity block information
	std::vector<ResistivityBlockInformation> m_resistivityBlockInfo;

};

#endif
