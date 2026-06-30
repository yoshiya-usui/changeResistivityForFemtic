//-------------------------------------------------------------------------------------------------------
// The MIT License (MIT)
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
//-------------------------------------------------------------------------------------------------------
#ifndef DBLDEF_RESISTIVITY_BLOCK
#define DBLDEF_RESISTIVITY_BLOCK

#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>
#include "MeshData.h"

// Class of resistivity blocks
class ResistivityBlock{

public:

	// Constructer
	ResistivityBlock();

	// Destructer
	virtual ~ResistivityBlock();

	// Output resistivity values to binary file
	virtual void outputResistivityValuesToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const = 0;

	// Read data of resisitivity block model from input file
	void inputResistivityBlock(const int iterNum);

	// Get resisitivity block index from element index
	int getBlockFromElement(const int iElem) const;

	// Get element indexes from resistivity block index
	const std::set<int> getElementsFromBlock(const int iBlk) const;

	// Get total number of resistivity blocks
	virtual int getNumResistivityBlockTotal() const = 0;

	// Output data of resisitivity block model to file
	void outputResistivityBlock(const MeshData* const MeshData, const int iterNum) const;

protected:

	// Array mapping element indexess to resistivity block indexes
	std::map<int, int> m_elementToBlocks;

	// Array mapping resistivity block indexes to element indexes
	std::vector< std::set<int> > m_blockToElements;

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

	// Read reslstivity values from input file
	virtual void inputResistivityValues( const int nElem, const int nBlk, std::ifstream& inFile ) = 0;

	// Output resistivity values to resistivity_block_iter*.dat
	virtual void outputResistivityValues(const int iterNum, FILE* fp) const = 0;


};

#endif
