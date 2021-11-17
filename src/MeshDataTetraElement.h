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
#ifndef DBLDEF_MESHDATA_TETRA_ELEMENT
#define DBLDEF_MESHDATA_TETRA_ELEMENT

#include <vector>
#include "MeshData.h"
#include "CommonParameters.h"

// Class of FEM mesh for tetrahedral element
class MeshDataTetraElement : public MeshData {

public:

	// Constructer
	MeshDataTetraElement();

	// Destructer
	virtual ~MeshDataTetraElement();

	// Input mesh data from "mesh.dat"
	virtual void inputMeshData();

	// Get local face ID of elements belonging to the boundary planes
	int getFaceIDLocalFromElementBoundaryPlanes( const int iPlane, const int iElem ) const;

	// Get local node ID  from local face ID
	int getNodeIDLocalFromFaceIDLocal( const int iFace, const int num ) const;

	// Get local node ID from local edge ID
	int getNodeIDLocalFromEdgeIDLocal( const int iEdge, const int num ) const;

	// Get global node ID of specified element and edge
	int getNodeIDGlobalFromElementAndEdge( const int iElem, const int iEdge, const int num ) const;

	// Get global node ID of specified element and face
	int getNodeIDGlobalFromElementAndFace( const int iElem, const int iFace, const int num ) const;

	// Get global node ID of specified element belonging to the boundary planes  
	int getNodeIDGlobalFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const;

	// Get X coordinate of node of specified element belonging to the boundary planes  
	double getCoordXFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const;

	// Get Y coordinate of node of specified element belonging to the boundary planes  
	double getCoordYFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const;

	// Get Z coordinate of node of specified element belonging to the boundary planes  
	double getCoordZFromElementBoundaryPlanes( const int iPlane, const int iElem, const int num ) const;

	// Get global node ID from ID of element belonging to the boundary planes and its edge ID
	// [note] : node ID is outputed as they make a clockwise turn around +X or +Y direction
	int getNodeIDGlobalFromElementAndEdgeBoundaryPlanes( const int iPlane, const int iElem, const int iEdge, const int num ) const;

	// Get local edge ID from local face ID
	int getEdgeIDLocalFromFaceIDLocal( const int iFace, const int num ) const;

	// Calculate length of edges of elements
	double calcEdgeLengthFromElementAndEdge( const int iElem, const int iEdge ) const;

	// Calculate length of edges of elements on boundary planes
	double calcEdgeLengthFromElementAndEdgeBoundaryPlanes( const int iPlane, const int iElem, const int iEdge ) const;

	//// Calculate length of edges of element face
	//double calcEdgeLengthOnElementFace( const int iElem, const int iFace, const int iEdge ) const;

	// Calculate horizontal coordinate differences of edges of the elements on boundary planes
	double calcHorizontalCoordDifferenceBoundaryPlanes( const int iPlane, const int iElem, const int iEdge ) const;

	// Calculate X coordinate of points on element face
	double calcXCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const;

	// Calculate Y coordinate of points on element face
	double calcYCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const;

	// Calculate Z coordinate of points on element face
	double calcZCoordOfPointOnFace( const int iElem, const int iFace, const CommonParameters::AreaCoords& areaCoord ) const;

	// Calculate volume of tetrahedral element
	double calcVolume( const int iElem ) const;

private:

	// Copy constructer
	MeshDataTetraElement(const MeshDataTetraElement& rhs);

	// Copy assignment operator
	MeshDataTetraElement& operator=(const MeshDataTetraElement& rhs);

	static const double m_eps;

	// Array of faces of elements belonging to the boundary planes
	//   m_facesOfElementsBoundaryPlanes[0] : Y-Z Plane ( Minus Side )
	//   m_facesOfElementsBoundaryPlanes[1] : Y-Z Plane ( Plus Side  )
	//   m_facesOfElementsBoundaryPlanes[2] : Z-X Plane ( Minus Side )
	//   m_facesOfElementsBoundaryPlanes[3] : Z-X Plane ( Plus Side  )
	//   m_facesOfElementsBoundaryPlanes[4] : X-Y Plane ( Minus Side )
	//   m_facesOfElementsBoundaryPlanes[5] : X-Y Plane ( Plus Side  )
	int* m_facesOfElementsBoundaryPlanes[6];

	// Number of elements belonging to the land surface
	int m_numElemOnLandSurface;

	// Array of elements belonging to the land surface
	int* m_elemOnLandSurface;

	// Array of faces belonging to the land surface
	int* m_faceLandSurface;

	// Array converting from face ID to node ID
	int m_faceID2NodeID[4][3];

	// Array converting from face ID to edge ID
	int m_faceID2EdgeID[4][3];

	// Array converting from edge ID to node ID
	int m_edgeID2NodeID[6][2];

	// Function determine if two segments intersect or not
	bool intersectTwoSegments( const CommonParameters::locationXY& startPointOf1stSegment, const CommonParameters::locationXY& endPointOf1stSegment,
		const CommonParameters::locationXY& startPointOf2ndSegment, const CommonParameters::locationXY& endPointOf2ndSegment ) const;

	// Function determine if two lines overlap or not
	bool overlapTwoLines( const CommonParameters::locationXY& coord1stLine1, const CommonParameters::locationXY& coord1stLine2,
		const CommonParameters::locationXY& coord2ndLine1, const CommonParameters::locationXY& coord2ndLine2 ) const;

	// Function determine if two segments overlap or not
	bool overlapTwoSegments( const CommonParameters::locationXY& startPointOf1stSegment, const CommonParameters::locationXY& endPointOf1stSegment,
		const CommonParameters::locationXY& startPointOf2ndSegment, const CommonParameters::locationXY& endPointOf2ndSegment ) const;

	// Calculate inner product of two vectors
	double calcInnerProduct2D( const CommonParameters::locationXY& startCoordOf1stVec, const CommonParameters::locationXY& endCoordOf1stVec,
		const CommonParameters::locationXY& startCoordOf2ndVec, const CommonParameters::locationXY& endCoordOf2ndVec) const;

	// Calculate coordinates of intersection point of two lines
	void calcCoordOfIntersectionPointOfTwoLines( const CommonParameters::locationXY& coord1stLine1, const CommonParameters::locationXY& coord1stLine2,
		const CommonParameters::locationXY& coord2ndLine1, const CommonParameters::locationXY& coord2ndLine2, CommonParameters::locationXY& coordIntersectionPoint) const;

	// Function determine if then inputed point locate at the left of the segment on the land surface
	//bool locateLeftOfSegmentOnXYPlane( const CommonParameters::locationXY& point, 
	//	const CommonParameters::locationXY& startPointOfSegment, const CommonParameters::locationXY& endPointOfSegment ) const;
	bool locateLeftOfSegmentOnLandSurface( const CommonParameters::locationXY& point, 
		const CommonParameters::locationXY& startPointOfSegment, const CommonParameters::locationXY& endPointOfSegment ) const;

	// Function determine if then inputed point locate at the left of the segment on the Y-Z plane of boundary
	//bool locateLeftOfSegmentOnYZPlaneOfBoundary( const CommonParameters::locationYZ& point,
	//	const CommonParameters::locationYZ& startPointOfSegment, const CommonParameters::locationYZ& endPointOfSegment ) const;
	bool locateLeftOfSegmentOnYZPlaneOfBoundary( const int iPlane, const int iElem, const int iEdge, const CommonParameters::locationYZ& point ) const;

	// Function determine if then inputed point locate at the left of the segment on the Z-X plane of boundary
	//bool locateLeftOfSegmentOnZXPlaneOfBoundary( const CommonParameters::locationZX& point, 
	//	const CommonParameters::locationZX& startPointOfSegment, const CommonParameters::locationZX& endPointOfSegment ) const;
	bool locateLeftOfSegmentOnZXPlaneOfBoundary( const int iPlane, const int iElem, const int iEdge, const CommonParameters::locationZX& point ) const;

	// Calculate volume of tetrahedron
	double calcVolume( const CommonParameters::locationXYZ& point1, const CommonParameters::locationXYZ& point2,
		const CommonParameters::locationXYZ& point3, const CommonParameters::locationXYZ& point4 ) const;

	// Calculate volume coordinates of point on the land surface
	//void calcVolumeCoordsOfPointOnLandSurface( const int elemID, const CommonParameters::locationXY& pointCoord, CommonParameters::VolumeCoords& coords ) const;
	void calcVolumeCoordsOfPointOnLandSurface( const int elemID, const int faceID, const CommonParameters::locationXY& pointCoord, CommonParameters::VolumeCoords& coords ) const;

	//// Calculate volume coordinates of point on YZ plane
	//void calcVolumeCoordsOfPointOnYZPlane( const int elemID, const int faceID, const CommonParameters::locationYZ& pointCoord, CommonParameters::VolumeCoords& coords ) const;

	//// Calculate volume coordinates of point on ZX plane
	//void calcVolumeCoordsOfPointOnZXPlane( const int elemID, const int faceID, const CommonParameters::locationZX& pointCoord, CommonParameters::VolumeCoords& coords ) const;

	// Calculate volume coordinates of the nputed point
	void calcVolumeCoordsOfPoint( const int elemID, const CommonParameters::locationXYZ& pointCoord, CommonParameters::VolumeCoords& coords ) const;

	// Calculate area coordinates of point on the land surface
	void calcAreaCoordsOfPointOnLandSurface( const int elemID, const int faceID, const CommonParameters::locationXY& pointCoord, CommonParameters::AreaCoords& coords ) const;

	// Calculate area of triangle from two dimensinal coordinates
	double calcArea( const CommonParameters::CoordPair& point1, const CommonParameters::CoordPair& point2, const CommonParameters::CoordPair& point3 ) const;

	// Calculate area coordinates of the specified point on the Y-Z plane of boundary
	void calcAreaCoordsOfPointOnYZPlaneOfBoundary( const int iPlane, const int iElem, const CommonParameters::CoordPair& point, CommonParameters::AreaCoords& coords ) const;

	// Calculate area coordinates of the specified point on the Z-X plane of boundary
	void calcAreaCoordsOfPointOnZXPlaneOfBoundary( const int iPlane, const int iElem, const CommonParameters::CoordPair& point, CommonParameters::AreaCoords& coords ) const;

	// Decide whether specified point locate inside of face
	bool locateInsideOfFace( const int elemID, const int faceID, const CommonParameters::locationXYZ& loc ) const;


};

#endif
