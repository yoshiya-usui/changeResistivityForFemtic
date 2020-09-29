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
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "Util.h"
#include "MeshDataTetraElement.h"
#include "CommonParameters.h"

const double MeshDataTetraElement::m_eps = 1.0e-12;

// Constructer
MeshDataTetraElement::MeshDataTetraElement():
	m_numElemOnLandSurface(0),
	m_elemOnLandSurface(NULL),
	m_faceLandSurface(NULL)
{
	m_numNodeOneElement = 4;
	m_numEdgeOneElement = 6;
	m_numNodeOnFaceOneElement = 3;
	m_numNeighborElement = 4;

	for ( int i = 0; i < 6; ++i ){
		m_facesOfElementsBoundaryPlanes[i] = NULL;
	}

	// m_faceID2NodeID
	m_faceID2NodeID[0][0] = 1;
	m_faceID2NodeID[0][1] = 2;
	m_faceID2NodeID[0][2] = 3;

	m_faceID2NodeID[1][0] = 0;
	m_faceID2NodeID[1][1] = 3;
	m_faceID2NodeID[1][2] = 2;

	m_faceID2NodeID[2][0] = 0;
	m_faceID2NodeID[2][1] = 1;
	m_faceID2NodeID[2][2] = 3;

	m_faceID2NodeID[3][0] = 0;
	m_faceID2NodeID[3][1] = 2;
	m_faceID2NodeID[3][2] = 1;

	// m_faceID2EdgeID
	m_faceID2EdgeID[0][0] = 3;
	m_faceID2EdgeID[0][1] = 5;
	m_faceID2EdgeID[0][2] = 4;

	m_faceID2EdgeID[1][0] = 2;
	m_faceID2EdgeID[1][1] = 5;
	m_faceID2EdgeID[1][2] = 1;

	m_faceID2EdgeID[2][0] = 0;
	m_faceID2EdgeID[2][1] = 4;
	m_faceID2EdgeID[2][2] = 2;

	m_faceID2EdgeID[3][0] = 1;
	m_faceID2EdgeID[3][1] = 3;
	m_faceID2EdgeID[3][2] = 0;

	// m_edgeID2NodeID
	m_edgeID2NodeID[0][0] = 0;
	m_edgeID2NodeID[0][1] = 1;

	m_edgeID2NodeID[1][0] = 0;
	m_edgeID2NodeID[1][1] = 2;

	m_edgeID2NodeID[2][0] = 0;
	m_edgeID2NodeID[2][1] = 3;

	m_edgeID2NodeID[3][0] = 1;
	m_edgeID2NodeID[3][1] = 2;

	m_edgeID2NodeID[4][0] = 3;
	m_edgeID2NodeID[4][1] = 1;

	m_edgeID2NodeID[5][0] = 2;
	m_edgeID2NodeID[5][1] = 3;

}

// Destructer
MeshDataTetraElement::~MeshDataTetraElement(){

	for ( int i = 0; i < 6; ++i ){
		if( m_facesOfElementsBoundaryPlanes[i] != NULL ){
			delete[] m_facesOfElementsBoundaryPlanes[i];
			m_facesOfElementsBoundaryPlanes[i] = NULL;
		}
	}

	if( m_elemOnLandSurface != NULL ){
		delete[] m_elemOnLandSurface;
		m_elemOnLandSurface = NULL;
	}

	if( m_faceLandSurface != NULL ){
		delete[] m_faceLandSurface;
		m_faceLandSurface = NULL;
	}

}

// Input mesh data from "mesh.dat"
void MeshDataTetraElement::inputMeshData(){

	std::ifstream inFile( "mesh.dat", std::ios::in );
	if( inFile.fail() )
	{
		std::cerr << "File open error : mesh.dat !!" << std::endl;
		exit(1);
	}

	std::string sbuf;
	inFile >> sbuf;

	if( sbuf.substr(0,5).compare("TETRA") != 0 ){
		std::cerr << "Mesh data written in mesh.dat is different from the ones of tetrahedral element !!" << std::endl;
		exit(1);
	}

	int ibuf(0);

	inFile >> ibuf;
	if( ibuf > 0 ){
		m_numNodeTotal = ibuf;
	}else{
		std::cerr << "Total number of nodes is less than or equal to zero ! : " << ibuf << std::endl;
		exit(1);
	}

	if( m_xCoordinatesOfNodes != NULL ){
		delete[] m_xCoordinatesOfNodes;	
	}
	m_xCoordinatesOfNodes = new double[m_numNodeTotal];

	if( m_yCoordinatesOfNodes != NULL ){
		delete[] m_yCoordinatesOfNodes;	
	}
	m_yCoordinatesOfNodes = new double[m_numNodeTotal];

	if( m_zCoordinatesOfNodes != NULL ){
		delete[] m_zCoordinatesOfNodes;	
	}
	m_zCoordinatesOfNodes = new double[m_numNodeTotal];

	for( int iNode = 0; iNode < m_numNodeTotal; ++iNode ){

		int idum(0);
		inFile >> idum >> m_xCoordinatesOfNodes[iNode] >> m_yCoordinatesOfNodes[iNode] >> m_zCoordinatesOfNodes[iNode];

#ifdef _DEBUG_WRITE
		std::cout << idum << " " << m_xCoordinatesOfNodes[iNode] << " " << m_yCoordinatesOfNodes[iNode] << " " << m_zCoordinatesOfNodes[iNode] << std::endl; // For debug
#endif

	}

	inFile >> ibuf;
	if( ibuf > 0 ){
		m_numElemTotal = ibuf;
	}else{
		std::cerr << "Total number of elements is less than or equal to zero ! : " << ibuf << std::endl;
		exit(1);
	}

	if( m_neighborElements != NULL ){
		delete[] m_neighborElements;
	}
	m_neighborElements = new int[ m_numElemTotal * 4 ];

	if( m_nodesOfElements == NULL ){
		delete[] m_nodesOfElements;
	}
	m_nodesOfElements = new int[ m_numElemTotal * m_numNodeOneElement ];

	for( int iElem = 0; iElem < m_numElemTotal; ++iElem ){

		int idum(0);
		inFile >> idum;

		// IDs of neighbor Elements
		for( int i = 0; i < 4; ++i ){
			int neib(0);
			inFile >> neib;

			m_neighborElements[ iElem * 4 + i ] = neib;

		}

		// Nodes of the element
		for( int i = 0; i < m_numNodeOneElement; ++i ){
			int node(0);
			inFile >> node;

			m_nodesOfElements[ iElem * m_numNodeOneElement + i ] = node;

		}

	}

#ifdef _DEBUG_WRITE
	for( int iElem = 0; iElem < m_numElemTotal; ++iElem ){

		std::cout << iElem << " ";

		// IDs of neighbor Elements
		for( int i = 0; i < 4; ++i ){
			std::cout << m_neighborElements[ iElem * 4 + i ] << " ";
		}

		// Nodes of the element
		for( int i = 0; i < m_numNodeOneElement; ++i ){
			std::cout << m_nodesOfElements[ iElem * m_numNodeOneElement + i ] << " ";
		}

		std::cout << std::endl;
	}
#endif


	for( int iPlane = 0; iPlane < 6; ++iPlane ){// Loop of boundary planes

		int nElemOnPlane;
		inFile >> nElemOnPlane;
		if( nElemOnPlane > 0 ){
			m_numElemOnBoundaryPlanes[iPlane] = nElemOnPlane;
		}else{
			std::cerr << "Number of faces belonging plane " << iPlane << " is less than or equal to zero ! : " << nElemOnPlane << std::endl;
			exit(1);
		}

#ifdef _DEBUG_WRITE
		std::cout << nElemOnPlane << std::endl; // For debug
#endif

		if( m_elemBoundaryPlanes[iPlane] != NULL ){
			delete [] m_elemBoundaryPlanes[iPlane];
		}
		m_elemBoundaryPlanes[iPlane] = new int[ nElemOnPlane ];

		if( m_facesOfElementsBoundaryPlanes[iPlane] != NULL ){
			delete [] m_facesOfElementsBoundaryPlanes[iPlane];	
		}
		m_facesOfElementsBoundaryPlanes[iPlane] = new int[ nElemOnPlane ];

		// Set elements belonging to the boundary planes
		for( int iElem = 0; iElem < nElemOnPlane; ++iElem ){		

			inFile >> m_elemBoundaryPlanes[iPlane][iElem] >> m_facesOfElementsBoundaryPlanes[iPlane][iElem];

			if( m_elemBoundaryPlanes[iPlane][iElem] < 0 || m_elemBoundaryPlanes[iPlane][iElem] >= m_numElemTotal ){
				std::cerr << "Element ID of plane " << iPlane << " is out of range !! : " << m_elemBoundaryPlanes[iPlane][iElem] << std::endl;
				exit(1);
			}
			if( m_facesOfElementsBoundaryPlanes[iPlane][iElem] < 0 || m_facesOfElementsBoundaryPlanes[iPlane][iElem] >= 4 ){
				std::cerr << "Face ID of plane " << iPlane << " is out of range !! : " << m_facesOfElementsBoundaryPlanes[iPlane][iElem] << std::endl;
				exit(1);
			}
			
#ifdef _DEBUG_WRITE
			std::cout << m_elemBoundaryPlanes[iPlane][iElem] << " "  << m_facesOfElementsBoundaryPlanes[iPlane][iElem] << std::endl; // For debug
#endif

		}
		
	}

	inFile >> ibuf;
	if( ibuf > 0 ){
		m_numElemOnLandSurface = ibuf;
	}else{
		std::cerr << "Total number of faces on the land surface is less than or equal to zero ! : " << ibuf << std::endl;
		exit(1);
	}

#ifdef _DEBUG_WRITE
		std::cout << "m_numElemOnLandSurface = " << m_numElemOnLandSurface << std::endl; // For debug
#endif

	if( m_elemOnLandSurface != NULL ){
		delete [] m_elemOnLandSurface;	
	}
	m_elemOnLandSurface = new int[ m_numElemOnLandSurface ];

	if( m_faceLandSurface != NULL ){
		delete [] m_faceLandSurface;	
	}
	m_faceLandSurface = new int[ m_numElemOnLandSurface ];

	// Set faces belonging to the boundary planes
	for( int iElem = 0; iElem < m_numElemOnLandSurface; ++iElem ){		

		inFile >> m_elemOnLandSurface[iElem] >> m_faceLandSurface[iElem];

		if( m_elemOnLandSurface[iElem] < 0 || m_elemOnLandSurface[iElem] >= m_numElemTotal ){
			std::cerr << "Element ID of land surface is out of range !! : " << m_elemOnLandSurface[iElem] << std::endl;
			exit(1);
		}
		if( m_faceLandSurface[iElem] < 0 || m_faceLandSurface[iElem] >= 4 ){
			std::cerr << "Face ID of land surface is out of range !! : " << m_faceLandSurface[iElem] << std::endl;
			exit(1);
		}
			
#ifdef _DEBUG_WRITE
		std::cout << m_elemOnLandSurface[iElem] << " "  << m_faceLandSurface[iElem] << std::endl; // For debug
#endif

	}

	inFile.close();

}

// Get local face ID of elements belonging to the boundary planes
int MeshDataTetraElement::getFaceIDLocalFromElementBoundaryPlanes( const int iPlane, const int iElem ) const{

	//if( iElem < 0 || iElem >= m_numElemTotal ){
	//	OutputFiles::m_logFile << " Error : iElem is out of range in getFaceIDLocalFromElementBoundaryPlanes. iElem = " << iElem << std::endl;
	//	exit(1);
	//}

	//if( iPlane < 0 || iPlane >= 6 ){
	//	OutputFiles::m_logFile << " Error : iPlane is out of range in getFaceIDLocalFromElementBoundaryPlanes !! : iPlane = " << iPlane << std::endl;
	//	exit(1);
	//}
	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iPlane >= 0 );
	assert( iPlane < 6 );

	return m_facesOfElementsBoundaryPlanes[iPlane][iElem];

}

// Get local node ID  from local face ID
int MeshDataTetraElement::getNodeIDLocalFromFaceIDLocal( const int iFace, const int num ) const{

	//if( iFace < 0 || iFace >= 4 ){
	//	OutputFiles::m_logFile << " Error : iFace is out of range in getNodeIDLocalFromFaceIDLocal. iFace = " << iFace << std::endl;
	//	exit(1);
	//}

	//if( num < 0 || num >= 3 ){
	//	OutputFiles::m_logFile << " Error : num is out of range in getNodeIDLocalFromFaceIDLocal. num = " << num << std::endl;
	//	exit(1);
	//}
	assert( iFace >= 0 );
	assert( iFace < 4 );
	assert( num >= 0 );
	assert( num < 3 );

	return m_faceID2NodeID[iFace][num];

}

// Get local node ID from local edge ID
int MeshDataTetraElement::getNodeIDLocalFromEdgeIDLocal( const int iEdge, const int num ) const{

	//if( iEdge < 0 || iEdge >= 6 ){
	//	OutputFiles::m_logFile << " Error : iEdge is out of range in getNodeIDLocalFromElementAndEdge. iEdge = " << iEdge << std::endl;
	//	exit(1);
	//}

	//if( num != 0 && num != 1 ){
	//	OutputFiles::m_logFile << " Error : num must be 0 or 1 in getNodeIDLocalFromElementAndEdge. num = " << num << std::endl;
	//	exit(1);
	//}
	assert( iEdge >= 0 );
	assert( iEdge < 6 );
	assert( num == 0 || num == 1 );

	return m_edgeID2NodeID[iEdge][num];

}

// Get ID of the nodes of specified element and edge
int MeshDataTetraElement::getNodeIDGlobalFromElementAndEdge( const int iElem, const int iEdge, const int num ) const{

	//if( iElem < 0 || iElem >= m_numElemTotal ){
	//	OutputFiles::m_logFile << " Error : iElem is out of range in getNodeIDFromElementAndEdge. iElem = " << iElem << std::endl;
	//	exit(1);
	//}

	//if( iEdge < 0 || iEdge >= 6 ){
	//	OutputFiles::m_logFile << " Error : iEdge is out of range in getNodeIDFromElementAndEdge. iEdge = " << iEdge << std::endl;
	//	exit(1);
	//}

	//if( num != 0 && num != 1 ){
	//	OutputFiles::m_logFile << " Error : num must be 0 or 1 in getNodeIDFromElementAndEdge. num = " << num << std::endl;
	//	exit(1);
	//}
	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iEdge >= 0 );
	assert( iEdge < 6 );
	assert( num == 0 || num == 1 );

	return getNodesOfElements( iElem, m_edgeID2NodeID[iEdge][num] );

}

// Get global node ID of specified element and face
int MeshDataTetraElement::getNodeIDGlobalFromElementAndFace( const int iElem, const int iFace, const int num ) const{

	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iFace >= 0 );
	assert( iFace < 4 );
	assert( num >= 0 );
	assert( num < 3 );

	return getNodesOfElements( iElem, m_faceID2NodeID[iFace][num] );

}

// Get global node ID of specified element belonging to the boundary planes  
int MeshDataTetraElement::getNodeIDGlobalFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const{

	const int elemID3D = getElemBoundaryPlanes( iPlane, iElem );
	const int faceID3D = getFaceIDLocalFromElementBoundaryPlanes( iPlane, iElem );

	//if( num < 0 || num >= 3 ){
	//	OutputFiles::m_logFile << " Error : num is out of range !! num = " << num << std::endl;
	//	exit(1);
	//}
	assert( num >= 0 );
	assert( num < 3 );

	int comp(num);

	if( iPlane == MeshData::YZMinus || iPlane == MeshData::ZXMinus ){// 2D plane is minus side

		if( num == 1 ){
			comp = 2;
		}else if( num == 2 ){
			comp = 1;
		}

	}

	return getNodesOfElements( elemID3D, m_faceID2NodeID[faceID3D][comp] );

}

// Get X coordinate of node of specified element belonging to the boundary planes  
double MeshDataTetraElement::getCoordXFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const{

	return getXCoordinatesOfNodes( getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, num ) );

}

// Get Y coordinate of node of specified element belonging to the boundary planes  
double MeshDataTetraElement::getCoordYFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const{

	return getYCoordinatesOfNodes( getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, num ) );

}

// Get Z coordinate of node of specified element belonging to the boundary planes  
double MeshDataTetraElement::getCoordZFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const{

	return getZCoordinatesOfNodes( getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, num ) );

}

// Get global node ID from ID of element belonging to the boundary planes and its edge ID
// [note] : node ID is outputed as they make a clockwise turn around +X or +Y direction
int MeshDataTetraElement::getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( const int iPlane, const int iElem, const int iEdge, const int num ) const{

	//const int elemID3D = getElemBoundaryPlanes( iPlane, iElem );
	//const int edgeID3D = getEdgeIDLocalFromElementBoundaryPlanes( iPlane, iElem, iEdge );
	//return getNodeIDGlobalFromElementAndEdge( elemID3D, edgeID3D, num );

	//const int elemID3D = getElemBoundaryPlanes( iPlane, iElem );
	//const int faceID3D = getFaceIDLocalFromElementBoundaryPlanes( iPlane, iElem );

	//int comp(0);

	//if( iPlane == MeshData::YZPlus || iPlane == MeshData::ZXPlus ){// node ID is 

	//	if( ( iEdge == 0 && num == 0 ) || ( iEdge == 2 && num == 1 ) ){
	//		comp = 0;
	//	}else if( ( iEdge == 1 && num == 0 ) || ( iEdge == 0 && num == 1 ) ){
	//		comp = 1;
	//	}else if( ( iEdge == 2 && num == 0 ) || ( iEdge == 1 && num == 1 ) ){
	//		comp = 2;
	//	}else{
	//		OutputFiles::m_logFile << " Error : Either iEdge or num is out of range !! iEdge = " << iEdge << ", num = " << num << std::endl;
	//		exit(1);
	//	}

	//}else{

	//	if( ( iEdge == 0 && num == 0 ) || ( iEdge == 2 && num == 1 ) ){
	//		comp = 0;
	//	}else if( ( iEdge == 1 && num == 0 ) || ( iEdge == 0 && num == 1 ) ){
	//		comp = 2;
	//	}else if( ( iEdge == 2 && num == 0 ) || ( iEdge == 1 && num == 1 ) ){
	//		comp = 1;
	//	}else{
	//		OutputFiles::m_logFile << " Error : Either iEdge or num is out of range !! iEdge = " << iEdge << ", num = " << num << std::endl;
	//		exit(1);
	//	}

	//}

	 //	return getNodesOfElements( elemID3D, m_faceID2NodeID[faceID3D][comp] );

	if( ( iEdge == 0 && num == 0 ) || ( iEdge == 2 && num == 1 ) ){
		return getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 0 );
	}else if( ( iEdge == 1 && num == 0 ) || ( iEdge == 0 && num == 1 ) ){
		return getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 1 );
	}else if( ( iEdge == 2 && num == 0 ) || ( iEdge == 1 && num == 1 ) ){
		return getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 2 );
	}else{
		std::cerr << " Error : Either iEdge or num is out of range !! iEdge = " << iEdge << ", num = " << num << std::endl;
		exit(1);
	}


}

// Get local edge ID from local face ID
int MeshDataTetraElement::getEdgeIDLocalFromFaceIDLocal( const int iFace, const int num ) const{

	//if( iFace < 0 || iFace >= 4 ){
	//	OutputFiles::m_logFile << " Error : iFace is out of range. iFace = " << iFace << std::endl;
	//	exit(1);
	//}

	//if( num < 0 || num >= 3 ){
	//	OutputFiles::m_logFile << " Error : num is out of range. num = " << num << std::endl;
	//	exit(1);
	//}

	assert( iFace >= 0 );
	assert( iFace < 4 );
	assert( num >= 0 );
	assert( num < 3 );

	return m_faceID2EdgeID[iFace][num];

}

// Copy constructer
MeshDataTetraElement::MeshDataTetraElement(const MeshDataTetraElement& rhs){
	std::cerr << "Error : Copy constructer of the class MeshDataTetraElement is not implemented." << std::endl;
	exit(1);
}

// Assignment operator
MeshDataTetraElement& MeshDataTetraElement::operator=(const MeshDataTetraElement& rhs){
	std::cerr << "Error : Assignment operator of the class MeshDataTetraElement is not implemented." << std::endl;
	exit(1);
}

// Function determine if two segments intersect or not
bool MeshDataTetraElement::intersectTwoSegments( const CommonParameters::locationXY& startPointOf1stSegment, const CommonParameters::locationXY& endPointOf1stSegment,
	const CommonParameters::locationXY& startPointOf2ndSegment, const CommonParameters::locationXY& endPointOf2ndSegment ) const{

	const double val1 = ( endPointOf1stSegment.Y - startPointOf1stSegment.Y ) * ( startPointOf2ndSegment.X - startPointOf1stSegment.X ) + ( endPointOf1stSegment.X - startPointOf1stSegment.X ) * ( startPointOf1stSegment.Y - startPointOf2ndSegment.Y );
	const double val2 = ( endPointOf1stSegment.Y - startPointOf1stSegment.Y ) * (   endPointOf2ndSegment.X - startPointOf1stSegment.X ) + ( endPointOf1stSegment.X - startPointOf1stSegment.X ) * ( startPointOf1stSegment.Y -   endPointOf2ndSegment.Y );

	//const double eps = 1.0e-9;

//#ifdef _DEBUG_WRITE	
//	std::cout << "startPointOf1stSegment : " << startPointOf1stSegment.X << " "  <<  startPointOf1stSegment.Y << std::endl; // For debug
//	std::cout << "endPointOf1stSegment : " << endPointOf1stSegment.X << " "  <<  endPointOf1stSegment.Y << std::endl; // For debug
//	std::cout << "startPointOf2ndSegment : " << startPointOf2ndSegment.X << " "  <<  startPointOf2ndSegment.Y << std::endl; // For debug
//	std::cout << "endPointOf2ndSegment : " << endPointOf2ndSegment.X << " "  <<  endPointOf2ndSegment.Y << std::endl; // For debug
//	std::cout << "val1 : " << ( endPointOf1stSegment.Y - startPointOf1stSegment.Y ) * ( startPointOf2ndSegment.X - startPointOf1stSegment.X ) << " " << ( endPointOf1stSegment.X - startPointOf1stSegment.X ) * ( startPointOf1stSegment.Y - startPointOf2ndSegment.Y ) << " " << val1 << std::endl; // For debug
//	std::cout << "val2 : " << ( endPointOf1stSegment.Y - startPointOf1stSegment.Y ) * (   endPointOf2ndSegment.X - startPointOf1stSegment.X ) << " " << ( endPointOf1stSegment.X - startPointOf1stSegment.X ) * ( startPointOf1stSegment.Y -   endPointOf2ndSegment.Y ) << " " << val2 << std::endl; // For debug
//#endif

	if( val1*val2 <= 0.0 ){

		const double val3 = ( endPointOf2ndSegment.Y - startPointOf2ndSegment.Y ) * ( startPointOf1stSegment.X - startPointOf2ndSegment.X ) + ( endPointOf2ndSegment.X - startPointOf2ndSegment.X ) * ( startPointOf2ndSegment.Y - startPointOf1stSegment.Y );
		const double val4 = ( endPointOf2ndSegment.Y - startPointOf2ndSegment.Y ) * (   endPointOf1stSegment.X - startPointOf2ndSegment.X ) + ( endPointOf2ndSegment.X - startPointOf2ndSegment.X ) * ( startPointOf2ndSegment.Y -   endPointOf1stSegment.Y );

//#ifdef _DEBUG_WRITE	
//		std::cout << "val3 : " << ( endPointOf2ndSegment.Y - startPointOf2ndSegment.Y ) * ( startPointOf1stSegment.X - startPointOf2ndSegment.X ) << " " << ( endPointOf2ndSegment.X - startPointOf2ndSegment.X ) * ( startPointOf2ndSegment.Y - startPointOf1stSegment.Y ) << " " << val3 << std::endl; // For debug
//		std::cout << "val4 : " << ( endPointOf2ndSegment.Y - startPointOf2ndSegment.Y ) * (   endPointOf1stSegment.X - startPointOf2ndSegment.X ) << " " << ( endPointOf2ndSegment.X - startPointOf2ndSegment.X ) * ( startPointOf2ndSegment.Y -   endPointOf1stSegment.Y ) << " " << val4 << std::endl; // For debug
//#endif

		if( fabs(val1*val2) < m_eps && fabs(val3*val4) < m_eps ){
			return false;
		}else if( val3*val4 <= 0.0 ){
			return true;
		}

		 //if( val3*val4 <= 0.0 ){
			//return true;
		 //}

	}

	return false;

}

// Function determine if two lines overlap or not
bool MeshDataTetraElement::overlapTwoLines( const CommonParameters::locationXY& coord1stLine1, const CommonParameters::locationXY& coord1stLine2,
	const CommonParameters::locationXY& coord2ndLine1, const CommonParameters::locationXY& coord2ndLine2 ) const{

//#ifdef _DEBUG_WRITE	
//	std::cout << "coord1stLine1 : " << coord1stLine1.X << " " << coord1stLine1.Y << std::endl; // For debug
//	std::cout << "coord1stLine2 : " << coord1stLine2.X << " " << coord1stLine2.Y << std::endl; // For debug
//	std::cout << "coord2ndLine1 : " << coord2ndLine1.X << " " << coord2ndLine1.Y << std::endl; // For debug
//	std::cout << "coord2ndLine2 : " << coord2ndLine2.X << " " << coord2ndLine2.Y << std::endl; // For debug
//#endif

	//const double eps = 1.0e-6;

	if( fabs( coord1stLine2.X - coord1stLine1.X ) < m_eps ){// If the first line is parallel to Y direction
		if( fabs( coord2ndLine2.X - coord2ndLine1.X ) < m_eps ){// If the second line is also parallel to Y direction
			if( fabs( coord1stLine1.X - coord2ndLine1.X ) < m_eps && fabs( coord1stLine2.X - coord2ndLine2.X ) < m_eps ){
				return true;
			}else{
				return false;
			}
		}else{// If the second line is not parallel to Y direction
			return false;
		}
	}

	if( fabs( coord1stLine2.Y - coord1stLine1.Y ) < m_eps ){// If the first line is parallel to X direction
		if( fabs( coord2ndLine2.Y - coord2ndLine1.Y ) < m_eps ){// If the second line is also parallel to X direction
			if( fabs( coord1stLine1.Y - coord2ndLine1.Y ) < m_eps && fabs( coord1stLine2.Y - coord2ndLine2.Y ) < m_eps ){
				return true;
			}else{
				return false;
			}
		}else{// If the second line is not parallel to X direction
			return false;
		}
	}

	const double val1 = ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X ) - ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y );

	const double val2 = ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X )*coord1stLine1.X
					  - ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y )*coord2ndLine1.X  
		  			  + ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.X - coord2ndLine1.X )*( coord2ndLine1.Y - coord1stLine1.Y ); 

//#ifdef _DEBUG_WRITE	
//	std::cout << "1 : " <<   ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X )*coord1stLine1.X << std::endl; // For debug
//	std::cout << "2 : " << - ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y )*coord2ndLine1.X << std::endl; // For debug
//	std::cout << "3 : " <<   ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.X - coord2ndLine1.X )*( coord2ndLine1.Y - coord1stLine1.Y ) << std::endl; // For debug
//	std::cout << "val1 : " << val1 << std::endl; // For debug
//	std::cout << "val2 : " << val2 << std::endl; // For debug
//#endif

	if( fabs(val1) < m_eps && fabs(val2) < m_eps ){
		return true;
	}

	return false;

}

// Function determine if two segments overlap or not
bool MeshDataTetraElement::overlapTwoSegments( const CommonParameters::locationXY& startPointOf1stSegment, const CommonParameters::locationXY& endPointOf1stSegment,
	const CommonParameters::locationXY& startPointOf2ndSegment, const CommonParameters::locationXY& endPointOf2ndSegment ) const{

	if( !overlapTwoLines( startPointOf1stSegment, endPointOf1stSegment, startPointOf2ndSegment, endPointOf2ndSegment ) ){
		// Two lines don't overlap
		return false;
	}
	
	const double innerProduct1 = calcInnerProduct2D( startPointOf1stSegment, endPointOf1stSegment, startPointOf1stSegment, endPointOf1stSegment );
	const double innerProduct2 = calcInnerProduct2D( startPointOf1stSegment, endPointOf1stSegment, startPointOf1stSegment, startPointOf2ndSegment );
	const double innerProduct3 = calcInnerProduct2D( startPointOf1stSegment, endPointOf1stSegment, startPointOf1stSegment, endPointOf2ndSegment );

//#ifdef _DEBUG_WRITE	
//	std::cout << "innerProduct1 : " << innerProduct1 << std::endl; // For debug
//	std::cout << "innerProduct2 : " << innerProduct2 << std::endl; // For debug
//	std::cout << "innerProduct3 : " << innerProduct3 << std::endl; // For debug
//#endif
//
	if( ( innerProduct2 < 0.0 && innerProduct3 < 0.0 ) || ( innerProduct2 > innerProduct1 && innerProduct3 > innerProduct1 ) ){
		return false;
	}

	return true;

}

// Calculate inner product of two vectors
double MeshDataTetraElement::calcInnerProduct2D( const CommonParameters::locationXY& startCoordOf1stVec, const CommonParameters::locationXY& endCoordOf1stVec,
	const CommonParameters::locationXY& startCoordOf2ndVec, const CommonParameters::locationXY& endCoordOf2ndVec) const{

	//return ( endCoordOf1stVec.X - startCoordOf1stVec.X )*( endCoordOf2ndVec.X - startCoordOf2ndVec.X )+( endCoordOf1stVec.Y - startCoordOf1stVec.Y )*( endCoordOf2ndVec.Y - startCoordOf2ndVec.Y );

	CommonParameters::Vector3D vec1 = { endCoordOf1stVec.X - startCoordOf1stVec.X, endCoordOf1stVec.Y - startCoordOf1stVec.Y, 0.0 };
	CommonParameters::Vector3D vec2 = { endCoordOf2ndVec.X - startCoordOf2ndVec.X, endCoordOf2ndVec.Y - startCoordOf2ndVec.Y, 0.0 };

	return calcInnerProduct( vec1, vec2 );

}

// Calculate coordinates of intersection point of two lines
void MeshDataTetraElement::calcCoordOfIntersectionPointOfTwoLines( const CommonParameters::locationXY& coord1stLine1, const CommonParameters::locationXY& coord1stLine2,
	const CommonParameters::locationXY& coord2ndLine1, const CommonParameters::locationXY& coord2ndLine2, CommonParameters::locationXY& coordIntersectionPoint) const{

	const double temp1 = ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X ) - ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y );

//#ifdef _DEBUG_WRITE	
//	std::cout << "coord1stLine1 : " << coord1stLine1.X << " "  <<  coord1stLine1.Y << std::endl; // For debug
//	std::cout << "coord1stLine2 : " << coord1stLine2.X << " "  <<  coord1stLine2.Y << std::endl; // For debug
//	std::cout << "coord2ndLine1 : " << coord2ndLine1.X << " "  <<  coord2ndLine1.Y << std::endl; // For debug
//	std::cout << "coord2ndLine2 : " << coord2ndLine2.X << " "  <<  coord2ndLine2.Y << std::endl; // For debug
//	std::cout << "temp1 : " << ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X ) << " "  <<  ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y ) << " " << temp1 << std::endl; // For debug
//#endif

	if( fabs( temp1 ) < 1.0e-12 ){
		std::cerr << " Error : Divide by zero in calculating X coordinate of intersection point of two lines !!" << std::endl;
		exit(1);
	}

	coordIntersectionPoint.X = ( coord1stLine2.Y - coord1stLine1.Y )*( coord2ndLine2.X - coord2ndLine1.X )*coord1stLine1.X
							 - ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.Y - coord2ndLine1.Y )*coord2ndLine1.X  
							 + ( coord1stLine2.X - coord1stLine1.X )*( coord2ndLine2.X - coord2ndLine1.X )*( coord2ndLine1.Y - coord1stLine1.Y );
	coordIntersectionPoint.X /= temp1;

	const double temp2 = coord1stLine2.X - coord1stLine1.X;
	const double temp3 = coord2ndLine2.X - coord2ndLine1.X;

	if( fabs( temp2 ) < 1.0e-8 && fabs( temp3 ) < 1.0e-8 ){
		std::cerr << " Error : Divide by zero in calculating Y coordinate of intersection point of two lines !!" << std::endl;
		exit(1);
	}

	if( fabs( temp2 ) > fabs( temp3 ) ){
		coordIntersectionPoint.Y = ( coord1stLine2.Y - coord1stLine1.Y )/temp2*( coordIntersectionPoint.X - coord1stLine1.X ) + coord1stLine1.Y;
	}else{
		coordIntersectionPoint.Y = ( coord2ndLine2.Y - coord2ndLine1.Y )/temp3*( coordIntersectionPoint.X - coord2ndLine1.X ) + coord2ndLine1.Y;
	}

	return;

}

// Function determine if then inputed point locate at the left of the segment on the land surface
bool MeshDataTetraElement::locateLeftOfSegmentOnLandSurface( const CommonParameters::locationXY& point, 
	const CommonParameters::locationXY& startPointOfSegment, const CommonParameters::locationXY& endPointOfSegment ) const{

	if( ( endPointOfSegment.Y - startPointOfSegment.Y )*( point.X - startPointOfSegment.X ) >= ( endPointOfSegment.X - startPointOfSegment.X )*( point.Y - startPointOfSegment.Y ) ){
		return true;
	}

	return false;

}

// Function determine if then inputed point locate at the left of the segment on the Y-Z plane of boundary
bool MeshDataTetraElement::locateLeftOfSegmentOnYZPlaneOfBoundary( const int iPlane, const int iElem, const int iEdge, const CommonParameters::locationYZ& point ) const{

	//if( iPlane != MeshData::YZMinus && iPlane != MeshData::YZPlus ){
	//	OutputFiles::m_logFile << " Error : iPlane is wrong !! iPlane = " << iPlane << std::endl;
	//	exit(1);
	//}
	assert( iPlane == MeshData::YZMinus || iPlane == MeshData::YZPlus );

	const int nodeID0 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 1 );

	const CommonParameters::locationYZ startPointOfSegment = { getYCoordinatesOfNodes( nodeID0 ), getZCoordinatesOfNodes( nodeID0 ) };
	const CommonParameters::locationYZ endPointOfSegment   = { getYCoordinatesOfNodes( nodeID1 ), getZCoordinatesOfNodes( nodeID1 ) };

	if( ( endPointOfSegment.Z - startPointOfSegment.Z )*( point.Y - startPointOfSegment.Y ) <= ( endPointOfSegment.Y - startPointOfSegment.Y )*( point.Z - startPointOfSegment.Z ) ){
		return true;
	}

	return false;

}

// Function determine if then inputed point locate at the left of the segment on the Z-Y plane of boundary
bool MeshDataTetraElement::locateLeftOfSegmentOnZXPlaneOfBoundary( const int iPlane, const int iElem, const int iEdge, const CommonParameters::locationZX& point ) const{

	//if( iPlane != MeshData::ZXMinus && iPlane != MeshData::ZXPlus ){
	//	OutputFiles::m_logFile << " Error : iPlane is wrong !! iPlane = " << iPlane << std::endl;
	//	exit(1);
	//}
	assert( iPlane == MeshData::ZXMinus || iPlane == MeshData::ZXPlus );

	const int nodeID0 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 1 );

	const CommonParameters::locationZX startPointOfSegment = { getZCoordinatesOfNodes( nodeID0 ), getXCoordinatesOfNodes( nodeID0 ) };
	const CommonParameters::locationZX endPointOfSegment   = { getZCoordinatesOfNodes( nodeID1 ), getXCoordinatesOfNodes( nodeID1 ) };

	if( ( endPointOfSegment.X - startPointOfSegment.X )*( point.Z - startPointOfSegment.Z ) <= ( endPointOfSegment.Z - startPointOfSegment.Z )*( point.X - startPointOfSegment.X ) ){
		return true;
	}

	return false;

}

// Calculate volume of tetrahedron
double MeshDataTetraElement::calcVolume( const CommonParameters::locationXYZ& point1, const CommonParameters::locationXYZ& point2,
	const CommonParameters::locationXYZ& point3, const CommonParameters::locationXYZ& point4 ) const{

	const double val = ( point2.X*point3.Y* point4.Z + point2.Y*point3.Z*point4.X + point2.Z*point3.X*point4.Y - point2.Z*point3.Y*point4.X - point2.X*point3.Z*point4.Y - point2.Y*point3.X*point4.Z )
		             - ( point1.X*point3.Y* point4.Z + point1.Y*point3.Z*point4.X + point1.Z*point3.X*point4.Y - point1.Z*point3.Y*point4.X - point1.X*point3.Z*point4.Y - point1.Y*point3.X*point4.Z )
		             + ( point1.X*point2.Y* point4.Z + point1.Y*point2.Z*point4.X + point1.Z*point2.X*point4.Y - point1.Z*point2.Y*point4.X - point1.X*point2.Z*point4.Y - point1.Y*point2.X*point4.Z )
		             - ( point1.X*point2.Y* point3.Z + point1.Y*point2.Z*point3.X + point1.Z*point2.X*point3.Y - point1.Z*point2.Y*point3.X - point1.X*point2.Z*point3.Y - point1.Y*point2.X*point3.Z );

	return val / 6.0;
}

// Calculate are of triangle
double MeshDataTetraElement::calcArea(  const CommonParameters::CoordPair& point1, const CommonParameters::CoordPair& point2, const CommonParameters::CoordPair& point3  ) const{

	return 0.5 * fabs( ( point2.first - point1.first ) * ( point3.second - point1.second ) - ( point2.second - point1.second ) * ( point3.first - point1.first ) ); 

}

// Calculate length of edges of elements
double MeshDataTetraElement::calcEdgeLengthFromElementAndEdge( const int iElem, const int iEdge ) const{

	return caldDistanceOfTwoNodes( getNodeIDGlobalFromElementAndEdge( iElem, iEdge, 0 ), getNodeIDGlobalFromElementAndEdge( iElem, iEdge, 1 ) );

}

// Calculate length of edges of elements on boundary planes
double MeshDataTetraElement::calcEdgeLengthFromElementAndEdgeBoundaryPlanes( const int iPlane, const int iElem, const int iEdge ) const{

	const int nodeID0 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 1 );

	const double coordZ0 = getZCoordinatesOfNodes( nodeID0 );
	const double coordZ1 = getZCoordinatesOfNodes( nodeID1 );

	if( iPlane == MeshData::ZXMinus || iPlane == MeshData::ZXPlus ){// Z-X plane
		const double coordX0 = getXCoordinatesOfNodes( nodeID0 );
		const double coordX1 = getXCoordinatesOfNodes( nodeID1 );

#ifdef _DEBUG_WRITE
		std::cout << "coordX0 coordZ0 : " << coordX0 << " " << coordZ0 << std::endl;
		std::cout << "coordX1 coordZ1 : " << coordX1 << " " << coordZ1 << std::endl;
#endif

		return hypot( coordX1 - coordX0, coordZ1 - coordZ0 ); 

	}else if( iPlane == MeshData::YZMinus || iPlane == MeshData::YZPlus ){// Y-Z plane
		const double coordY0 = getYCoordinatesOfNodes( nodeID0 );
		const double coordY1 = getYCoordinatesOfNodes( nodeID1 );

#ifdef _DEBUG_WRITE
		std::cout << "coordY0 coordZ0 : " << coordY0 << " " << coordZ0 << std::endl;
		std::cout << "coordY1 coordZ1 : " << coordY1 << " " << coordZ1 << std::endl;
#endif

		return hypot( coordY1 - coordY0, coordZ1 - coordZ0 );
	}

	std::cerr << "Error : Wrong plane ID !! : iPlane = " << iPlane << std::endl;
	exit(1);

	return -1;

}

//// Calculate length of edges of element face
//double MeshDataTetraElement::calcEdgeLengthOnElementFace( const int iElem, const int iFace, const int iEdge ) const{
//
//	return calcEdgeLengthFromElementAndEdge( iElem, getEdgeIDLocalFromFaceIDLocal( iFace, iEdge ) );
//
//}

// Calculate horizontal coordinate differences of edges of the elements on boundary planes
double MeshDataTetraElement::calcHorizontalCoordDifferenceBoundaryPlanes( const int iPlane, const int iElem, const int iEdge ) const{

	const int nodeID0 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( iPlane, iElem, iEdge, 1 );

	if( iPlane == MeshData::ZXMinus || iPlane == MeshData::ZXPlus ){// Z-X plane

		return getXCoordinatesOfNodes( nodeID1 ) - getXCoordinatesOfNodes( nodeID0 ); 

	}else if( iPlane == MeshData::YZMinus || iPlane == MeshData::YZPlus ){// Y-Z plane

		return getYCoordinatesOfNodes( nodeID1 ) - getYCoordinatesOfNodes( nodeID0 ); 

	}

	std::cerr << "Error : Wrong plane ID !! : iPlane = " << iPlane << std::endl;
	exit(1);

	return -1;

}

// Calculate X coordinate of points on element face
double MeshDataTetraElement::calcXCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const{

	//if( iElem < 0 || iElem >= m_numElemTotal ){
	//	OutputFiles::m_logFile << " Error : iElem is out of range. iElem = " << iElem << std::endl;
	//	exit(1);
	//}

	//if( iFace < 0 || iFace >= 4 ){
	//	OutputFiles::m_logFile << " Error : iFace is out of range. iFace = " << iFace << std::endl;
	//	exit(1);
	//}
	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iFace >= 0 );
	assert( iFace < 4 );

	double val(0.0);

	val += getXCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][0] ) ) * areaCoord.coord0; 
	val += getXCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][1] ) ) * areaCoord.coord1; 
	val += getXCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][2] ) ) * areaCoord.coord2; 

	return val;

}

// Calculate Y coordinate of points on element face
double MeshDataTetraElement::calcYCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const{

	//if( iElem < 0 || iElem >= m_numElemTotal ){
	//	OutputFiles::m_logFile << " Error : iElem is out of range. iElem = " << iElem << std::endl;
	//	exit(1);
	//}

	//if( iFace < 0 || iFace >= 4 ){
	//	OutputFiles::m_logFile << " Error : iFace is out of range. iFace = " << iFace << std::endl;
	//	exit(1);
	//}
	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iFace >= 0 );
	assert( iFace < 4 );

	double val(0.0);

	val += getYCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][0] ) ) * areaCoord.coord0; 
	val += getYCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][1] ) ) * areaCoord.coord1; 
	val += getYCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][2] ) ) * areaCoord.coord2; 

	return val;

}

// Calculate Z coordinate of points on element face
double MeshDataTetraElement::calcZCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const{

	//if( iElem < 0 || iElem >= m_numElemTotal ){
	//	OutputFiles::m_logFile << " Error : iElem is out of range. iElem = " << iElem << std::endl;
	//	exit(1);
	//}

	//if( iFace < 0 || iFace >= 4 ){
	//	OutputFiles::m_logFile << " Error : iFace is out of range. iFace = " << iFace << std::endl;
	//	exit(1);
	//}
	assert( iElem >= 0 );
	assert( iElem < m_numElemTotal );
	assert( iFace >= 0 );
	assert( iFace < 4 );

	double val(0.0);

	val += getZCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][0] ) ) * areaCoord.coord0; 
	val += getZCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][1] ) ) * areaCoord.coord1; 
	val += getZCoordinatesOfNodes( getNodesOfElements( iElem, m_faceID2NodeID[iFace][2] ) ) * areaCoord.coord2; 

	return val;

}

// Calculate volume of tetrahedral element
double MeshDataTetraElement::calcVolume( const int elemID ) const{

	assert( elemID >= 0 );
	assert( elemID < m_numElemTotal );

	CommonParameters::locationXYZ nodeCoord[4];
	for( int i = 0; i < 4; ++i ){
		const int nodeID = getNodesOfElements( elemID, i ); 
		nodeCoord[i].X = getXCoordinatesOfNodes( nodeID );
		nodeCoord[i].Y = getYCoordinatesOfNodes( nodeID );
		nodeCoord[i].Z = getZCoordinatesOfNodes( nodeID );
	}

	return calcVolume( nodeCoord[0], nodeCoord[1], nodeCoord[2], nodeCoord[3] );
}

// Calculate volume coordinates of point
void MeshDataTetraElement::calcVolumeCoordsOfPoint( const int elemID, const CommonParameters::locationXYZ& pointCoord, CommonParameters::VolumeCoords& coords ) const{

	//if( elemID >= m_numElemTotal || elemID < 0 ){
	//	OutputFiles::m_logFile << "Error : ID of element is out of range !! : elemID = " << elemID << std::endl;
	//	exit(1);
	//}
	assert( elemID >= 0 );
	assert( elemID < m_numElemTotal );

	CommonParameters::locationXYZ nodeCoord[4];
	for( int i = 0; i < 4; ++i ){
		const int nodeID = getNodesOfElements( elemID, i ); 
		nodeCoord[i].X = getXCoordinatesOfNodes( nodeID );
		nodeCoord[i].Y = getYCoordinatesOfNodes( nodeID );
		nodeCoord[i].Z = getZCoordinatesOfNodes( nodeID );
	}

	const double volTotal = calcVolume( nodeCoord[0], nodeCoord[1], nodeCoord[2], nodeCoord[3] );

	coords.coord0 = calcVolume(   pointCoord, nodeCoord[1], nodeCoord[2], nodeCoord[3] ) / volTotal;
	coords.coord1 = calcVolume( nodeCoord[0],   pointCoord, nodeCoord[2], nodeCoord[3] ) / volTotal;
	coords.coord2 = calcVolume( nodeCoord[0], nodeCoord[1],   pointCoord, nodeCoord[3] ) / volTotal;
	coords.coord3 = calcVolume( nodeCoord[0], nodeCoord[1], nodeCoord[2],   pointCoord ) / volTotal;

	if( coords.coord0 < - m_eps || coords.coord1 < - m_eps || coords.coord2 < - m_eps || coords.coord3 < - m_eps ){
		std::cerr << " Error : Volume coordinate of element " << elemID << " is negative for the point (X,Y,Z) = ( " << pointCoord.X << ", " << pointCoord.Y << ", " << pointCoord.Z << " ) ." << std::endl;
		exit(1);
	}

}

// Calculate area coordinates of point on the land surface
void MeshDataTetraElement::calcAreaCoordsOfPointOnLandSurface( const int elemID, const int faceID, const CommonParameters::locationXY& pointCoord, CommonParameters::AreaCoords& coords ) const{

	//if( elemID >= m_numElemTotal || elemID < 0 ){
	//	OutputFiles::m_logFile << "Error : ID of element is out of range !! : elemID = " << elemID << std::endl;
	//	exit(1);
	//}

	//if( faceID < 0 || faceID >= 4 ){
	//	OutputFiles::m_logFile << "Error : ID of face is out of range !! : faceID = " << faceID << std::endl;
	//	exit(1);
	//}
	assert( elemID >= 0 );
	assert( elemID < m_numElemTotal );
	assert( faceID >= 0 );
	assert( faceID < 4 );

	CommonParameters::CoordPair nodeCoord[3];
	for( int i = 0; i < 3; ++i ){
		const int nodeID = getNodesOfElements( elemID, m_faceID2NodeID[faceID][i] );
		nodeCoord[i].first  = getXCoordinatesOfNodes( nodeID );
		nodeCoord[i].second = getYCoordinatesOfNodes( nodeID );
	}

	const CommonParameters::CoordPair point = { pointCoord.X, pointCoord.Y };

	const double areaTotal = calcArea( nodeCoord[0], nodeCoord[1], nodeCoord[2] );

	coords.coord0 = calcArea( point, nodeCoord[1], nodeCoord[2] ) / areaTotal;
	coords.coord1 = calcArea( nodeCoord[0], point, nodeCoord[2] ) / areaTotal;
	coords.coord2 = calcArea( nodeCoord[0], nodeCoord[1], point ) / areaTotal;

	if( coords.coord0 < - m_eps || coords.coord1 < - m_eps || coords.coord2 < - m_eps ){
		std::cerr << " Error : Area coordinate of face " << faceID << " of element " << elemID << " is negative for the point (X,Y) = ( " << pointCoord.X << ", " << pointCoord.Y << " ) ." << std::endl;
		exit(1);
	}

}

// Calculate area coordinates of the specified point on the Y-Z plane of boundary
void MeshDataTetraElement::calcAreaCoordsOfPointOnYZPlaneOfBoundary( const int iPlane, const int iElem, const CommonParameters::CoordPair& point, CommonParameters::AreaCoords& coords ) const{

	//if( iPlane != MeshData::YZMinus && iPlane != MeshData::YZPlus ){
	//	OutputFiles::m_logFile << " Error : iPlane is wrong !! iPlane = " << iPlane << std::endl;
	//	exit(1);
	//}
	assert( iPlane == MeshData::YZMinus || iPlane == MeshData::YZPlus );

	const int nodeID0 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 1 );
	const int nodeID2 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 2 );

	const CommonParameters::CoordPair nodeCoord0 = { getYCoordinatesOfNodes(nodeID0), getZCoordinatesOfNodes(nodeID0) };
	const CommonParameters::CoordPair nodeCoord1 = { getYCoordinatesOfNodes(nodeID1), getZCoordinatesOfNodes(nodeID1) };
	const CommonParameters::CoordPair nodeCoord2 = { getYCoordinatesOfNodes(nodeID2), getZCoordinatesOfNodes(nodeID2) };

	const double areaTotal = calcArea( nodeCoord0, nodeCoord1, nodeCoord2 );

	coords.coord0 = calcArea( point, nodeCoord1, nodeCoord2 ) / areaTotal;
	coords.coord1 = calcArea( nodeCoord0, point, nodeCoord2 ) / areaTotal;
	coords.coord2 = calcArea( nodeCoord0, nodeCoord1, point ) / areaTotal;

	if( coords.coord0 < - m_eps || coords.coord1 < - m_eps || coords.coord2 < - m_eps ){
		std::cerr << " Error : Area coordinate is negative for the point (Y,Z) = ( " << point.first << ", " << point.second << " ) ." << std::endl;
		exit(1);
	}

}

// Calculate area coordinates of the specified point on the Z-X plane of boundary
void MeshDataTetraElement::calcAreaCoordsOfPointOnZXPlaneOfBoundary( const int iPlane, const int iElem, const CommonParameters::CoordPair& point, CommonParameters::AreaCoords& coords ) const{

	//if( iPlane != MeshData::ZXMinus && iPlane != MeshData::ZXPlus ){
	//	OutputFiles::m_logFile << " Error : iPlane is wrong !! iPlane = " << iPlane << std::endl;
	//	exit(1);
	//}
	assert( iPlane == MeshData::ZXMinus || iPlane == MeshData::ZXPlus );

	const int nodeID0 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 0 );
	const int nodeID1 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 1 );
	const int nodeID2 = getNodeIDGlobalFromElementBoundaryPlanes( iPlane, iElem, 2 );

	const CommonParameters::CoordPair nodeCoord0 = { getZCoordinatesOfNodes(nodeID0), getXCoordinatesOfNodes(nodeID0) };
	const CommonParameters::CoordPair nodeCoord1 = { getZCoordinatesOfNodes(nodeID1), getXCoordinatesOfNodes(nodeID1) };
	const CommonParameters::CoordPair nodeCoord2 = { getZCoordinatesOfNodes(nodeID2), getXCoordinatesOfNodes(nodeID2) };

	const double areaTotal = calcArea( nodeCoord0, nodeCoord1, nodeCoord2 );

	coords.coord0 = calcArea( point, nodeCoord1, nodeCoord2 ) / areaTotal;
	coords.coord1 = calcArea( nodeCoord0, point, nodeCoord2 ) / areaTotal;
	coords.coord2 = calcArea( nodeCoord0, nodeCoord1, point ) / areaTotal;

	if( coords.coord0 < - m_eps || coords.coord1 < - m_eps || coords.coord2 < - m_eps ){
		std::cerr << " Error : Area coordinate is negative for the point (Z,X) = ( " << point.first << ", " << point.second << " ) ." << std::endl;
		exit(1);
	}

}

//// Calculate are of triangle projected on the X-Y plane
//double MeshDataTetraElement::calcAreaOfTriangleOnXYPlane( const CommonParameters::locationXY& vertex1, const CommonParameters::locationXY& vertex2,
//	const CommonParameters::locationXY& vertex3 ) const{
//
//	return 0.5 * fabs( ( vertex2.X - vertex1.X )*( vertex3.Y - vertex1.Y ) - ( vertex2.Y - vertex1.Y )*( vertex3.X - vertex1.X ) );
//
//}

//// Calulate normal vector of element face
//CommonParameters::Vector3D MeshDataTetraElement::calulateNormalVectorOfElementFace( const int elemID, const int faceID ) const{
//
//	if( elemID >= m_numElemTotal || elemID < 0 ){
//		OutputFiles::m_logFile << "Error : ID of element is out of range !! : elemID = " << elemID << std::endl;
//		exit(1);
//	}
//
//	const int nodeID0 = getNodesOfElements( elemID, m_faceID2NodeID[faceID][0] );
//	const int nodeID1 = getNodesOfElements( elemID, m_faceID2NodeID[faceID][1] );
//	const int nodeID2 = getNodesOfElements( elemID, m_faceID2NodeID[faceID][2] );
//
//	const CommonParameters::locationXYZ nodeCoord0 = { getXCoordinatesOfNodes( nodeID0 ), getYCoordinatesOfNodes( nodeID0 ), getZCoordinatesOfNodes( nodeID0 ) };
//	const CommonParameters::locationXYZ nodeCoord1 = { getXCoordinatesOfNodes( nodeID1 ), getYCoordinatesOfNodes( nodeID1 ), getZCoordinatesOfNodes( nodeID1 ) };
//	const CommonParameters::locationXYZ nodeCoord2 = { getXCoordinatesOfNodes( nodeID2 ), getYCoordinatesOfNodes( nodeID2 ), getZCoordinatesOfNodes( nodeID2 ) };
//
//	const CommonParameters::Vector3D vec1 = { nodeCoord1.X - nodeCoord0.X, nodeCoord1.Y - nodeCoord0.Y, nodeCoord1.Z - nodeCoord0.Z };
//	const CommonParameters::Vector3D vec2 = { nodeCoord2.X - nodeCoord1.X, nodeCoord2.Y - nodeCoord1.Y, nodeCoord2.Z - nodeCoord1.Z };
//
//	return calcOuterProduct( vec1, vec2 );
//
//}

// Decide whether specified point locate inside of face
//bool MeshDataTetraElement::locateInsideOfFace( const int elemID, const int faceID, const double locX, const double locY, const double locZ ) const{
bool MeshDataTetraElement::locateInsideOfFace( const int elemID, const int faceID, const CommonParameters::locationXYZ& loc ) const{

	//if( elemID >= m_numElemTotal || elemID < 0 ){
	//	OutputFiles::m_logFile << "Error : ID of element is out of range !! : elemID = " << elemID << std::endl;
	//	exit(1);
	//}
	assert( elemID >= 0 );
	assert( elemID < m_numElemTotal );

	CommonParameters::locationXYZ nodeCoord[3];

	for( int i = 0; i < 3; ++i ){
		const int nodeID = getNodesOfElements( elemID, m_faceID2NodeID[faceID][i] );
		nodeCoord[i].X =getXCoordinatesOfNodes( nodeID );
		nodeCoord[i].Y =getYCoordinatesOfNodes( nodeID );
		nodeCoord[i].Z =getZCoordinatesOfNodes( nodeID );
	}

	//const CommonParameters::Vector3D vec1 = { nodeCoord1.X - nodeCoord0.X, nodeCoord1.Y - nodeCoord0.Y, nodeCoord1.Z - nodeCoord0.Z };
	//const CommonParameters::Vector3D vec2 = { nodeCoord2.X - nodeCoord1.X, nodeCoord2.Y - nodeCoord1.Y, nodeCoord2.Z - nodeCoord1.Z };
	//const CommonParameters::Vector3D vec3 = { loc.X - nodeCoord0.X, loc.Y - nodeCoord0.Y, locZ - nodeCoord0.Z };

	const CommonParameters::Vector3D vec1 = calcVector3D( nodeCoord[0], nodeCoord[1] );
	const CommonParameters::Vector3D vec2 = calcVector3D( nodeCoord[1], nodeCoord[2] );
	const CommonParameters::Vector3D vec3 = calcVector3D( nodeCoord[0], loc );

	if( calcInnerProduct( calcOuterProduct( vec1, vec2 ), vec3 ) <= 0.0 ){
		return true;
	}

	return false;

}

//// Decide whether specified point is included in element
//bool MeshDataTetraElement::includePointInElement( const int elemID, const double locX, const double locY, const double locZ ) const{
//
//	for( int i = 0; i < 4; ++i ){
//		if( !locateInsideOfFace( elemID, i, locX, locY, locZ ) ){
//			return false;
//		}
//	}
//
//	return true;
//
//}
