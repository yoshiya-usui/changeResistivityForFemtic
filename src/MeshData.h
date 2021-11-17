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
#ifndef DBLDEF_MESHDATA
#define DBLDEF_MESHDATA

#include <vector>
#include "CommonParameters.h"

// Class of FEM mesh for brick element
class MeshData{

public:

	enum BoundaryPlanes{
		YZMinus = 0,
		YZPlus,
		ZXMinus,
		ZXPlus,
		XYMinus,
		XYPlus,
	};

	enum MeshType{
		HEXA = 0,
		TETRA,
	};

	struct coordinateValue{
		double X;
		double Y;
		double Z;
	};

	struct coordinateValueXY{
		double X;
		double Y;
	};

	// Constructer
	MeshData();

	// Destructer
	virtual ~MeshData();

	// Input mesh data from "mesh.dat"
	virtual void inputMeshData() = 0;

	// Get tolal number of elements
	int getNumElemTotal() const;

	// Get total number of elements belonging to the boundary planes
	int getNumElemOnBoundaryPlanes( const int iPlane ) const;

	// Get X coordinates of node
	double getXCoordinatesOfNodes( const int iNode ) const;

	// Get Y coordinates of node
	double getYCoordinatesOfNodes( const int iNode ) const;

	// Get Z coordinates of node
	double getZCoordinatesOfNodes( const int iNode ) const;

	// Get ID of the Node composing specified element
	int getNodesOfElements( const int iElem, const int iNode ) const;
	
	// Get ID of the element belonging to the boundary planes
	int getElemBoundaryPlanes( const int iPlane, const int iElem ) const;

	// Get ID of neighbor Elements
	int getIDOfNeighborElement( const int iElem, const int num ) const;

	// Get number of neighbor elements of one element
	int getNumNeighborElement() const;

	// Calculate distance of two nodes
	double caldDistanceOfTwoNodes( const int nodeID0,  const int nodeID1 ) const;

	// Calculate distance of two nodes along X direction
	double caldDiffXOfTwoNodes( const int nodeID0,  const int nodeID1 ) const;

	// Calculate distance of two nodes along Y direction
	double caldDiffYOfTwoNodes( const int nodeID0,  const int nodeID1 ) const;

	// Calculate distance of two nodes along Z direction
	double caldDiffZOfTwoNodes( const int nodeID0,  const int nodeID1 ) const;

	// Decide whether specified elements share same nodes
	bool shareSameNodes( const int elemID1, const int elemID2 ) const;

	// Calculate coordinate of the center of a specified element
	CommonParameters::locationXYZ getElementCenter( const int iElem ) const;

protected:

	// Copy constructer
	MeshData(const MeshData& rhs);

	// Copy assignment operator
	MeshData& operator=(const MeshData& rhs);

	// Total number of elements
	int m_numElemTotal;

	// Total number of nodes
	int m_numNodeTotal;

	// Number of nodes belonging to one element
	int m_numNodeOneElement;

	// Number of edges belonging to one element
	int m_numEdgeOneElement;

	// Number of nodes on a face of one element
	int m_numNodeOnFaceOneElement;

	// Number of neighbor elements of one element
	int m_numNeighborElement;

	// Total number of elements belonging to the boundary planes
	int m_numElemOnBoundaryPlanes[6];

	// Array of the X coordinates of nodes
	double* m_xCoordinatesOfNodes;

	// Array of the Y coordinates of nodes
	double* m_yCoordinatesOfNodes;

	// Array of the Z coordinates of nodes
	double* m_zCoordinatesOfNodes;

	// Array of IDs of neighbor Elements
	int* m_neighborElements;

	// Array of nodes composing each element
	int* m_nodesOfElements;

	// Array of elements belonging to the boundary planes
	//   m_elemBoundaryPlane[0] : Y-Z Plane ( Minus Side )
	//   m_elemBoundaryPlane[1] : Y-Z Plane ( Plus Side  )
	//   m_elemBoundaryPlane[2] : Z-X Plane ( Minus Side )
	//   m_elemBoundaryPlane[3] : Z-X Plane ( Plus Side  )
	//   m_elemBoundaryPlane[4] : X-Y Plane ( Minus Side )
	//   m_elemBoundaryPlane[5] : X-Y Plane ( Plus Side  )
	int* m_elemBoundaryPlanes[6];

	// Calculate distanceof two points
	double calcDistance( const CommonParameters::locationXY& point0,  const CommonParameters::locationXY& point1 ) const;

};

#endif
