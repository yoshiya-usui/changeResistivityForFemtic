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
#ifndef DBLDEF_RESISTIVITY_BLOCK_ISOTROPIC
#define DBLDEF_RESISTIVITY_BLOCK_ISOTROPIC

#include "ResistivityBlock.h"

#include <iostream>

// Class of isotropic resistivity blocks
class ResistivityBlockIsotropic : public ResistivityBlock {

public:

	enum ResistivityBlockTypes{
		FREE_AND_CONSTRAINED = 0,
		FIXED_AND_ISOLATED,
		FIXED_AND_CONSTRAINED,
		FREE_AND_ISOLATED,
	};

	enum BoundconstrainingTypes{
		SIMPLE_BOUND_CONSTRAINING = 0,
		TRANSFORMING_METHOD,
	};

	struct ResistivityBlockParameters {
		double resistivityValue;// Array of resistivity values of each block	
		double resistivityValueMin;// Array of minimum resistivity values of each block
		double resistivityValueMax;// Array of maximum resistivity values of each block
		double weightingConstant;// Positive constant parameter n
		int type;// Type of resistivity block
	};

	struct CriteriaAndModifiedIsotropicResistivity {
		double minResistivityForSelecting;
		double maxResistivityForSelecting;
		double modResistivity;
		double modMinResistivity;
		double modMaxResistivity;
	};

	// Constructer
	ResistivityBlockIsotropic();

	// Destructer
	virtual ~ResistivityBlockIsotropic();

	// Change resistivity of the selected elements
	void changeResistivityOfSelectedElements(const std::set<int>& elementsSelected, const CriteriaAndModifiedIsotropicResistivity& criteriaAndModifiedResistivity);

	// Output resistivity values to binary file
	virtual void outputResistivityValuesToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Get total number of resistivity blocks
	virtual int getNumResistivityBlockTotal() const;

private:

	// Copy constructer
	ResistivityBlockIsotropic(const ResistivityBlockIsotropic& rhs){
		std::cerr << "Error : Copy constructer of the class ResistivityBlockIsotropic is not implemented." << std::endl;
		exit(1);
	};

	// Assignment operator
	ResistivityBlockIsotropic& operator=(const ResistivityBlockIsotropic& rhs){
		std::cerr << "Error : Assignment operator of the class ResistivityBlockIsotropic is not implemented." << std::endl;
		exit(1);
	};

	// Arrays of resistivity block parameters
	std::vector<ResistivityBlockParameters> m_resistivityBlockParams;

	// Read reslstivity values from input file
	virtual void inputResistivityValues(const int nElem, const int nBlk, std::ifstream& inFile);

	// Output resistivity values to resistivity_block_iter*.dat
	virtual void outputResistivityValues(const int iterNum, FILE* fp) const;

	// Get resistivity value from resisitivity block index
	double getResistivityValueFromBlockIndex(const int iBlk) const;

	// Get flag specifing whether resistivity value of resistivity block is fixed or not
	bool isFixedResistivityValue(const int iBlk) const;

};

#endif
