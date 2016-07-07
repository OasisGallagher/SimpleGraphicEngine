#include "vector3.h"
#include "matrix.h"

Matrix Matrix::identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
Matrix Matrix::zero;

void Matrix::operator()(size_t row, const Vector3& other){
	buffer[row][0] = other.x;
	buffer[row][1] = other.y;
	buffer[row][2] = other.z;
}

Matrix Matrix::operator * (const Matrix& other) const {
	Matrix ans;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				ans(i, j) += (*this)(i, k) * other(k, j);
			}
		}
	}
	
	return ans;
}

Matrix& Matrix::operator *= (const Matrix& other) {
	Matrix ans;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				ans(i, j) += (*this)(i, k) * other(k, j);
			}
		}
	}

	return *this = ans;
}
