/**
* MIT License
* 
* Copyright (c) 2024 Manuel Bottini
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


/*
The implementation of the sensor fusion algorithm in BnOrientationAbsSensor took inspiration
from here https://github.com/Mayitzin/ahrs/blob/master/ahrs/filters/madgwick.py
Thank you Mario García for providing the code on GitHub.

The algorithm here has been modified for the purpose of being practical in motion capture
applications and Bodynodes in particular

*/


/**
The MIT License (MIT)

Copyright (c) 2019-2020, Mario García.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "BnOrientationAbsSensorFusion_ArduinoLSM9DS1.h"

#ifdef BN_ORIENTATION_ABS_SENSOR_FUSION_ARDUINO_LSM9DS1_H

#include <time.h>
#include <cmath>
#include <utility>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197
#endif

///////////////// BnMatrix START

// Copy constructor
BnMatrix::BnMatrix(const BnMatrix& other) {
    m_rows = other.m_rows;
    m_columns = other.m_columns;
    const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
    m_values = (float*)malloc(tot_bytes);
    memcpy(m_values, other.m_values, tot_bytes);
    m_isEmpty = other.m_isEmpty;
}

// Copy assignment operator
BnMatrix& BnMatrix::operator=(const BnMatrix& other) {
    if (this != &other) {
        if (m_values != nullptr) {
            free(m_values);
            m_values = nullptr;
        }
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
        m_values = (float*)malloc(tot_bytes);
        memcpy(m_values, other.m_values, tot_bytes);
        m_isEmpty = other.m_isEmpty;
    }
    return *this;
}

// Move constructor
BnMatrix::BnMatrix(BnMatrix&& other) noexcept {
    m_rows = other.m_rows;
    m_columns = other.m_columns;
    m_values = other.m_values;
    other.m_values = nullptr;
    m_isEmpty = other.m_isEmpty;
    other.m_isEmpty = true;
}

// Move assignment constuctor
BnMatrix& BnMatrix::operator=(BnMatrix&& other) noexcept {
    if (this != &other) {
        if (m_values != nullptr) {
            free(m_values);
        }
    	m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_values = other.m_values;
        other.m_values = nullptr;
        m_isEmpty = other.m_isEmpty;
        other.m_isEmpty = true;
    }
    return *this;
}

BnMatrix::BnMatrix(const uint16_t rows, const uint16_t columns ) {
    // Empty matrix
    m_rows = rows;
    m_columns = columns;
    m_values = (float*)malloc(sizeof(float)*m_rows*m_columns);
    m_isEmpty = true;
}

BnMatrix::BnMatrix(const uint16_t rows, const uint16_t columns, const float *values ) {
    m_rows = rows;
    m_columns = columns;
    const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
    m_values = (float*)malloc(tot_bytes);
    memcpy(m_values, values, tot_bytes);
    m_isEmpty = false;
}

BnMatrix::~BnMatrix(){
    if(m_values!=nullptr){
        free(m_values);
        m_values = nullptr;
    }
}

float BnMatrix::val(const uint16_t row, const uint16_t column) const {
    if(m_values != nullptr) {
        return m_values[column + row * m_columns];
    }
    return 0;
}

void BnMatrix::val(const uint16_t row, const uint16_t column, float value) {
    if(m_values != nullptr) {
        m_isEmpty = false;
        m_values[column + row * m_columns] = value;
    }
}

bool BnMatrix::isEmpty() const {
    return m_isEmpty;
}

uint16_t BnMatrix::rows() const {
    return m_rows;
}

uint16_t BnMatrix::columns() const {
    return m_columns;
}

void BnMatrix::print() const {
    if(m_isEmpty){
        printf("Object is empty\n");
        return;
    }

    for(uint16_t row_counter = 0; row_counter < m_rows; ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < m_columns; ++col_counter ){
            printf("%.4f ",  val(row_counter, col_counter));
        }
        printf("\n");
    }
}

BnMatrix BnMatrix::transposed() {
    if(m_isEmpty){
        printf("Object is empty\n");
        return BnMatrix(0,0);
    }

    float *tmp_m_values = transposeArray( m_rows, m_columns, m_values );
    BnMatrix transp(m_columns, m_rows, tmp_m_values);
    free(tmp_m_values);
    return transp;
}

float BnMatrix::determinant() const {
    if(m_isEmpty){
        printf("Object is empty\n");
        return -1;
    }
    if( m_columns != m_rows ) {
        // matrix must be a square matrix
        return 0;
    }
    if( m_columns == 1 ) {
        return m_values[0];
    }
    if( m_columns == 2 ) {
        return m_values[0] * m_values[3] - m_values[1] * m_values[2];
    }

    // This is a computeDeterminantLU implementation
    // Somehow the previous algo was creating problems
    // in the creation of the dll library (windows)
    // Maybe the recursion, who knows
	return determinantArray(m_rows, m_columns, m_values);
}

BnMatrix BnMatrix::cofactor() {
    if(m_isEmpty){
        printf("Object is empty\n");
        return BnMatrix(0,0);
    }
    if( m_columns != m_rows ) {
        // matrix must be a square matrix
        printf("matrix must be a square matrix\n");
        return BnMatrix(0,0);
    }

    float *values_cof = cofactorArray(m_rows, m_columns, m_values);
    BnMatrix matrC(m_rows, m_columns, values_cof);
    free(values_cof);
    return matrC;
}

void BnMatrix::multiply(const float mult){
    if(m_isEmpty){
        printf("Object is empty\n");
        return;
    }
    uint32_t num_elems = m_rows * m_columns;
    for( uint32_t counter = 0; counter < num_elems; ++counter ){
        m_values[counter] *= mult;
    }
}

BnMatrix BnMatrix::inverted() {
    if(m_isEmpty){
        printf("Object is empty\n");
        return BnMatrix(0,0);
    }
    const float tmp_determinant = determinantArray(m_rows, m_columns, m_values);
    float *values_cof = cofactorArray(m_rows, m_columns, m_values);
    float *values_cofT = transposeArray( m_rows, m_columns, values_cof );
    free(values_cof);
    multiplyInplaceArray(m_rows, m_columns, values_cofT, 1/tmp_determinant);
    BnMatrix matrCT(m_rows, m_columns, values_cofT);
    free(values_cofT);
	return matrCT;
}


void BnMatrix::multiply(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return;
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return;
    }
    if( matrL.columns() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return;
    }
    if( matrL.rows() != result.rows() || matrR.columns() != result.columns() ){
        // Bad shapes output matrix
        printf("equal: Bad shapes output matrix\n");
        return;
    }
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            float sum = 0;
            for(uint16_t elems_counter = 0; elems_counter < matrL.columns(); ++elems_counter ){
                sum += matrL.val(row_counter, elems_counter) * matrR.val(elems_counter, col_counter);
            }
            result.val(row_counter, col_counter, sum);
        }
    }
}

BnMatrix BnMatrix::multiply(const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return BnMatrix(0,0);
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return BnMatrix(0,0);
    }
    if( matrL.columns() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return BnMatrix(0,0);
    }
    BnMatrix result( matrL.rows(), matrR.columns() );
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            float sum = 0;
            for(uint16_t elems_counter = 0; elems_counter < matrL.columns(); ++elems_counter ){
                sum += matrL.val(row_counter, elems_counter) * matrR.val(elems_counter, col_counter);
            }
            result.val(row_counter, col_counter, sum);
        }
    }
    return result;
}

void BnMatrix::sum(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return;
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return;
    }
    if( matrL.columns() != matrR.columns() || matrL.rows() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return;
    }
    if( matrL.rows() != result.rows() || matrR.columns() != result.columns() ){
        // Bad shapes output matrix
        printf("equal: Bad shapes output matrix\n");
        return;
    }
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            const float sum = matrL.val(row_counter, col_counter) + matrR.val(row_counter, col_counter);
            result.val(row_counter, col_counter, sum);
        }
    }
}

BnMatrix BnMatrix::sum( const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return BnMatrix(0,0);
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return BnMatrix(0,0);
    }
    if( matrL.columns() != matrR.columns() || matrL.rows() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return BnMatrix(0,0);
    }

    BnMatrix result( matrL.rows(), matrL.columns() );
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            const float sum = matrL.val(row_counter, col_counter) + matrR.val(row_counter, col_counter);
            result.val(row_counter, col_counter, sum);
        }
    }
    return result;
}

void BnMatrix::subtract(BnMatrix &result, const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return;
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return;
    }
    if( matrL.columns() != matrR.columns() || matrL.rows() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return;
    }
    if( matrL.rows() != result.rows() || matrR.columns() != result.columns() ){
        // Bad shapes output matrix
        printf("equal: Bad shapes output matrix\n");
        return;
    }
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            const float sum = matrL.val(row_counter, col_counter) - matrR.val(row_counter, col_counter);
            result.val(row_counter, col_counter, sum);
        }
    }
}

BnMatrix BnMatrix::subtract( const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return BnMatrix(0,0);
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return BnMatrix(0,0);
    }
    if( matrL.columns() != matrR.columns() || matrL.rows() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return BnMatrix(0,0);
    }
    BnMatrix result( matrL.rows(), matrL.columns() );
    for(uint16_t row_counter = 0; row_counter < result.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < result.columns(); ++col_counter ){
            const float sum = matrL.val(row_counter, col_counter) - matrR.val(row_counter, col_counter);
            result.val(row_counter, col_counter, sum);
        }
    }
    return result;
}

bool BnMatrix::equal( const BnMatrix &matrL, const BnMatrix &matrR ){
    if(matrL.isEmpty()){
        printf("matrL is empty");
        return false;
    }
    if(matrR.isEmpty()){
        printf("matrR is empty");
        return false;
    }
    if( matrL.columns() != matrR.columns() || matrL.rows() != matrR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input matrices\n");
        return false;
    }
    for(uint16_t row_counter = 0; row_counter < matrL.rows(); ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < matrL.columns(); ++col_counter ){
            if( matrL.val(row_counter, col_counter) != matrR.val(row_counter, col_counter) ) {
                return false;
            }
        }
    }
    return true;
}

BnMatrix BnMatrix::identity( const uint16_t size ) {

    const uint16_t tmp_m_rows = size;
    const uint16_t tmp_m_columns = size;
    float* tmp_m_values = (float*)malloc(sizeof(float)*tmp_m_rows*tmp_m_columns);

    for(uint16_t row_counter = 0; row_counter < tmp_m_rows ; ++row_counter ) {
        for(uint16_t col_counter = 0; col_counter < tmp_m_columns; ++col_counter ) {
            if( row_counter == col_counter ) {
                tmp_m_values[row_counter + col_counter * tmp_m_columns] = 1;
            } else {
                tmp_m_values[row_counter + col_counter * tmp_m_columns] = 0;
            }
        }
    }

    BnMatrix eye(tmp_m_rows, tmp_m_columns, tmp_m_values);
    return eye;
}


float BnMatrix::determinantArray(const uint16_t rows, const uint16_t columns, const float *values_orig) {
    const size_t tot_bytes = sizeof(float)*rows*columns;
    float *values_matr = (float*)malloc(tot_bytes);
    memcpy(values_matr, values_orig, tot_bytes);

    float *tmp_switch = (float*)malloc(sizeof(float)*columns);
    float det = 1.0f;
    const uint16_t n = rows;
	for (uint16_t i = 0; i < n; ++i) {
		uint16_t maxRow = i;
		for (uint16_t k = i + 1; k < n; ++k) {
			if (abs(values_matr[k*columns + i]) > abs(values_matr[maxRow*columns+i])) {
				maxRow = k;
			}
		}
		if (i != maxRow) {
			const uint16_t row_bytes = sizeof(float)*columns;
			memcpy(tmp_switch, values_matr+i*columns, row_bytes);
			memcpy(values_matr+i*columns, values_matr+maxRow*columns, row_bytes);
			memcpy(values_matr+maxRow*columns, tmp_switch, row_bytes);
			det = -det;
		}
		if (values_matr[i*columns+i] == 0){
			return 0;
		}

		det *= values_matr[i*columns+i];
		for (uint16_t k = i + 1; k < n; ++k) {
			values_matr[i*columns+k] /= values_matr[i*columns+i];
		}
		for (uint16_t j = i + 1; j < n; ++j) {
			for (uint16_t k = i + 1; k < n; ++k) {
				values_matr[j*columns+k] -= values_matr[j*columns+i] * values_matr[i*columns+k];
			}
		}
	}
	free(tmp_switch);
    free(values_matr);
	return det;
}

float* BnMatrix::cofactorArray(const uint16_t rows, const uint16_t columns, const float *values_orig) {
    float *values_cof = (float*)malloc(sizeof(float)*rows*columns);
    for(uint16_t row_counter = 0; row_counter < rows; ++row_counter ){
        for(uint16_t col_counter = 0; col_counter < columns; ++col_counter ){
            const float mult = ( ( col_counter + row_counter ) % 2 == 0 ) ? 1 :-1;

            // Creation of submatrix
            float *new_values = (float*)malloc(sizeof(float)*(rows-1) * (columns-1));
            uint32_t mv_counter = 0;
            for(uint16_t row_counter_i = 0; row_counter_i < rows; ++row_counter_i ){
                for(uint16_t col_counter_i = 0; col_counter_i < columns; ++col_counter_i ){
                    if( row_counter_i != row_counter && col_counter_i != col_counter ){
                    	new_values[mv_counter] = values_orig[row_counter_i*columns +col_counter_i];
                        ++mv_counter;
                    }
                }
            }

            float determinant = 1;
            if( rows > 1 ){
                determinant = determinantArray(rows-1, columns-1, new_values);
            }
            free(new_values);
            values_cof[row_counter*columns + col_counter] = mult * determinant;
        }
    }
    return values_cof;
}

float* BnMatrix::transposeArray(const uint16_t rows, const uint16_t columns, const float *values_orig) {
    float *values_tr = (float*)malloc(sizeof(float)*rows*columns);

    for(uint16_t row_counter = 0; row_counter < rows ; ++row_counter ) {
        for(uint16_t col_counter = 0; col_counter < columns; ++col_counter ) {
        	values_tr[row_counter + col_counter * rows] = values_orig[col_counter + row_counter * columns] ;
        }
    }

    return values_tr;
}

void BnMatrix::multiplyInplaceArray(const uint16_t rows, const uint16_t columns, float *values_orig, const float mult){
    uint32_t num_elems = rows * columns;
    for( uint32_t counter = 0; counter < num_elems; ++counter ){
    	values_orig[counter] *= mult;
    }
}
///////////////// BnMatrix END

///////////////// BnVector START 

// Copy constructor from BnMatrix
BnVector::BnVector(const BnMatrix& other) : BnMatrix(0, 0)  {
    if( other.columns() != 1 ) {
        printf("Input matrix cannot be represented with a vector\n");
        m_isEmpty = true;
        return;
    }
    m_rows = other.rows();
    m_columns = 1;
    const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
    m_values = (float*)malloc(tot_bytes);
    for( uint16_t el_counter = 0; el_counter < m_rows; ++ el_counter ){
        m_values[el_counter] = other.val(el_counter,0);
    }
    m_isEmpty = other.isEmpty();
}

// Copy assignment operator from BnMatrix
BnVector& BnVector::operator=(const BnMatrix& other) {
    if( other.columns() != 1 ) {
        printf("Input matrix cannot be represented with a vector\n");
        m_isEmpty = true;
        return *this;
    }

    if (this != &other) {
        m_rows = other.rows();
        m_columns = 1;
        const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
        m_values = (float*)malloc(tot_bytes);
        for( uint16_t el_counter = 0; el_counter < m_rows; ++ el_counter ){
            m_values[el_counter] = other.val(el_counter,0);
        }
        m_isEmpty = other.isEmpty();
    }
    return *this;
}

void BnVector::normalize() {

    if(isEmpty()){
        printf("Object is empty\n");
        return;
    }
    float sum_squares = 0;
    for(uint16_t elem_counter = 0; elem_counter < m_rows; ++elem_counter) {
        sum_squares += val(elem_counter, 0) * val(elem_counter, 0);
    }

    const float inv_sqrt_sum_squares = 1/std::sqrt( sum_squares );
    multiply(inv_sqrt_sum_squares);
}

void BnVector::productElementwise(BnVector &result, const BnVector &vecL, const BnVector &vecR ) {
    if(vecL.isEmpty()){
        printf("vecL is empty");
        return;
    }
    if(vecR.isEmpty()){
        printf("vecR is empty");
        return;
    }
    if( vecL.rows() != vecR.rows() ){
        // Bad shapes input vectors
        printf("equal: Bad shapes input vectors\n");
        return;
    }
    if( vecL.rows() != result.rows() || vecR.rows() != result.rows() ){
        // Bad shapes output vector
        printf("equal: Bad shapes output vector\n");
        return;
    }
    for(uint16_t elems_counter = 0; elems_counter < vecL.rows(); ++elems_counter ){
        const float res = vecL.val(elems_counter, 0) * vecR.val(elems_counter,0 );
        result.val(elems_counter, 0 , res);
    }
}

BnVector BnVector::productElementwise(const BnVector &vecL, const BnVector &vecR ) {
    if(vecL.isEmpty()){
        printf("vecL is empty");
        return BnVector(0);
    }
    if(vecR.isEmpty()){
        printf("vecR is empty");
        return BnVector(0);
    }
    if( vecL.rows() != vecR.rows() ){
        // Bad shapes input vectors
        printf("equal: Bad shapes input vectors\n");
        return BnVector(0);
    }
    BnVector result( vecL.rows() );
    for(uint16_t elems_counter = 0; elems_counter < vecL.rows(); ++elems_counter ){
        const float res = vecL.val(elems_counter, 0 ) * vecR.val(elems_counter, 0);
        result.val(elems_counter, 0, res);
    }
    return result;
}

///////////////// BnVector END

///////////////// BnQuaternion START

// Copy constructor from BnMatrix
BnQuaternion::BnQuaternion(const BnMatrix& other) : BnVector(4) {
    if( other.rows() != 4 || other.columns() != 1 ) {
        printf("Input matrix cannot be represented with a quaternion\n");
        m_isEmpty = true;
        return;
    }
    m_rows = 4;
    m_columns = 1;
    if (m_values != nullptr) {
        free(m_values);
        m_values = nullptr;
    }
    const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
    m_values = (float*)malloc(tot_bytes);
    m_values[0] = other.val(0,0);
    m_values[1] = other.val(1,0);
    m_values[2] = other.val(2,0);
    m_values[3] = other.val(3,0);
    m_isEmpty = other.isEmpty();
}

// Copy assignment operator from BnMatrix
BnQuaternion& BnQuaternion::operator=(const BnMatrix& other) {
    if( other.rows() != 4 || other.columns() != 1 ) {
        printf("Input matrix cannot be represented with a quaternion\n");
        m_isEmpty = true;
        return *this;
    }

    if (this != &other) {
        m_rows = 4;
        m_columns = 1;
        if (m_values != nullptr) {
            free(m_values);
            m_values = nullptr;
        }
        const size_t tot_bytes = sizeof(float)*m_rows*m_columns;
        m_values = (float*)malloc(tot_bytes);
        m_values[0] = other.val(0,0);
        m_values[1] = other.val(1,0);
        m_values[2] = other.val(2,0);
        m_values[3] = other.val(3,0);
        m_isEmpty = other.isEmpty();
    }
    return *this;
}

void BnQuaternion::getRotationMatrix(BnMatrix &matrO) {
    if( matrO.rows() != 3 || matrO.columns() != 3 ) {
        // Rotations Matrix must be a 3x3 matrix
        return;
    }
    /* The formula is what it is, so let's just apply it
    R = [
            [ 1-2y^2-2z^2   2xy-2wz         2xz+2wy     ],
            [ 2xy+2wz       1-2x^2-2z^2     2yz-2wx     ],
            ​[ 2xz-2wy       2yz+2wx         1-2x^2-2y^2 ]
        ]
    */

    matrO.val( 0, 0, 1-2*(y()*y()+z()*z()) );
    matrO.val( 0, 1, 2*(x()*y()-w()*z()) );
    matrO.val( 0, 2, 2*(x()*z()+w()*y()) );

    matrO.val( 1, 0, 2*(x()*y()+w()*z()) );
    matrO.val( 1, 1, 1-2*(x()*x()+z()*z()) );
    matrO.val( 1, 2, 2*(y()*z()-w()*x()) );

    matrO.val( 2, 0, 2*(x()*z()-w()*y()) );
    matrO.val( 2, 1, 2*(y()*z()+w()*x()) );
    matrO.val( 2, 2, 1-2*(x()*x()+y()*y()) );
}

void BnQuaternion::getEulerAngles(BnEulerAngles &eulerAnglesO) {

    if( eulerAnglesO.columns() != 1 || eulerAnglesO.rows() != 3 ) {
        printf("Euler Angle vector must be a 3x1 matrix or 3 elements vector\n");
        return;
    }

    BnMatrix matrixR(3,3);
    getRotationMatrix(matrixR);
    
    eulerAnglesO.roll( -std::atan2( matrixR.val( 2, 1 ), matrixR.val( 2, 2 ) ) ); // Roll
    eulerAnglesO.pitch( -std::asin( -matrixR.val( 2, 0 ) ) );             // Pitch
    eulerAnglesO.yaw( -std::atan2( matrixR.val( 1, 0 ), matrixR.val( 0, 0) ) );   // Yaw


}

void BnQuaternion::conjugate() {
    // [w, -x, -y, -z]
    x( -x() );
    x( -y() );
    x( -z() );
}

float BnQuaternion::w() const {
    return val(0,0);
}

float BnQuaternion::x() const {
    return val(1, 0);
}

float BnQuaternion::y() const {
    return val(2, 0);
}

float BnQuaternion::z() const{
    return val(3, 0);
}

void BnQuaternion::w(const float value) {
    val(0,0,value);
}

void BnQuaternion::x(const float value) {
    val(1,0,value);
}

void BnQuaternion::y(const float value) {
    val(2,0,value);
}

void BnQuaternion::z(const float value) {
    val(3,0,value);
}

void BnQuaternion::productHamilton(BnQuaternion &result, const BnQuaternion &quatL, const BnQuaternion &quatR ) {
    
    if(quatL.isEmpty()){
        printf("quatL is empty");
        return;
    }
    if(quatR.isEmpty()){
        printf("quatR is empty");
        return;
    }
    if( quatL.columns() != quatR.columns() || quatL.rows() != quatR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input quaternions\n");
        return;
    }
    if( quatL.rows() != result.rows() || quatR.columns() != result.columns() ){
        // Bad shapes output matrix
        printf("equal: Bad shapes output quaternion\n");
        return;
    }
    
    result.w( quatL.w()*quatR.w() - quatL.x()*quatR.x() - quatL.y()*quatR.y() - quatL.z()*quatR.z());
    result.x( quatL.w()*quatR.x() - quatL.x()*quatR.w() - quatL.y()*quatR.z() - quatL.z()*quatR.y());
    result.y( quatL.w()*quatR.y() - quatL.x()*quatR.z() - quatL.y()*quatR.w() - quatL.z()*quatR.x());
    result.z( quatL.w()*quatR.z() - quatL.x()*quatR.y() - quatL.y()*quatR.x() - quatL.z()*quatR.w());
}

BnQuaternion BnQuaternion::productHamilton( const BnQuaternion &quatL, const BnQuaternion &quatR ) {
    if(quatL.isEmpty()){
        printf("quatL is empty");
        return BnQuaternion();
    }
    if(quatR.isEmpty()){
        printf("quatR is empty");
        return BnQuaternion();
    }
    if( quatL.columns() != quatR.columns() || quatL.rows() != quatR.rows() ){
        // Bad shapes input matrices
        printf("equal: Bad shapes input quaternions\n");
        return BnQuaternion();
    }
    BnQuaternion result;
    result.w( quatL.w()*quatR.w() - quatL.x()*quatR.x() - quatL.y()*quatR.y() - quatL.z()*quatR.z());
    result.x( quatL.w()*quatR.x() + quatL.x()*quatR.w() + quatL.y()*quatR.z() - quatL.z()*quatR.y());
    result.y( quatL.w()*quatR.y() - quatL.x()*quatR.z() + quatL.y()*quatR.w() + quatL.z()*quatR.x());
    result.z( quatL.w()*quatR.z() + quatL.x()*quatR.y() - quatL.y()*quatR.x() + quatL.z()*quatR.w());
    return result;
}

/////////////////// BnQuaternion END

////////////////// BnEulerAngles START

// The order in which rotations are applied is Roll, Pitch, Yaw (or XYZ)
void BnEulerAngles::getRotationMatrix(BnMatrix &matrO) {
    // I need to construct 3 single rotation matrices, and then multiply them together
    
    // Yaw
    BnMatrix matrYaw(3,3);
    matrYaw.val( 0, 0, std::cos( yaw() ) );
    matrYaw.val( 0, 1, std::sin( yaw() ) );
    matrYaw.val( 0, 2, 0 );

    matrYaw.val( 1, 0, -std::sin( yaw() ) );
    matrYaw.val( 1, 1, std::cos( yaw() ) );
    matrYaw.val( 1, 2, 0 );

    matrYaw.val( 2, 0, 0 );
    matrYaw.val( 2, 1, 0 );
    matrYaw.val( 2, 2, 1 );
    //matrYaw.print();

    // Pitch
    BnMatrix matrPitch(3,3);
    matrPitch.val( 0, 0, std::cos( pitch() ) );
    matrPitch.val( 0, 1, 0);
    matrPitch.val( 0, 2, -std::sin( pitch() )  );

    matrPitch.val( 1, 0, 0 );
    matrPitch.val( 1, 1, 1 );
    matrPitch.val( 1, 2, 0 );

    matrPitch.val( 2, 0, std::sin( pitch() )  );
    matrPitch.val( 2, 1, 0 );
    matrPitch.val( 2, 2, std::cos( pitch() ) );
    //matrPitch.print();


    // Roll
    BnMatrix matrRoll(3,3);
    matrRoll.val( 0, 0, 1 );
    matrRoll.val( 0, 1, 0 );
    matrRoll.val( 0, 2, 0 );

    matrRoll.val( 1, 0, 0 );
    matrRoll.val( 1, 1, std::cos( roll() ) );
    matrRoll.val( 1, 2, std::sin( roll() ) );

    matrRoll.val( 2, 0, 0 );
    matrRoll.val( 2, 1, -std::sin( roll() ) );
    matrRoll.val( 2, 2, std::cos( roll() ) );
    //matrRoll.print();

    BnMatrix matrTmp(3,3);
    BnMatrix::multiply(matrTmp, matrPitch, matrRoll);
    BnMatrix::multiply(matrO, matrYaw, matrTmp );
}

// The order in which rotations are applied is Roll, Pitch, Yaw (or XYZ)
void BnEulerAngles::getQuaternion(BnQuaternion &quaternionO) {
    BnMatrix matrR(3,3);
    getRotationMatrix(matrR);

    const float trace = matrR.val(0, 0) + matrR.val(1, 1) + matrR.val(2, 2);
    if(trace > 0) {
        quaternionO.w( std::sqrt( 1 + trace ) / 2);
        quaternionO.x( ( matrR.val(2, 1) - matrR.val(1, 2) ) / ( 4 * quaternionO.w() ) );
        quaternionO.y( ( matrR.val(0, 2) - matrR.val(2, 0) ) / ( 4 * quaternionO.w() ) );
        quaternionO.z( ( matrR.val(1, 0) - matrR.val(0, 1) ) / ( 4 * quaternionO.w() ) );
    } else {
        if( matrR.val(0, 0) >= matrR.val(1, 1) && matrR.val(0, 0) >= matrR.val(2, 2) ) {
            quaternionO.x( std::sqrt( 1 + matrR.val(1, 1) - matrR.val(0, 0) - matrR.val(2, 2) ) / 2);
            quaternionO.w( ( matrR.val(2, 1) - matrR.val(1, 2) ) / ( 4 * quaternionO.x() ) );
            quaternionO.y( ( matrR.val(0, 1) + matrR.val(1, 0) ) / ( 4 * quaternionO.x() ) );
            quaternionO.z( ( matrR.val(0, 2) + matrR.val(2, 0) ) / ( 4 * quaternionO.x() ) );
        } else if( matrR.val(1, 1) >= matrR.val(0, 0) && matrR.val(1, 1) >= matrR.val(2, 2) ) {
            quaternionO.y( std::sqrt( 1 + matrR.val(0, 0) - matrR.val(1, 1) - matrR.val(2, 2) ) / 2);
            quaternionO.w( ( matrR.val(0, 2) - matrR.val(2, 0) ) / ( 4 * quaternionO.y() ) );
            quaternionO.x( ( matrR.val(0, 1) + matrR.val(1, 0) ) / ( 4 * quaternionO.y() ) );
            quaternionO.z( ( matrR.val(1, 2) + matrR.val(2, 1) ) / ( 4 * quaternionO.y() ) );
        } else {
            quaternionO.z( std::sqrt( 1 + matrR.val(2, 2) - matrR.val(0, 0) - matrR.val(1, 1) ) / 2);
            quaternionO.w( ( matrR.val(1, 0) - matrR.val(0, 1) ) / ( 4 * quaternionO.z() ) );
            quaternionO.x( ( matrR.val(0, 2) + matrR.val(2, 0) ) / ( 4 * quaternionO.z() ) );
            quaternionO.y( ( matrR.val(1, 2) + matrR.val(2, 1) ) / ( 4 * quaternionO.z() ) );
        }
    }
    quaternionO.normalize();
}

float BnEulerAngles::roll() const {
    return val(0,0);
}

float BnEulerAngles::pitch() const {
    return val(1,0);
}

float BnEulerAngles::yaw() const {
    return val(2,0);
}

void BnEulerAngles::roll(const float value) {
    val(0,0, value);
}

void BnEulerAngles::pitch(const float value) {
    val(1,0, value);
}

void BnEulerAngles::yaw(const float value) {
    val(2,0, value);
}

float BnEulerAngles::toDegrees(const float rads) {
    return rads / M_PI * 180.0f;
}

float BnEulerAngles::toRadiants(const float degrees) {
    return degrees / 180.0f * M_PI;
}

void BnEulerAngles::printRadiants() const {
    print();
}

void BnEulerAngles::printDegrees() const {
    printf("%.4f, %.4f, %.4f\n", toDegrees(val(0,0)), toDegrees(val(1,0)), toDegrees(val(2,0)) );
}

////////////////// BnEulerAngles END

///////////////// BnSensorFusionMadgwickAHRS START

BnSensorFusionMadgwickAHRS::BnSensorFusionMadgwickAHRS(
        const uint32_t samplePeriod_ms,
        const float gain,
        const float rescaleGyro,
        const BnVector &axisSigns):

        m_samplePeriod_ms(samplePeriod_ms),
        m_gain(gain),
        m_rescaleGyro(rescaleGyro),
        m_axisSigns(axisSigns),
        m_internalQuat(),
        m_timeNow(0) {} ;

void BnSensorFusionMadgwickAHRS::init(const BnQuaternion &initialQuat) {
    m_internalQuat = initialQuat;
}

void BnSensorFusionMadgwickAHRS::updateSamplePeriod_ms(const uint64_t time_now) {
    if(m_timeNow != 0) {
        m_samplePeriod_ms = static_cast<uint32_t>(time_now - m_timeNow);
    }
    m_timeNow = time_now;
}

void BnSensorFusionMadgwickAHRS::updateIMU(
    const BnVector &gyro,
    const BnVector &accel,
    const uint64_t time_now) {

    if(m_internalQuat.isEmpty() ){
        printf("Please call init first\n");
        return;
    }

    BnVector gyro_i = BnVector::productElementwise( gyro, m_axisSigns );
    gyro_i.multiply( m_rescaleGyro );
    BnVector accel_i = BnVector::productElementwise( accel, m_axisSigns );
    updateSamplePeriod_ms(time_now);

    //# (eq. 12)
    BnQuaternion qDot( 0, gyro_i.val(0,0), gyro_i.val(1,0), gyro_i.val(2,0) );
    qDot = BnQuaternion::productHamilton( m_internalQuat, qDot);
    qDot.multiply(0.5);

    accel_i.normalize();

    m_internalQuat.normalize();    
    const float qw = m_internalQuat.w();
    const float qx = m_internalQuat.x();
    const float qy = m_internalQuat.y();
    const float qz = m_internalQuat.z();
    BnVector vecF( 3 );
    vecF.val(0,0, 2.0 * ( qx* qz - qw*qy) - accel_i.val(0,0) ); 
    vecF.val(0,1, 2.0 * ( qw* qx - qy*qz) - accel_i.val(1,0) ); 
    vecF.val(0,2, 2.0 * ( 0.5 - qx* qx - qy*qy) - accel_i.val(2,0) ); 
   
    //# Transposed Jacobian (eq. 26)
    BnMatrix matrJT( 4,3 );
    matrJT.val( 0,0, -2.0*qy );
    matrJT.val( 1,0, 2.0*qz );
    matrJT.val( 2,0, -2.0*qw );
    matrJT.val( 3,0, 2.0*qx );

    matrJT.val( 0,1, 2.0*qx );
    matrJT.val( 1,1, 2.0*qw );
    matrJT.val( 2,1, 2.0*qz );
    matrJT.val( 3,1, 2.0*qy );

    matrJT.val( 0,2, 0.0 );
    matrJT.val( 1,2, -4.0*qx );
    matrJT.val( 2,2, -4.0*qy );
    matrJT.val( 3,2, 0.0 );

    //# Objective Function Gradient
    //# (eq. 34)
    BnQuaternion gradient = BnMatrix::multiply( matrJT, vecF );
    gradient.normalize();
    gradient.multiply(m_gain);
    //# (eq. 33)
    qDot = BnQuaternion::subtract( qDot, gradient );
    
    //# (eq. 13)
    qDot.multiply( static_cast<float>(m_samplePeriod_ms) / 1000.0 );
    m_internalQuat = BnQuaternion::sum(m_internalQuat, qDot);
    m_internalQuat.normalize();
}

void BnSensorFusionMadgwickAHRS::updateMAGR(
    const BnVector &gyro,
    const BnVector &accel,
    const BnVector &magn,
    const uint64_t time_now) {

    if( m_internalQuat.isEmpty() ){
        printf("Please call init first\n");
        return;
    }

    BnVector gyro_i = BnVector::productElementwise( gyro, m_axisSigns );
    gyro_i.multiply( m_rescaleGyro );
    BnVector accel_i = BnVector::productElementwise( accel, m_axisSigns );
    BnVector magn_i = BnVector::productElementwise( magn, m_axisSigns );
    updateSamplePeriod_ms(time_now);

    
    //# (eq. 12)
    BnQuaternion qDot( 0, gyro_i.val(0,0), gyro_i.val(1,0), gyro_i.val(2,0) );
    qDot = BnQuaternion::productHamilton( m_internalQuat, qDot);
    qDot.multiply(0.5);
    
    accel_i.normalize();
    magn_i.normalize();


    //# Rotate normalized magnetometer measurements
    //# (eq. 45)
    
    // h = quat_prod(self.internalQuat, quat_prod([0, *m], quat_conj(self.internalQuat)))
    BnQuaternion tmp = m_internalQuat;
    tmp.conjugate();
    BnQuaternion quatH( 0, magn_i.val(0,0), magn_i.val(1,0), magn_i.val(2,0) );
    quatH = BnQuaternion::productHamilton( quatH, tmp );
    quatH = BnQuaternion::productHamilton( m_internalQuat, quatH);


    //# (eq. 46)
    const float bx = std::sqrt( quatH.x()*quatH.x() + quatH.y()*quatH.y() );
    const float bz = quatH.z();

    m_internalQuat.normalize();
    BnVector vecF( 6 );
    
    const float qw = m_internalQuat.w();
    const float qx = m_internalQuat.x();
    const float qy = m_internalQuat.y();
    const float qz = m_internalQuat.z();

    
    vecF.val(0,0, 2.0 * ( qx* qz - qw*qy) - accel_i.val(0,0) ); 
    vecF.val(0,1, 2.0 * ( qw* qx - qy*qz) - accel_i.val(0,1) ); 
    vecF.val(0,2, 2.0 * ( 0.5 - qx* qx - qy*qy) - accel_i.val(0,2) ); 
    vecF.val(0,3, 2.0*bx*(0.5 - qy*qy - qz*qz ) + 2.0*bz*( qx*qz - qw*qy) - magn_i.val(0,0) ); 
    vecF.val(0,4, 2.0*bx*(qx* qx - qw*qz) + 2.0*bz*(qw* qx + qy*qz) - magn_i.val(0,1) ); 
    vecF.val(0,5, 2.0*bx*(qw*qy + qx* qz) + 2.0*bz*(0.5 - qx* qx - qy*qy) - magn_i.val(0,2) ); 


    //# Transposed Jacobian (eq. 26)
    BnMatrix matrJT( 4,6 );
    matrJT.val( 0,0, -2.0*qy );
    matrJT.val( 1,0, 2.0*qz );
    matrJT.val( 2,0, -2.0*qw );
    matrJT.val( 3,0, 2.0*qx );

    matrJT.val( 0,1, 2.0*qx );
    matrJT.val( 1,1, 2.0*qw );
    matrJT.val( 2,1, 2.0*qz );
    matrJT.val( 3,1, 2.0*qy );

    matrJT.val( 0,2, 0.0);
    matrJT.val( 1,2, -4.0*qx );
    matrJT.val( 2,2, -4.0*qy );
    matrJT.val( 3,2, 0.0 );

    matrJT.val( 0,3, -2.0*bz*qy );
    matrJT.val( 1,3, 2.0*bz*qz );
    matrJT.val( 2,3, -4.0*bx*qy-2.0*bz*qw );
    matrJT.val( 3,3, -4.0*bx*qz+2.0*bz*qx );

    matrJT.val( 0,4, -2.0*bx*qz+2.0*bz*qx );
    matrJT.val( 1,4, 2.0*bx*qy+2.0*bz*qw );
    matrJT.val( 2,4, 2.0*bx*qx+2.0*bz*qz );
    matrJT.val( 3,4, -2.0*bx*qw+2.0*bz*qy );

    matrJT.val( 0,5, 2.0*bx*qy );
    matrJT.val( 1,5, 2.0*bx*qz-4.0*bz*qx );
    matrJT.val( 2,5, 2.0*bx*qw-4.0*bz*qy );
    matrJT.val( 3,5, 2.0*bx*qx  );
    

    //# Objective Function Gradient
    //# (eq. 34)
    BnQuaternion gradient = BnMatrix::multiply( matrJT, vecF );
    gradient.normalize();
    gradient.multiply(m_gain);

    //# (eq. 33)
    qDot = BnMatrix::subtract( qDot, gradient );

    //# (eq. 13)
    qDot.multiply( static_cast<float>(m_samplePeriod_ms) / 1000.0 );
    m_internalQuat = BnMatrix::sum(m_internalQuat, qDot);
    m_internalQuat.normalize();
    
}

void BnSensorFusionMadgwickAHRS::getQuaternion(BnQuaternion &out) {
    if(m_internalQuat.isEmpty()) {
        printf("The internal quaternion is empty\n");
        return;    
    }
    out.w( m_internalQuat.w() );
    out.x( m_internalQuat.x() );
    out.y( m_internalQuat.y() );
    out.z( m_internalQuat.z() );
}


///////////////// BnSensorFusionMadgwickAHRS END


///////////////// BnOrientationAbsSensor START

// You need to install the Arduino_LSM9DS1 package from Tools->Manage Libraries...
#include <Arduino_LSM9DS1.h>

const uint32_t samplePeriod_ms = SENSOR_READ_INTERVAL_MS;
const float gain = 0.8;
const float rescaleGyro = 0.02;
const float signs_vals[] = {1.0, -1.0, 1.0};
const BnVector axisSigns(3, signs_vals );

BnOrientationAbsSensor::BnOrientationAbsSensorFusion() : s_sensorfusion( samplePeriod_ms, gain, rescaleGyro, axisSigns ) { }

void BnOrientationAbsSensor::init(){

    s_enabled = true;
    pinMode(STATUS_SENSOR_HMI_LED_P, OUTPUT);
    BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION;
    s_statusSensorLED.on = false;
    s_statusSensorLED.lastToggle = millis();

    s_sensorInit=false;

    s_lastReadSensorTime=millis();
    s_sensorReconnectionTime=millis();
    /* Initialise the sensor */
    if(IMU.begin()) {
        setStatus(SENSOR_STATUS_WORKING);
    } else {
        setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
    }
    BnQuaternion initialQuat( 1.0, 0.0, 0.0, 0.0 );
    s_sensorfusion.init(initialQuat);
}

void BnOrientationAbsSensor::setStatus(int sensor_status){
    if(sensor_status == SENSOR_STATUS_NOT_ACCESSIBLE) {
        s_sensorInit=false;
        DEBUG_PRINTLN("Ooops, no LSM9DS1 detected ... Check your board!");
        BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION;
        s_statusSensorLED.on = true;
        s_statusSensorLED.lastToggle = millis();
    } else if(sensor_status == SENSOR_STATUS_CALIBRATING) {
        if(millis()-s_statusSensorLED.lastToggle > 500) {
            s_statusSensorLED.lastToggle = millis();
            s_statusSensorLED.on = !s_statusSensorLED.on;
            if(s_statusSensorLED.on){
                BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION;
            } else {
                BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION;
            }
        }
    } else if(sensor_status == SENSOR_STATUS_WORKING) {
        s_sensorInit=true;
        BN_NODE_SPECIFIC_BN_ORIENTATION_ABS_SENSOR_WRITE_STATUS_PIN_FUNCTION;
        s_statusSensorLED.on = false;
        s_statusSensorLED.lastToggle = millis();
    }
}

bool BnOrientationAbsSensor::checkAllOk(){
    if(!s_sensorInit){
        if(millis()-s_sensorReconnectionTime<5000){
            return false;
        }
        DEBUG_PRINTLN("Sensor not connected");
        s_sensorReconnectionTime=millis();
        if(IMU.begin()) {
            setStatus(SENSOR_STATUS_WORKING);
            return true;
        } else {
            setStatus(SENSOR_STATUS_NOT_ACCESSIBLE);
            return false;
        }
    }
    if(millis()-s_lastReadSensorTime<SENSOR_READ_INTERVAL_MS){
        return false;
    }

    if (!IMU.gyroscopeAvailable() || !IMU.accelerationAvailable() || !IMU.magneticFieldAvailable() ) {
        return false;
    }

    float accx;
    float accy;
    float accz;
    float magnx;
    float magny;
    float magnz;
    float gyrox;
    float gyroy;
    float gyroz;

    IMU.readAcceleration(accx, accy, accz); // outputs in g, convert to m/s^2
    IMU.readMagneticField(magnx, magny, magnz);  // outputs in uT
    IMU.readGyroscope(gyrox, gyroy, gyroz);  // outputs in dps, covert it to rad/s

    const float accel1_vals[] = { accx, accy, accz };
    const float magn1_vals[] = { magnx, magny, magnz};
    const float gyro1_vals[] = { gyrox, gyroy, gyroz};

    const BnVector accel1_vec(3, accel1_vals );
    const BnVector magn1_vec(3, magn1_vals );
    const BnVector gyro1_vec(3, gyro1_vals );    
    //s_sensorfusion.updateMAGR(gyro1_vec, accel1_vec, magn1_vec, millis());
    s_sensorfusion.updateIMU(gyro1_vec, accel1_vec, millis());

    BnQuaternion resQ;
    s_sensorfusion.getQuaternion(resQ);
    
    float svalues[4] = { resQ.w(), resQ.x(), resQ.y(), resQ.z() };
    float tvalues[4];
    realignAxis(svalues, tvalues);

    s_values[0] = tvalues[0];
    s_values[1] = tvalues[1];
    s_values[2] = tvalues[2];
    s_values[3] = tvalues[3];

    return true;
}

bool BnOrientationAbsSensor::isCalibrated(){
    setStatus(SENSOR_STATUS_WORKING);
    return true;
}

BnSensorData BnOrientationAbsSensor::getData(){
  s_lastReadSensorTime=millis();
  /*
  DEBUG_PRINT("values = ");
  DEBUG_PRINT(s_values[0]);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(s_values[1]);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(s_values[2]);
  DEBUG_PRINT(", ");
  DEBUG_PRINTLN(s_values[3]);
  */
  BnSensorData sensorData;
  sensorData.setValues(s_values, SENSORTYPE_ORIENTATION_ABS_TAG);
  return sensorData;
}

BnType BnOrientationAbsSensor::getType(){
    return SENSORTYPE_ORIENTATION_ABS_TAG;
}

void BnOrientationAbsSensor::setEnable(bool enable_status){
    s_enabled = enable_status;
}

bool BnOrientationAbsSensor::isEnabled(){
    return s_enabled;
}

void BnOrientationAbsSensor::realignAxis(float values[], float revalues[]){

  // Axis W
  #if OUT_AXIS_W == SENSOR_AXIS_W
  revalues[0] = values[0];
  #elif OUT_AXIS_W == SENSOR_AXIS_X
  revalues[0] = values[1];
  #elif OUT_AXIS_W == SENSOR_AXIS_Y
  revalues[0] = values[2];
  #elif OUT_AXIS_W == SENSOR_AXIS_Z
  revalues[0] = values[3];
  #endif

  // Axis X
  #if OUT_AXIS_X == SENSOR_AXIS_W
  revalues[1] = values[0];
  #elif OUT_AXIS_X == SENSOR_AXIS_X
  revalues[1] = values[1];
  #elif OUT_AXIS_X == SENSOR_AXIS_Y
  revalues[1] = values[2];
  #elif OUT_AXIS_X == SENSOR_AXIS_Z
  revalues[1] = values[3];
  #endif

  // Axis Y
  #if OUT_AXIS_Y == SENSOR_AXIS_W
  revalues[2] = values[0];
  #elif OUT_AXIS_Y == SENSOR_AXIS_X
  revalues[2] = values[1];
  #elif OUT_AXIS_Y == SENSOR_AXIS_Y
  revalues[2] = values[2];
  #elif OUT_AXIS_Y == SENSOR_AXIS_Z
  revalues[2] = values[3];
  #endif

  // Axis Z
  #if OUT_AXIS_Z == SENSOR_AXIS_W
  revalues[3] = values[0];
  #elif OUT_AXIS_Z == SENSOR_AXIS_X
  revalues[3] = values[1];
  #elif OUT_AXIS_Z == SENSOR_AXIS_Y
  revalues[3] = values[2];
  #elif OUT_AXIS_Z == SENSOR_AXIS_Z
  revalues[3] = values[3];
  #endif

  revalues[0] = MUL_AXIS_W * revalues[0];
  revalues[1] = MUL_AXIS_X * revalues[1];
  revalues[2] = MUL_AXIS_Y * revalues[2];
  revalues[3] = MUL_AXIS_Z * revalues[3];
}

///////////////// BnOrientationAbsSensor END

#endif // BN_ORIENTATION_ABS_SENSOR_FUSION_ARDUINO_LSM9DS1_H

