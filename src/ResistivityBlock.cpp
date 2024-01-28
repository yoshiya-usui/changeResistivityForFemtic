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
ResistivityBlock::ResistivityBlock(){
}

// Destructer
ResistivityBlock::~ResistivityBlock(){
}

// Change resistivity of the selected elements
void ResistivityBlock::changeResistivityOfSelectedElements( const std::set<int>& elementsSelected, const double resistivityMod,
														   const double resistivityModMin, const double resistivityMax ){

	std::set<int> elementsSelectedMod = elementsSelected;
	const int nBlkOrg = getNumResistivityBlockTotal();
	for( int iBlk = 0; iBlk < nBlkOrg; ++iBlk ){
		const std::set<int> elements = getElementsFromBlock(iBlk);
		bool allElementsSelected(true);
		for( std::set<int>::const_iterator itr = elements.begin(); itr != elements.end(); ++itr ){
			if( elementsSelected.find(*itr) == elementsSelected.end() ){
				allElementsSelected = false;
				break;
			}
		}
		if(allElementsSelected){
			ResistivityBlockInformation& info = m_resistivityBlockInfo[iBlk];
			info.resistivityValue = resistivityMod;
			info.resistivityValueMin = resistivityModMin;
			info.resistivityValueMax = resistivityMax;
			info.type = FIXED_AND_ISOLATED;
			for( std::set<int>::const_iterator itr = elements.begin(); itr != elements.end(); ++itr ){
				elementsSelectedMod.erase(*itr);
			}
		}
	}

	int iBlk(nBlkOrg);
	for( std::set<int>::const_iterator itr = elementsSelectedMod.begin(); itr != elementsSelectedMod.end(); ++itr, ++iBlk ){
		const int iElem = *itr;
		const int iBlkOrg = getBlockFromElement(iElem);
		const ResistivityBlockInformation& infoOrg = m_resistivityBlockInfo[iBlkOrg];
		ResistivityBlockInformation info;
		info.resistivityValue = resistivityMod;
		info.resistivityValueMin = resistivityModMin;
		info.resistivityValueMax = resistivityMax;
		info.type = FIXED_AND_ISOLATED;
		info.weightingConstant = infoOrg.weightingConstant;
		m_resistivityBlockInfo.push_back(info);
		m_elementToBlocks[iElem] = iBlk;
		m_blockToElements[iBlkOrg].erase(iElem);
		std::set<int> setBuf;
		setBuf.insert(iElem);
		m_blockToElements.push_back(setBuf);
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

	int nBlk(0);
	inFile >> nBlk;

	m_resistivityBlockInfo.reserve(nBlk);
#ifdef _DEBUG_WRITE
	std::cout << nElem << " " << nBlk << std::endl; // For debug
#endif
	
	for( int iElem = 0; iElem < nElem; ++iElem ){
		int idum(0);
		int iBlk(0);// Resistivity block ID
		inFile >> idum >> iBlk;
		if( idum != iElem ){
			std::cerr << "Error : Element index is wrong !!" << std::endl;
			exit(1);
		}
		m_elementToBlocks.insert( std::make_pair(iElem,iBlk) );
		if( iBlk >= nBlk || iBlk < 0 )	{
			std::cerr << "Error : Resistivity block index " << iBlk << " of element " << iElem << " is improper !!" << std::endl;
			exit(1);
		}
#ifdef _DEBUG_WRITE
		std::cout << iElem << " " << iBlk << std::endl; // For debug
#endif
	}
	
	for( int iBlk = 0; iBlk < nBlk; ++iBlk ){
		int idum(0);
		ResistivityBlockInformation info;
		inFile >> idum;
		if( idum != iBlk ){
			std::cerr << "Error : Block index is wrong !!" << std::endl;
			exit(1);
		}
		inFile >> info.resistivityValue;
		inFile >> info.resistivityValueMin;
		inFile >> info.resistivityValueMax;
		inFile >> info.weightingConstant;
		inFile >> info.type;
		m_resistivityBlockInfo.push_back(info);
	}

	//if( !isFixedResistivityValue(0) ){
	//	std::cerr << "Error : Resistivity block 0 must be the air. And, its resistivity must be fixed." << std::endl;
	//	exit(1);
	//}
	inFile.close();

	m_blockToElements.reserve(nBlk);
	for( int iBlk = 0; iBlk < nBlk; ++iBlk ){
		std::set<int> dummy;
		m_blockToElements.push_back(dummy);
	}
	for( int iElem = 0; iElem < nElem; ++iElem ){
		const int iBlk = getBlockFromElement(iElem);
		m_blockToElements[iBlk].insert(iElem);
	}

#ifdef _DEBUG_WRITE
	for( int iBlk = 0; iBlk < nBlk; ++iBlk ){
		int icount(0);
		for( std::set<int>::const_iterator itr = m_blockToElements[iBlk].begin(); itr != m_blockToElements[iBlk].end(); ++itr, ++icount ){
			std::cout << " m_blockID2Elements[ " << iBlk << " ][ " << icount << "] : " << *itr << std::endl;
		}
	}
#endif

}

int ResistivityBlock::getBlockFromElement( const int iElem ) const{
	std::map<int, int>::const_iterator itr = m_elementToBlocks.find(iElem);
	if( itr == m_elementToBlocks.end() ){
		std::cerr << "Error : Element index " << iElem << " is not found in m_elementToBlocks." << std::endl;
		exit(1);
	}
	return itr->second;
}

// Get resistivity value from resisitivity block index
double ResistivityBlock::getResistivityValueFromBlockIndex( const int iBlk ) const{
	assert( iBlk >= 0 );
	assert( iBlk < static_cast<int>(m_resistivityBlockInfo.size()) );
	const ResistivityBlockInformation& info = m_resistivityBlockInfo[iBlk];
	return info.resistivityValue;
}

// Get total number of resistivity blocks
int ResistivityBlock::getNumResistivityBlockTotal() const{
	return static_cast<int>(m_resistivityBlockInfo.size());
}

// Get flag specifing whether resistivity value of resistivity block is fixed or not
bool ResistivityBlock::isFixedResistivityValue( const int iBlk ) const{
	assert( iBlk >= 0 );
	assert( iBlk < static_cast<int>(m_resistivityBlockInfo.size()) );
	const ResistivityBlockInformation& info = m_resistivityBlockInfo[iBlk];
	switch(info.type){
		case ResistivityBlock::FREE_AND_CONSTRAINED:// Go through
		case ResistivityBlock::FREE_AND_ISOLATED:
			return false;
			break;
		case ResistivityBlock::FIXED_AND_ISOLATED:// Go through
		case ResistivityBlock::FIXED_AND_CONSTRAINED:
			return true;
			break;
		default:
			std::cerr << "Error : Type of resistivity block is unknown !! : " << info.type << std::endl;
			exit(1);
			break;
	}
	return true;
}

// Get element indexes from resistivity block index
const std::set<int> ResistivityBlock::getElementsFromBlock( const int iBlk ) const{
	return m_blockToElements[iBlk];
}

// Output data of resisitivity block model to file
void ResistivityBlock::outputResisitivityBlock( const MeshData* const MeshData, const int iterNum ) const{

	std::ostringstream fileName;
	fileName << "resistivity_block_iter" << iterNum << ".mod.dat";

	FILE *fp;
	if( (fp = fopen( fileName.str().c_str(), "w")) == NULL ) {
		std::cerr  << "File open error !! : " << fileName.str() << std::endl;
		exit(1);
	}

	const int numElems = MeshData->getNumElemTotal();
	const int numBlocks = static_cast<int>(m_resistivityBlockInfo.size());
	fprintf(fp, "%10d%10d\n",numElems, numBlocks );
	for( int iElem = 0; iElem < numElems; ++iElem ){
		fprintf(fp, "%10d%10d\n", iElem, getBlockFromElement(iElem) );
	}
	for( int iBlk = 0; iBlk < numBlocks; ++iBlk ){
		const ResistivityBlockInformation& info = m_resistivityBlockInfo[iBlk];
		fprintf(fp, "%10d%5s%15e%15e%15e%15e%10d\n", iBlk, "     ",
			info.resistivityValue, 
			info.resistivityValueMin,
			info.resistivityValueMax,
			info.weightingConstant,
			info.type);
	}
	
	fclose(fp);

}

// Output resistivity values to binary file
void ResistivityBlock::outputResistivityValuesToBinary( const bool isTetra, const MeshData* const MeshData, const int iterNum ) const{

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

	if(isTetra){
		strcpy( line, "tetra4" );
	}else{
		strcpy( line, "hexa8" );
	}
	fout.write( line, 80 );

	const int nElem = MeshData->getNumElemTotal();
	for( int iElem = 0 ; iElem < nElem; ++iElem ){
		const int iBlk = getBlockFromElement(iElem);
		const ResistivityBlockInformation& info = m_resistivityBlockInfo[iBlk];
		float dbuf = static_cast<float>(info.resistivityValue);
		fout.write( (char*) &dbuf, sizeof( float ) );
	}

	fout.close();

}
