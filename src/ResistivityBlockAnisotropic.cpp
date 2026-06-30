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
#include "ResistivityBlockAnisotropic.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <iomanip>


// Constructer
ResistivityBlockAnisotropic::ResistivityBlockAnisotropic(){
}

// Destructer
ResistivityBlockAnisotropic::~ResistivityBlockAnisotropic(){
}

// Change resistivity of the selected elements
void ResistivityBlockAnisotropic::changeResistivityOfSelectedElements(const std::set<int>& elementsSelected,
	const CriteriaAndModifiedAnisotropicResistivity& criteriaAndModifiedResistivity) {

	std::set<int> elementsSelectedMod = elementsSelected;
	// Resistivity criteria
	for (std::set<int>::const_iterator itr = elementsSelected.begin(); itr != elementsSelected.end(); ++itr) {
		const int iBlk = getBlockFromElement(*itr);
		const ResistivityBlockParameters& params = m_resistivityBlockParams[iBlk];
		bool deleteFromList(true);
		switch (params.type) {
		case ISOTROPY:
			// rho_XX
			if (!params.isFixedRhoXX) {
				if (params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
					params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting) {
					deleteFromList = false;
				}
			}
			break;
		case TRANSVERSE_ISOTROPY:
			// rho_XX
			if (!params.isFixedRhoXX) {
				if (params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
					params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting) {
					deleteFromList = false;
				}
			}
			// rho_YY
			if (!params.isFixedRhoYY) {
				if (params.rhoYY >= criteriaAndModifiedResistivity.minRhoYYForSelecting &&
					params.rhoYY <= criteriaAndModifiedResistivity.maxRhoYYForSelecting) {
					deleteFromList = false;
				}
			}
			// strike
			if (!params.isFixedStrike) {
				if (params.strike >= criteriaAndModifiedResistivity.minStrikeForSelecting &&
					params.strike <= criteriaAndModifiedResistivity.maxStrikeForSelecting) {
					deleteFromList = false;
				}
			}
			// dip
			if (!params.isFixedDip) {
				if (params.dip >= criteriaAndModifiedResistivity.minDipForSelecting &&
					params.dip <= criteriaAndModifiedResistivity.maxDipForSelecting) {
					deleteFromList = false;
				}
			}
			break;
		case GENERAL_ANISOTROPY:
			// rho_XX
			if (!params.isFixedRhoXX) {
				if (params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
					params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting) {
					deleteFromList = false;
				}
			}
			// rho_YY
			if (!params.isFixedRhoYY) {
				if (params.rhoYY >= criteriaAndModifiedResistivity.minRhoYYForSelecting &&
					params.rhoYY <= criteriaAndModifiedResistivity.maxRhoYYForSelecting) {
					deleteFromList = false;
				}
			}
			// rho_ZZ
			if (!params.isFixedRhoZZ) {
				if (params.rhoZZ >= criteriaAndModifiedResistivity.minRhoZZForSelecting &&
					params.rhoZZ <= criteriaAndModifiedResistivity.maxRhoZZForSelecting) {
					deleteFromList = false;
				}
			}
			// strike
			if (!params.isFixedStrike) {
				if (params.strike >= criteriaAndModifiedResistivity.minStrikeForSelecting &&
					params.strike <= criteriaAndModifiedResistivity.maxStrikeForSelecting) {
					deleteFromList = false;
				}
			}
			// dip
			if (!params.isFixedDip) {
				if (params.dip >= criteriaAndModifiedResistivity.minDipForSelecting &&
					params.dip <= criteriaAndModifiedResistivity.maxDipForSelecting) {
					deleteFromList = false;
				}
			}
			// slant
			if (!params.isFixedSlant) {
				if (params.slant >= criteriaAndModifiedResistivity.minSlantForSelecting &&
					params.slant <= criteriaAndModifiedResistivity.maxSlantForSelecting) {
					deleteFromList = false;
				}
			}
			break;
		default:
			std::cout << "Unsupported anisotropy type : " << params.type << std::endl;
			break;
		}
		if (deleteFromList) {
			elementsSelectedMod.erase(*itr);
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
			assignModifiedResistivity(params, criteriaAndModifiedResistivity);
			for (std::set<int>::const_iterator itr = elements.begin(); itr != elements.end(); ++itr) {
				elementsSelectedMod.erase(*itr);
			}
		}
	}

	int iBlk(nBlkOrg);
	for (std::set<int>::const_iterator itr = elementsSelectedMod.begin(); itr != elementsSelectedMod.end(); ++itr, ++iBlk) {
		const int iElem = *itr;
		const int iBlkOrg = getBlockFromElement(iElem);
		const ResistivityBlockParameters& paramsOrg = m_resistivityBlockParams[iBlkOrg];
		ResistivityBlockParameters params(paramsOrg);
		assignModifiedResistivity(params, criteriaAndModifiedResistivity);
		m_resistivityBlockParams.push_back(params);
		m_elementToBlocks[iElem] = iBlk;
		m_blockToElements[iBlkOrg].erase(iElem);
		std::set<int> setBuf;
		setBuf.insert(iElem);
		m_blockToElements.push_back(setBuf);
	}

}

// Read reslstivity values from input file
void ResistivityBlockAnisotropic::inputResistivityValues(const int nElem, const int nBlk, std::ifstream& inFile) {

	for (int iBlk = 0; iBlk < nBlk; ++iBlk) {
		int idum(0);
		ResistivityBlockParameters params;
		inFile >> idum;
		if (idum != iBlk) {
			std::cerr << "Error : Block ID is wrong !!" << std::endl;
			exit(1);
		}
		inFile >> idum;
		const int typeOfAnistropy = idum;
		params.type = typeOfAnistropy;
		int isFixedArray[6] = { -1, -1, -1, -1, -1, -1 };
		if (typeOfAnistropy == ResistivityBlockAnisotropic::ISOTROPY) {
			inFile >> params.rhoXX;
			if (params.rhoXX <= 0.0) {
				std::cerr << "Error : The resistivity of block " << iBlk << " is less than or equal to zero !! : " << params.rhoXX << std::endl;
				exit(1);
			}
			params.rhoYY = params.rhoXX;
			params.rhoZZ = params.rhoXX;
			inFile >> params.resistivityValueMin >> params.resistivityValueMax;
			if (params.resistivityValueMin <= 0.0) {
				std::cerr << "Error : Minimum resistivity value of block " << iBlk << " is less than or equal to zero !! : " << params.resistivityValueMin << std::endl;
				exit(1);
			}
			if (params.resistivityValueMax <= 0.0) {
				std::cerr << "Error : Maximum resistivity value of block " << iBlk << " is less than or equal to zero !! : " << params.resistivityValueMax << std::endl;
				exit(1);
			}
			if (params.resistivityValueMax < params.rhoXX) {
				std::cerr << "Error : Maximum resistivity value ( " << params.resistivityValueMax << " ) is less than initial resistivity ( " << params.rhoXX << " )." << std::endl;
				exit(1);
			}
			if (params.resistivityValueMin > params.rhoXX) {
				std::cerr << "Error : Minimum resistivity value ( " << params.resistivityValueMin << " ) is greater than initial resistivity ( " << params.rhoXX << " )." << std::endl;
				exit(1);
			}
			int isFixed(0);
			inFile >> isFixed;
			isFixedArray[RHO_XX] = isFixed;
			isFixedArray[RHO_YY] = 1;
			isFixedArray[RHO_ZZ] = 1;
			isFixedArray[STRIKE] = 1;
			isFixedArray[DIP] = 1;
			isFixedArray[SLANT] = 1;
		}
		else if (typeOfAnistropy == TRANSVERSE_ISOTROPY || typeOfAnistropy == GENERAL_ANISOTROPY) {
			// Read principle resistivity values
			inFile >> params.rhoXX >> params.rhoYY;
			if (params.rhoXX <= 0.0) {
				std::cerr << "Error : The xx-compoonent of the resistivity of block " << iBlk << " is less than or equal to zero !! : " << params.rhoXX << std::endl;
				exit(1);
			}
			if (params.rhoYY <= 0.0) {
				std::cerr << "Error : The yy-compoonent of the resistivity of block " << iBlk << " is less than or equal to zero !! : " << params.rhoYY << std::endl;
				exit(1);
			}
			if (typeOfAnistropy == TRANSVERSE_ISOTROPY) {
				params.rhoZZ = params.rhoXX;
			}
			else{
				inFile >> params.rhoZZ;
				if (params.rhoZZ <= 0.0) {
					std::cerr << "Error : The zz-compoonent of the resistivity of block " << iBlk << " is less than or equal to zero !! : " << params.rhoZZ << std::endl;
					exit(1);
				}
			}
			// Read orientation angles
			inFile >> params.strike >> params.dip;
			if (typeOfAnistropy == TRANSVERSE_ISOTROPY) {
				params.slant = 0.0;
			}
			else{
				inFile >> params.slant;
			}
			// Read lower and upper bounds of resistivity values
			inFile >> params.resistivityValueMin >> params.resistivityValueMax;
			if (params.resistivityValueMax < params.rhoXX) {
				std::cerr << "Error : Maximum resistivity value ( " << params.resistivityValueMax << " ) is less than initial resistivity of the xx-component ( " << params.rhoXX << " )." << std::endl;
				exit(1);
			}
			if (params.resistivityValueMin > params.rhoXX) {
				std::cerr << "Error : Minimum resistivity value ( " << params.resistivityValueMax << " ) is greater than initial resistivity of the xx-component ( " << params.rhoXX << " )." << std::endl;
				exit(1);
			}
			if (params.resistivityValueMax < params.rhoYY) {
				std::cerr << "Error : Maximum resistivity value ( " << params.resistivityValueMax << " ) is less than initial resistivity of the yy-component ( " << params.rhoYY << " )." << std::endl;
				exit(1);
			}
			if (params.resistivityValueMin > params.rhoYY) {
				std::cerr << "Error : Minimum resistivity value ( " << params.resistivityValueMax << " ) is greater than initial resistivity of the yy-component ( " << params.rhoYY << " )." << std::endl;
				exit(1);
			}
			if (typeOfAnistropy == GENERAL_ANISOTROPY) {
				if (params.resistivityValueMax < params.rhoZZ) {
					std::cerr << "Error : Maximum resistivity value ( " << params.resistivityValueMax << " ) is less than initial resistivity of the zz-component ( " << params.rhoZZ << " )." << std::endl;
					exit(1);
				}
				if (params.resistivityValueMin > params.rhoZZ) {
					std::cerr << "Error : Minimum resistivity value ( " << params.resistivityValueMax << " ) is greater than initial resistivity of the zz-component ( " << params.rhoZZ << " )." << std::endl;
					exit(1);
				}
			}
			// Read fixing flags
			if (typeOfAnistropy == TRANSVERSE_ISOTROPY) {
				inFile >> isFixedArray[RHO_XX] >> isFixedArray[RHO_YY];
				isFixedArray[RHO_ZZ] = 1;
				inFile >> isFixedArray[STRIKE] >> isFixedArray[DIP];
				isFixedArray[SLANT] = 1;
			} 
			else {
				inFile >> isFixedArray[RHO_XX] >> isFixedArray[RHO_YY] >> isFixedArray[RHO_ZZ];
				inFile >> isFixedArray[STRIKE] >> isFixedArray[DIP] >> isFixedArray[SLANT];
			}
		}
		else {
			std::cerr << "Error : Unsupported type of anisotropy (" << typeOfAnistropy << ")" << std::endl;
			exit(1);
		}
		params.isFixedRhoXX = isFixedArray[RHO_XX] == 1 ? true : false;
		params.isFixedRhoYY = isFixedArray[RHO_YY] == 1 ? true : false;
		params.isFixedRhoZZ = isFixedArray[RHO_ZZ] == 1 ? true : false;
		params.isFixedStrike = isFixedArray[STRIKE] == 1 ? true : false;
		params.isFixedDip = isFixedArray[DIP] == 1 ? true : false;
		params.isFixedSlant = isFixedArray[SLANT] == 1 ? true : false;
		m_resistivityBlockParams.push_back(params);
	}

}

// Get total number of resistivity blocks
int ResistivityBlockAnisotropic::getNumResistivityBlockTotal() const {
	return static_cast<int>(m_resistivityBlockParams.size());
}

// Output resistivity values to binary file
void ResistivityBlockAnisotropic::outputResistivityValuesToBinary(const bool isTetra, const MeshData* const MeshData, const int iterNum) const {

	outputAnisotropyTypesToBinary(isTetra, MeshData);
	outputRhoXXOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputRhoYYOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputRhoZZOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputStrikeOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputDipOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputSlantOfAnisotropicResistivityTensorToBinary(isTetra, MeshData, iterNum);
	outputfAnisotropyIndicatorToBinary(isTetra, MeshData, iterNum);

}

// Output anisotropy types to binary file
void ResistivityBlockAnisotropic::outputAnisotropyTypesToBinary(const bool isTetra, const MeshData* const MeshData) const {

	std::ofstream fout;
	fout.open("AnisotropyTypesMod", std::ios::out | std::ios::binary | std::ios::trunc);
	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Anisotropy types";
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
		float dbuf = static_cast<float>(params.type);
		fout.write((char*)&dbuf, sizeof(float));
	}
	fout.close();

}
// Output xx-component of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputRhoXXOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData, 
	const int iterNum) const{

	std::ostringstream oss;
	oss << "RhoXXMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "RhoXX[Ohm-m]";
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
		float dbuf = static_cast<float>(params.rhoXX);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output yy-component of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputRhoYYOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "RhoYYMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "RhoYY[Ohm-m]";
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
		float dbuf = static_cast<float>(params.rhoYY);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output zz-component of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputRhoZZOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "RhoZZMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "RhoZZ[Ohm-m]";
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
		float dbuf = static_cast<float>(params.rhoZZ);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output strike of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputStrikeOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "StrikeMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Strike[Deg.]";
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
		float dbuf = static_cast<float>(params.strike);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();
}

// Output dip of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputDipOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "DipMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Dip[Deg.]";
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
		float dbuf = static_cast<float>(params.dip);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output slant of the anisotropic resistivity tensor to binary file
void ResistivityBlockAnisotropic::outputSlantOfAnisotropicResistivityTensorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "SlantMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Slant[Deg.]";
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
		float dbuf = static_cast<float>(params.slant);
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output abs(log10(rho_xx)-log10(rho_yy)) to binary file
void ResistivityBlockAnisotropic::outputfAnisotropyIndicatorToBinary(const bool isTetra, const MeshData* const MeshData,
	const int iterNum) const {

	std::ostringstream oss;
	oss << "AnisotropyMod.iter" << iterNum;
	std::ofstream fout;
	fout.open(oss.str().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	char line[80];
	std::ostringstream ossTitle;
	ossTitle << "Anisotropy";
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
		const double log10RhoXX = log10(params.rhoXX);
		const double log10RhoYY = log10(params.rhoYY);
		const float dbuf = static_cast<float>(fabs(log10RhoXX - log10RhoYY));
		fout.write((char*)&dbuf, sizeof(float));
	}

	fout.close();

}

// Output data of resisitivity block model to file (anisotropic case)
void ResistivityBlockAnisotropic::outputResistivityValues(const int iterNum, FILE* fp) const {

	int iBlk(0);
	for (std::vector<ResistivityBlockParameters>::const_iterator itr = m_resistivityBlockParams.begin(); itr != m_resistivityBlockParams.end(); ++itr, ++iBlk) {
		const int typeOfAnisotropy = itr->type;
		switch (typeOfAnisotropy) {
		case ResistivityBlockAnisotropic::ISOTROPY:
			fprintf(fp, "%10d%10d%5s%15e%15e%15e%10d\n", iBlk, typeOfAnisotropy, "     ",
				itr->rhoXX,
				itr->resistivityValueMin, itr->resistivityValueMax,
				itr->isFixedRhoXX ? 1 : 0);
			break;
		case ResistivityBlockAnisotropic::TRANSVERSE_ISOTROPY:
			fprintf(fp, "%10d%10d%5s%15e%15e%15e%15e%15e%15e%10d%10d%10d%10d\n", iBlk, typeOfAnisotropy, "     ",
				itr->rhoXX,
				itr->rhoYY,
				itr->strike,
				itr->dip,
				itr->resistivityValueMin, itr->resistivityValueMax,
				itr->isFixedRhoXX ? 1 : 0,
				itr->isFixedRhoYY ? 1 : 0,
				itr->isFixedStrike ? 1 : 0,
				itr->isFixedDip ? 1 : 0);
			break;
		case ResistivityBlockAnisotropic::GENERAL_ANISOTROPY:
			fprintf(fp, "%10d%10d%5s%15e%15e%15e%15e%15e%15e%15e%15e%10d%10d%10d%10d%10d%10d\n", iBlk, typeOfAnisotropy, "     ",
				itr->rhoXX,
				itr->rhoYY,
				itr->rhoZZ,
				itr->strike,
				itr->dip,
				itr->slant,
				itr->resistivityValueMin, itr->resistivityValueMax,
				itr->isFixedRhoXX ? 1 : 0,
				itr->isFixedRhoYY ? 1 : 0,
				itr->isFixedRhoZZ ? 1 : 0,
				itr->isFixedStrike ? 1 : 0,
				itr->isFixedDip ? 1 : 0,
				itr->isFixedSlant ? 1 : 0);
			break;
		}
	}

}

// Assign modified resistivity
void ResistivityBlockAnisotropic::assignModifiedResistivity(ResistivityBlockParameters& params,
	const CriteriaAndModifiedAnisotropicResistivity& criteriaAndModifiedResistivity) const{

	switch (params.type) {
	case ISOTROPY:
		// rho_XX
		if (!params.isFixedRhoXX &&
			params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
			params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting ){
			params.rhoXX = criteriaAndModifiedResistivity.modRhoXX;
			params.isFixedRhoXX = true;
			params.rhoYY = params.rhoXX;
			params.isFixedRhoYY = true;
			params.rhoZZ = params.rhoXX;
			params.isFixedRhoZZ = true;
		}
		break;
	case TRANSVERSE_ISOTROPY:
		// rho_XX
		if (!params.isFixedRhoXX &&
			params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
			params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting) {
			params.rhoXX = criteriaAndModifiedResistivity.modRhoXX;
			params.isFixedRhoXX = true;
			params.rhoZZ = params.rhoXX;
			params.isFixedRhoZZ = true;
		}
		// rho_YY
		if (!params.isFixedRhoYY &&
			params.rhoYY >= criteriaAndModifiedResistivity.minRhoYYForSelecting &&
			params.rhoYY <= criteriaAndModifiedResistivity.maxRhoYYForSelecting) {
			params.rhoYY = criteriaAndModifiedResistivity.modRhoYY;
			params.isFixedRhoYY = true;
		}
		// strike
		if (!params.isFixedStrike &&
			params.strike >= criteriaAndModifiedResistivity.minStrikeForSelecting &&
			params.strike <= criteriaAndModifiedResistivity.maxStrikeForSelecting) {
			params.strike = criteriaAndModifiedResistivity.modStrike;
			params.isFixedStrike = true;
		}
		// dip
		if (!params.isFixedDip &&
			params.dip >= criteriaAndModifiedResistivity.minDipForSelecting &&
			params.dip <= criteriaAndModifiedResistivity.maxDipForSelecting) {
			params.dip = criteriaAndModifiedResistivity.modDip;
			params.isFixedDip = true;
		}
		break;
	case GENERAL_ANISOTROPY:
		// rho_XX
		if (!params.isFixedRhoXX &&
			params.rhoXX >= criteriaAndModifiedResistivity.minRhoXXForSelecting &&
			params.rhoXX <= criteriaAndModifiedResistivity.maxRhoXXForSelecting) {
			params.rhoXX = criteriaAndModifiedResistivity.modRhoXX;
			params.isFixedRhoXX = true;
		}
		// rho_YY
		if (!params.isFixedRhoYY &&
			params.rhoYY >= criteriaAndModifiedResistivity.minRhoYYForSelecting &&
			params.rhoYY <= criteriaAndModifiedResistivity.maxRhoYYForSelecting) {
			params.rhoYY = criteriaAndModifiedResistivity.modRhoYY;
			params.isFixedRhoYY = true;
		}
		// rho_ZZ
		if (!params.isFixedRhoZZ &&
			params.rhoZZ >= criteriaAndModifiedResistivity.minRhoZZForSelecting &&
			params.rhoZZ <= criteriaAndModifiedResistivity.maxRhoZZForSelecting) {
			params.rhoZZ = criteriaAndModifiedResistivity.modRhoZZ;
			params.isFixedRhoZZ = true;
		}
		// strike
		if (!params.isFixedStrike &&
			params.strike >= criteriaAndModifiedResistivity.minStrikeForSelecting &&
			params.strike <= criteriaAndModifiedResistivity.maxStrikeForSelecting) {
			params.strike = criteriaAndModifiedResistivity.modStrike;
			params.isFixedStrike = true;
		}
		// dip
		if (!params.isFixedDip &&
			params.dip >= criteriaAndModifiedResistivity.minDipForSelecting &&
			params.dip <= criteriaAndModifiedResistivity.maxDipForSelecting) {
			params.dip = criteriaAndModifiedResistivity.modDip;
			params.isFixedDip = true;
		}
		// slant
		if (!params.isFixedSlant &&
			params.slant >= criteriaAndModifiedResistivity.minSlantForSelecting &&
			params.slant <= criteriaAndModifiedResistivity.maxSlantForSelecting) {
			params.slant = criteriaAndModifiedResistivity.modSlant;
			params.isFixedSlant = true;
		}
		break;
	default:
		std::cout << "Unsupported anisotropy type : " << params.type << std::endl;
		break;
	}

}
