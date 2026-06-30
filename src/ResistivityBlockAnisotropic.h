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
#ifndef DBLDEF_RESISTIVITY_BLOCK_ANISOTROPIC
#define DBLDEF_RESISTIVITY_BLOCK_ANISOTROPIC

#include "ResistivityBlock.h"

#include <iostream>

// Class of anisotropic resistivity blocks
class ResistivityBlockAnisotropic : public ResistivityBlock {

public:

	enum AnisotropyTypes{
		ISOTROPY = 0,
		TRANSVERSE_ISOTROPY,
		GENERAL_ANISOTROPY,
		END_OF_ANISOTROPY_TYPE// THIS MUST BE WRITTEN AT THE END
	};

	enum AnisotropyParameters {
		RHO_XX = 0,
		RHO_YY,
		RHO_ZZ,
		STRIKE,
		DIP,
		SLANT,
		END_OF_ANISOTROPY_PARAMETERS// THIS MUST BE WRITTEN AT THE END
	};

	struct ResistivityBlockParameters {
		int type;
		double rhoXX;
		double rhoYY;
		double rhoZZ;
		double strike;
		double dip;
		double slant;
		double resistivityValueMin;
		double resistivityValueMax;
		bool isFixedRhoXX;
		bool isFixedRhoYY;
		bool isFixedRhoZZ;
		bool isFixedStrike;
		bool isFixedDip;
		bool isFixedSlant;
	};

	struct CriteriaAndModifiedAnisotropicResistivity {
		double minRhoXXForSelecting;
		double maxRhoXXForSelecting;
		double modRhoXX;
		double minRhoYYForSelecting;
		double maxRhoYYForSelecting;
		double modRhoYY;
		double minRhoZZForSelecting;
		double maxRhoZZForSelecting;
		double modRhoZZ;
		double minStrikeForSelecting;
		double maxStrikeForSelecting;
		double modStrike;
		double minDipForSelecting;
		double maxDipForSelecting;
		double modDip;
		double minSlantForSelecting;
		double maxSlantForSelecting;
		double modSlant;
	};

	// Constructer
	ResistivityBlockAnisotropic();

	// Destructer
	virtual ~ResistivityBlockAnisotropic();

	// Change resistivity of the selected elements
	void changeResistivityOfSelectedElements(const std::set<int>& elementsSelected, const CriteriaAndModifiedAnisotropicResistivity& criteriaAndModifiedResistivity);

	// Output resistivity values to binary file
	virtual void outputResistivityValuesToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Get total number of resistivity blocks
	virtual int getNumResistivityBlockTotal() const;

private:

	// Copy constructer
	ResistivityBlockAnisotropic(const ResistivityBlockAnisotropic& rhs){
		std::cerr << "Error : Copy constructer of the class ResistivityBlockAnisotropic is not implemented." << std::endl;
		exit(1);
	};

	// Assignment operator
	ResistivityBlockAnisotropic& operator=(const ResistivityBlockAnisotropic& rhs){
		std::cerr << "Error : Assignment operator of the class ResistivityBlockAnisotropic is not implemented." << std::endl;
		exit(1);
	};

	// Arrays of resistivity block information
	std::vector<ResistivityBlockParameters> m_resistivityBlockParams;

	// Read reslstivity values from input file
	virtual void inputResistivityValues(const int nElem, const int nBlk, std::ifstream& inFile);

	// Output resistivity values to resistivity_block_iter*.dat
	virtual void outputResistivityValues(const int iterNum, FILE* fp) const;

	// Output xx-component of the anisotropic resistivity tensor to binary file
	void outputRhoXXOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output yy-component of the anisotropic resistivity tensor to binary file
	void outputRhoYYOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output zz-component of the anisotropic resistivity tensor to binary file
	void outputRhoZZOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output strike of the anisotropic resistivity tensor to binary file
	void outputStrikeOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output dip of the anisotropic resistivity tensor to binary file
	void outputDipOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output slant of the anisotropic resistivity tensor to binary file
	void outputSlantOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output abs(log10(rho_xx)-log10(rho_yy)) to binary file
	void outputfAnisotropyIndicatorToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const;

	// Output anisotropy types to binary file
	void outputAnisotropyTypesToBinary(const bool isTetra, const MeshData* const MeshData) const;

	// Assign modified resistivity
	void assignModifiedResistivity(ResistivityBlockParameters& params, const CriteriaAndModifiedAnisotropicResistivity& criteriaAndModifiedResistivity) const;

};

#endif
