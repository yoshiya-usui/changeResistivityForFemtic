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
#include "ResistivityBlockIsotropic.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <iomanip>

// Constructer
ResistivityBlockIsotropic::ResistivityBlockIsotropic(){
}

// Destructer
ResistivityBlockIsotropic::~ResistivityBlockIsotropic(){
}

// Change resistivity of the selected elements
void ResistivityBlockIsotropic::changeResistivityOfSelectedElements(const std::set<int>& elementsSelected, 
	const CriteriaAndModifiedIsotropicResistivity& criteriaAndModifiedResistivity) {

	std::set<int> elementsSelectedMod = elementsSelected;
	// Resistivity criteria
	for (std::set<int>::const_iterator itr = elementsSelected.begin(); itr != elementsSelected.end(); ++itr) {
		const int iBlk = getBlockFromElement(*itr);
		if (isFixedResistivityValue(iBlk)) {
			elementsSelectedMod.erase(*itr);
			continue;
		}
		const double resistivity = getResistivityValueFromBlockIndex(iBlk);
		if (resistivity < criteriaAndModifiedResistivity.minResistivityForSelecting ||
			resistivity > criteriaAndModifiedResistivity.maxResistivityForSelecting) {
			elementsSelectedMod.erase(*itr);
			continue;
		}
	}
	std::cout << "Number of the selected elements : " << elementsSelectedMod.size() << std::endl;

	const int nBlkOrg = getNumResistivityBlockTotal();
	for (int iBlk = 0; iBlk < nBlkOrg; ++iBlk) {
		const std::set<int> elements = getElementsFromBlock(iBlk);
		bool allElementsSelected(true);
		for (std::set<int>::const_iterator itr = elements.begin(); itr != elements.end(); ++itr) {
			if (elementsSelectedMod.find(*itr) == elementsSelectedMod.end()) {
				allElementsSelected = false;
				break;
			}
		}
		if (allElementsSelected) {
			ResistivityBlockParameters& params = m_resistivityBlockParams[iBlk];
			params.resistivityValue = criteriaAndModifiedResistivity.modResistivity;
			params.resistivityValueMin = criteriaAndModifiedResistivity.modMinResistivity;
			params.resistivityValueMax = criteriaAndModifiedResistivity.modMaxResistivity;
			params.type = FIXED_AND_ISOLATED;
			for (std::set<int>::const_iterator itr = elements.begin(); itr != elements.end(); ++itr) {
				elementsSelectedMod.erase(*itr);
			}
		}
	}

	int iBlk(nBlkOrg);
	for (std::set<int>::const_iterator itr = elementsSelectedMod.begin(); itr != elementsSelectedMod.end(); ++itr, ++iBlk) {
		const int iElem = *itr;
		const int iBlkOrg = getBlockFromElement(iElem);
		const ResistivityBlockParameters& infoOrg = m_resistivityBlockParams[iBlkOrg];
		ResistivityBlockParameters params;
		params.resistivityValue = criteriaAndModifiedResistivity.modResistivity;
		params.resistivityValueMin = criteriaAndModifiedResistivity.modMinResistivity;
		params.resistivityValueMax = criteriaAndModifiedResistivity.modMaxResistivity;
		params.type = FIXED_AND_ISOLATED;
		params.weightingConstant = infoOrg.weightingConstant;
		m_resistivityBlockParams.push_back(params);
		m_elementToBlocks[iElem] = iBlk;
		m_blockToElements[iBlkOrg].erase(iElem);
		std::set<int> setBuf;
		setBuf.insert(iElem);
		m_blockToElements.push_back(setBuf);
	}

}

// Read isotropic reslstivity values from input file
void ResistivityBlockIsotropic::inputResistivityValues(const int nElem, const int nBlk, std::ifstream& inFile){

	for (int iBlk = 0; iBlk < nBlk; ++iBlk) {
		int idum(0);
		ResistivityBlockParameters params;
		inFile >> idum;
		if (idum != iBlk) {
			std::cerr << "Error : Block index is wrong !!" << std::endl;
			exit(1);
		}
		inFile >> params.resistivityValue;
		inFile >> params.resistivityValueMin;
		inFile >> params.resistivityValueMax;
		inFile >> params.weightingConstant;
		inFile >> params.type;
		m_resistivityBlockParams.push_back(params);
	}

}

// Get resistivity value from resisitivity block index
double ResistivityBlockIsotropic::getResistivityValueFromBlockIndex(const int iBlk) const {
	assert(iBlk >= 0);
	assert(iBlk < static_cast<int>(m_resistivityBlockParams.size()));
	const ResistivityBlockParameters& params = m_resistivityBlockParams[iBlk];
	return params.resistivityValue;
}

// Get total number of resistivity blocks
int ResistivityBlockIsotropic::getNumResistivityBlockTotal() const {
	return static_cast<int>(m_resistivityBlockParams.size());
}

// Get flag specifing whether resistivity value of resistivity block is fixed or not
bool ResistivityBlockIsotropic::isFixedResistivityValue(const int iBlk) const {
	assert(iBlk >= 0);
	assert(iBlk < static_cast<int>(m_resistivityBlockParams.size()));
	const ResistivityBlockParameters& params = m_resistivityBlockParams[iBlk];
	switch (params.type) {
	case ResistivityBlockIsotropic::FREE_AND_CONSTRAINED:// Go through
	case ResistivityBlockIsotropic::FREE_AND_ISOLATED:
		return false;
		break;
	case ResistivityBlockIsotropic::FIXED_AND_ISOLATED:// Go through
	case ResistivityBlockIsotropic::FIXED_AND_CONSTRAINED:
		return true;
		break;
	default:
		std::cerr << "Error : Type of resistivity block is unknown !! : " << params.type << std::endl;
		exit(1);
		break;
	}
	return true;
}

// Output data of resisitivity block model to file
void ResistivityBlockIsotropic::outputResistivityValues(const int iterNum, FILE* fp) const{

	int iBlk(0);
	for(std::vector<ResistivityBlockParameters>::const_iterator itr = m_resistivityBlockParams.begin(); itr != m_resistivityBlockParams.end(); ++itr, ++iBlk){
		fprintf(fp, "%10d%5s%15e%15e%15e%15e%10d\n", iBlk, "     ",
			itr->resistivityValue,
			itr->resistivityValueMin,
			itr->resistivityValueMax,
			itr->weightingConstant,
			itr->type);
	}

}

// Output resistivity values to binary file
void ResistivityBlockIsotropic::outputResistivityValuesToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const {

	std::ostringstream oss;
	oss << "ResistivityMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Resistivity[Ohm-m]";
	strcpy(line, ossTitle.str().c_str());
	fout.write(line, 80);

	strcpy(line, "part");
	fout.write(line, 80);

	int ibuf(1);
	fout.write((char*)&ibuf, sizeof(int));

	if (isTetra) {
		strcpy(line, "tetra4");
	}
	else {
		strcpy(line, "hexa8");
	}
	fout.write(line, 80);

	const int nElem = MeshData->getNumElemTotal();
	for (int iElem = 0; iElem < nElem; ++iElem) {
		const int iBlk = getBlockFromElement(iElem);
		const ResistivityBlockParameters& params = m_resistivityBlockParams[iBlk];
		float dbuf = static_cast<float>(params.resistivityValue);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}
