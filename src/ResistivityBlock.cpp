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

#include "ResistivityBlock.h"

#include <sstream>
#include <fstream>
#include <string.h>
#include <assert.h>
#include <iomanip>

// Constructer
ResistivityBlock::ResistivityBlock()
{}

// Destructer
ResistivityBlock::~ResistivityBlock()
{}

// Read data of resisitivity block model from input file
void ResistivityBlock::inputResistivityBlock(const int iterNum) {

	std::ostringstream inputFile;
	inputFile << "resistivity_block_iter" << iterNum << ".dat";
	std::ifstream inFile(inputFile.str().c_str(), std::ios::in);

	if (inFile.fail())
	{
		std::cerr << "File open error : " << inputFile.str().c_str() << " !!" << std::endl;
		exit(1);
	}

	int nElem(0);
	inFile >> nElem;

	int nBlk(0);
	inFile >> nBlk;

#ifdef _DEBUG_WRITE
	std::cout << nElem << " " << nBlk << std::endl; // For debug
#endif

	for (int iElem = 0; iElem < nElem; ++iElem) {
		int idum(0);
		int iBlk(0);// Resistivity block ID
		inFile >> idum >> iBlk;
		if (idum != iElem) {
			std::cerr << "Error : Element index is wrong !!" << std::endl;
			exit(1);
		}
		m_elementToBlocks.insert(std::make_pair(iElem, iBlk));
		if (iBlk >= nBlk || iBlk < 0) {
			std::cerr << "Error : Resistivity block index " << iBlk << " of element " << iElem << " is improper !!" << std::endl;
			exit(1);
		}
#ifdef _DEBUG_WRITE
		std::cout << iElem << " " << iBlk << std::endl; // For debug
#endif
	}

	inputResistivityValues(nElem, nBlk, inFile);
	inFile.close();

	m_blockToElements.reserve(nBlk);
	for (int iBlk = 0; iBlk < nBlk; ++iBlk) {
		std::set<int> dummy;
		m_blockToElements.push_back(dummy);
	}
	for (int iElem = 0; iElem < nElem; ++iElem) {
		const int iBlk = getBlockFromElement(iElem);
		m_blockToElements[iBlk].insert(iElem);
	}

#ifdef _DEBUG_WRITE
	for (int iBlk = 0; iBlk < nBlk; ++iBlk) {
		int icount(0);
		for (std::set<int>::const_iterator itr = m_blockToElements[iBlk].begin(); itr != m_blockToElements[iBlk].end(); ++itr, ++icount) {
			std::cout << " m_blockID2Elements[ " << iBlk << " ][ " << icount << "] : " << *itr << std::endl;
		}
	}
#endif

}

int ResistivityBlock::getBlockFromElement(const int iElem) const {
	std::map<int, int>::const_iterator itr = m_elementToBlocks.find(iElem);
	if (itr == m_elementToBlocks.end()) {
		std::cerr << "Error : Element index " << iElem << " is not found in m_elementToBlocks." << std::endl;
		exit(1);
	}
	return itr->second;
}

// Get element indexes from resistivity block index
const std::set<int> ResistivityBlock::getElementsFromBlock(const int iBlk) const {
	return m_blockToElements[iBlk];
}

// Output data of resisitivity block model to file
void ResistivityBlock::outputResistivityBlock(const MeshData* const MeshData, const int iterNum) const {

	std::ostringstream fileName;
	fileName << "resistivity_block_iter" << iterNum << ".mod.dat";

	FILE* fp;
	if ((fp = fopen(fileName.str().c_str(), "w")) == NULL) {
		std::cerr << "File open error !! : " << fileName.str() << std::endl;
		exit(1);
	}

	const int numElems = MeshData->getNumElemTotal();
	const int numBlocks = static_cast<int>(getNumResistivityBlockTotal());
	fprintf(fp, "%10d%10d\n", numElems, numBlocks);
	for (int iElem = 0; iElem < numElems; ++iElem) {
		fprintf(fp, "%10d%10d\n", iElem, getBlockFromElement(iElem));
	}
	outputResistivityValues(iterNum, fp);

	fclose(fp);

}
