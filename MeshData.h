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

	// Calculate coordinate of the gravity center of a specified element
	CommonParameters::locationXYZ getGravityCenter( const int iElem ) const;

	// Calculate difference of tthe gravity centers of the specified two element
	CommonParameters::locationXYZ calDiffOfGravityCenters( const int iElem1, const int iElem2 ) const;

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
