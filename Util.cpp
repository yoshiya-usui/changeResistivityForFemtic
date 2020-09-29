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
#include "Util.h"

#include <stdio.h>
#include <math.h>
#include <vector>

// Sort elements by its key value with quick sort
// cf) Numerical Recipes in C++ Second Edition, p336-p339.
// [Input]:
//   1) numOfIDs: Number of elements to be sorted
//   2)   values: Values of each element. Elements are sorted by this values.
// [Input/Output]:
//   1)      ids: Array of elements to be sorted
void quickSort( const int numOfIDs, int* ids, const double* values ){

	const int minimumSizeOfQuickSort = 7;
	std::vector< std::pair<int, int> > stack;

	//const int numOfIDs = sizeof( ids ) / sizeof( ids[0] );
	if( ids == NULL ){
		std::cerr << "Error : ids is NULL in quickSort !!" << std::endl;
		exit(1);
	}
	if( values == NULL ){
		std::cerr << "Error : values is NULL quickSort !!" << std::endl;
		exit(1);
	}
	if( numOfIDs <= 0 ){
		std::cerr << "Error : numOfIDs is equal to or less than zero !!" << std::endl;
		exit(1);
	}
	int iLeft(0);
	int iRight(numOfIDs - 1);

	while(1){

		if( iRight - iLeft < minimumSizeOfQuickSort ){
			//----- Insertion sort >>>>>
			for( int j = iLeft + 1; j <= iRight; ++j ){
				const int moved = ids[j];
				const double valueOfElemnentMoved = values[moved];
				int i = j - 1;
				for( ; i >= iLeft; --i ){
					if( values[ids[i]] <= valueOfElemnentMoved ){
						break;
					}
					ids[i+1] = ids[i];
				}
				ids[i+1] = moved;
			}

			if( stack.empty() ){
				break;// Finish while loop
			}
			iLeft  = stack.back().first;
			iRight = stack.back().second;
			stack.pop_back();
			//----- Insertion sort <<<<<
		}else{
			//----- Quick sort >>>>>
			std::swap( ids[(iLeft+iRight)/2] , ids[iLeft+1] );// Exchange element
		    if( values[ids[iLeft]] > values[ids[iRight]] ){
				std::swap( ids[iLeft], ids[iRight] );// Exchange element
			}
			if( values[ids[iLeft+1]] > values[ids[iRight]] ){
				std::swap( ids[iLeft+1], ids[iRight] );// Exchange element
			}
			if( values[ids[iLeft]] > values[ids[iLeft+1]] ){
				std::swap( ids[iLeft], ids[iLeft+1] );// Exchange element
			}
			int i = iLeft + 1;
			int j = iRight;
			const int partitioningElement = ids[iLeft+1];// Partitioning element
			const double valueOfPartitioningElement = values[ids[iLeft+1]];// values of partitioning element

			while(1){
				++i;
				while( values[ids[i]] < valueOfPartitioningElement ){
					++i;// Scan up to find element which >= partitioning element
				}
				--j;
				while( values[ids[j]] > valueOfPartitioningElement ){
					--j;// Scan up to find element which <= partitioning element
				}
				if( j < i ){// i and j are crossed, so that partitioning complete
					break;
				}
				std::swap( ids[i], ids[j] );// Exchange element
			}

			 // Exchange IDs of j and iLeft+1 ( partitioning element )
			ids[iLeft+1] = ids[j];
			ids[j] = partitioningElement;

			if( iLeft - i + 1 >= j - iLeft ){// Elements in the right side >= Elements in the left side
				stack.push_back( std::pair<int, int>( i, iRight ) );
				iRight = j - 1;
			}else{// Elements in the right side < Elements in the left side
				stack.push_back( std::pair<int, int>( iLeft, j - 1) );
				iLeft = i;
			}
			//----- Quick sort <<<<<
		}
		
	}

	return;

}

// Sort elements by values of its three keys with quick sort
// cf) Numerical Recipes in C++ Second Edition, p336-p339.
// [Input]:
//   1) numOfIDs: Number of elements to be sorted
//   2) firstKeyValuesvalues: First key values of each element.
//   3) secondKeyValues: Second key values of each element.
//   4) thirdKeyValues: Third key values of each element.
// [Input/Output]:
//   1)      ids: Array of elements to be sorted
void quickSortThreeKeys( const int numOfIDs, int* ids,
	const double* firstKeyValues, const double* secondKeyValues, const double* thirdKeyValues ){

	const double EPSOfValue = 1e-10;
	const int minimumSizeOfQuickSort = 7;
	std::vector< std::pair<int, int> > stack;

	if( numOfIDs <= 0 ){
		std::cerr << "Error : numOfIDs is equal to or less than zero in quickSortThreeKeys !!" << std::endl;
		exit(1);
	}
	if( ids == NULL ){
		std::cerr << "Error : ids is NULL in quickSortThreeKeys !!" << std::endl;
		exit(1);
	}
	if( firstKeyValues == NULL ){
		std::cerr << "Error : firstKeyValues is NULL quickSortThreeKeys !!" << std::endl;
		exit(1);
	}
	if( secondKeyValues == NULL ){
		std::cerr << "Error : secondKeyValues is NULL quickSortThreeKeys !!" << std::endl;
		exit(1);
	}
	if( thirdKeyValues == NULL ){
		std::cerr << "Error : thirdKeyValues is NULL quickSortThreeKeys !!" << std::endl;
		exit(1);
	}
	int iLeft(0);
	int iRight(numOfIDs - 1);

	while(1){

		if( iRight - iLeft < minimumSizeOfQuickSort ){
			//----- Insertion sort >>>>>
			for( int j = iLeft + 1; j <= iRight; ++j ){
				const int moved = ids[j];
				//const double valueOfElemnentMoved = values[moved];
				int i = j - 1;
				for( ; i >= iLeft; --i ){
					//if( values[ids[i]] <= valueOfElemnentMoved ){
					//	break;
					//}
					//ids[i+1] = ids[i];
					if( compareValueByThreeKeys( ids[i], moved, firstKeyValues, secondKeyValues, thirdKeyValues) == LARGE_LEFT_HAND_SIDE ){
						ids[i+1] = ids[i];
					}else{
						break;
					}
				}
				ids[i+1] = moved;
			}

			if( stack.empty() ){
				break;// Finish while loop
			}
			iLeft  = stack.back().first;
			iRight = stack.back().second;
			stack.pop_back();
			//----- Insertion sort <<<<<
		}else{
			//----- Quick sort >>>>>
			std::swap( ids[(iLeft+iRight)/2] , ids[iLeft+1] );// Exchange element
		    //if( values[ids[iLeft]] > values[ids[iRight]] ){
			if( compareValueByThreeKeys( ids[iLeft], ids[iRight], firstKeyValues, secondKeyValues, thirdKeyValues) == LARGE_LEFT_HAND_SIDE ){
				std::swap( ids[iLeft], ids[iRight] );// Exchange element
			}
			//if( values[ids[iLeft+1]] > values[ids[iRight]] ){
			if( compareValueByThreeKeys( ids[iLeft+1], ids[iRight], firstKeyValues, secondKeyValues, thirdKeyValues) == LARGE_LEFT_HAND_SIDE ){
				std::swap( ids[iLeft+1], ids[iRight] );// Exchange element
			}
			//if( values[ids[iLeft]] > values[ids[iLeft+1]] ){
			if( compareValueByThreeKeys( ids[iLeft], ids[iLeft+1], firstKeyValues, secondKeyValues, thirdKeyValues) == LARGE_LEFT_HAND_SIDE ){
				std::swap( ids[iLeft], ids[iLeft+1] );// Exchange element
			}
			int i = iLeft + 1;
			int j = iRight;
			const int partitioningElement = ids[iLeft+1];// Partitioning element
			//const double valueOfPartitioningElement = values[ids[iLeft+1]];// values of partitioning element

			while(1){
				++i;
				//while( values[ids[i]] < valueOfPartitioningElement ){
				while( compareValueByThreeKeys( ids[i], partitioningElement, firstKeyValues, secondKeyValues, thirdKeyValues) == SMALL_LEFT_HAND_SIDE ){
					++i;// Scan up to find element which >= partitioning element
				}
				--j;
				//while( values[ids[j]] > valueOfPartitioningElement ){
				while( compareValueByThreeKeys( ids[j], partitioningElement, firstKeyValues, secondKeyValues, thirdKeyValues) == LARGE_LEFT_HAND_SIDE ){
					--j;// Scan up to find element which <= partitioning element
				}
				if( j < i ){// i and j are crossed, so that partitioning complete
					break;
				}
				std::swap( ids[i], ids[j] );// Exchange element
			}

			 // Exchange IDs of j and iLeft+1 ( partitioning element )
			ids[iLeft+1] = ids[j];
			ids[j] = partitioningElement;

			if( iLeft - i + 1 >= j - iLeft ){// Elements in the right side >= Elements in the left side
				stack.push_back( std::pair<int, int>( i, iRight ) );
				iRight = j - 1;
			}else{// Elements in the right side < Elements in the left side
				stack.push_back( std::pair<int, int>( iLeft, j - 1) );
				iLeft = i;
			}
			//----- Quick sort <<<<<
		}
		
	}

	return;

}

// Compare values by its three keys
int compareValueByThreeKeys( const int lhsID, const int rhsID, 
	const double* firstKeyValues, const double* secondKeyValues, const double* thirdKeyValues ) {

	const double EPS(1e-10);

	//----- Compare value by the first key >>>>>
	if( fabs( firstKeyValues[lhsID] - firstKeyValues[rhsID] ) <= EPS ){
		//----- Compare value by the second key >>>>>
		if( fabs( secondKeyValues[lhsID] - secondKeyValues[rhsID] ) <= EPS ){
			//----- Compare value by the third key >>>>>
			if( fabs( thirdKeyValues[lhsID] - thirdKeyValues[rhsID] ) <= EPS ){
				return EQUAL;
			}else if( thirdKeyValues[lhsID] > thirdKeyValues[rhsID] - EPS ){
				return LARGE_LEFT_HAND_SIDE;
			}else{
				return SMALL_LEFT_HAND_SIDE;
			}		
			//----- Compare value by the third key <<<<<
		}else if( secondKeyValues[lhsID] > secondKeyValues[rhsID] - EPS ){
			return LARGE_LEFT_HAND_SIDE;
		}else{
			return SMALL_LEFT_HAND_SIDE;
		}
		//----- Compare value by the second key <<<<<
	}else if( firstKeyValues[lhsID] > firstKeyValues[rhsID] - EPS ){
		return LARGE_LEFT_HAND_SIDE;
	}else{
		return SMALL_LEFT_HAND_SIDE;
	}
	//----- Compare value by the first key <<<<<

}

// Calculate matrix product for 2 x 2 double matrix
void calcProductFor2x2DoubleMatrix( const CommonParameters::DoubleMatrix2x2& matInA, const CommonParameters::DoubleMatrix2x2& matInB, CommonParameters::DoubleMatrix2x2& matOut ){

	matOut.comp11 = matInA.comp11 * matInB.comp11 + matInA.comp12 * matInB.comp21;

#ifdef _DEBUG_WRITE
	std::cout << "matOut.comp11 matInA.comp11 matInB.comp11 matInA.comp12 matInB.comp21 : " << matOut.comp11 << " " << matInA.comp11 << " " << matInB.comp11 << " " << matInA.comp12 << " " << matInB.comp21 << std::endl;
#endif

	matOut.comp12 = matInA.comp11 * matInB.comp12 + matInA.comp12 * matInB.comp22;
	matOut.comp21 = matInA.comp21 * matInB.comp11 + matInA.comp22 * matInB.comp21;
	matOut.comp22 = matInA.comp21 * matInB.comp12 + matInA.comp22 * matInB.comp22;

	return;
}

// Calculate 3D vectors
CommonParameters::Vector3D calcVector3D( const CommonParameters::locationXYZ& startCoords, const CommonParameters::locationXYZ& endCoords ){

	const CommonParameters::Vector3D vec =  { endCoords.X - startCoords.X, endCoords.Y - startCoords.Y, endCoords.Z - startCoords.Z };
	return vec;

}

// Calculate outer product of 3D vectors
CommonParameters::Vector3D calcOuterProduct( const CommonParameters::Vector3D& vec1, const CommonParameters::Vector3D& vec2 ){

	CommonParameters::Vector3D result = { vec1.Y*vec2.Z - vec1.Z*vec2.Y, vec1.Z*vec2.X - vec1.X*vec2.Z, vec1.X*vec2.Y - vec1.Y*vec2.X };

	return result; 

}

// Calculate inner product of 3D vectors
double calcInnerProduct( const CommonParameters::Vector3D& vec1, const CommonParameters::Vector3D& vec2 ){

	return vec1.X*vec2.X + vec1.Y*vec2.Y + vec1.Z*vec2.Z;

}

