#pragma once
#include <iostream>
#include <iomanip>
#include <math.h>
using namespace std;

//회전에 사용할 라디안 값 설정
#define RADIAN 3.1415926535897/180

//행렬의 형태 정의
struct Matrix3x3
{
	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float m[3][3];
	};
	Matrix3x3() {}
	Matrix3x3(float m00, float m01, float m02,
					float m10,  float m11,  float m12, 
					float m20,  float m21, float m22) {}
};
struct Matrix4x4
{
	union
	{
		struct
		{
			float _11,  _12,  _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
	Matrix4x4() {}
	Matrix4x4(	float m00, float m01, float m02, float m03,
						float m10,  float m11,  float m12,  float m13,
						float m20, float m21,  float m22, float m23,
						float m30, float m31,  float m32, float m33) {}
};
struct Matrix1x3
{
	union
	{
		struct
		{
			float _11, _12, _13;
		};
		float m[1][3];
	};
	Matrix1x3() {}
	Matrix1x3(float m00, float m01, float m02) {}
};
struct Matrix1x4
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
		};
		float m[1][4];
	};
	Matrix1x4() {}
	Matrix1x4(float m00, float m01, float m02,float m03) {}
};

// 행렬[3][3] 곱하기, 더하기, 빼기, 스칼라곱
Matrix3x3 Multiple(Matrix3x3 _M3x3_1, Matrix3x3 _M3x3_2) //행렬 곱
{
	Matrix3x3 tempMatrix;
	for (int i = 0; i < 3; i++)//초기화
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	//3x3 * 3x3
	for (int i = 0; i < 3; i++)//행
	{
		for (int j = 0; j < 3; j++)//열
		{
			for (int k = 0; k < 3; k++)
			{
				tempMatrix.m[i][j] += _M3x3_1.m[i][k] * _M3x3_2.m[k][j];
			}
		}
	}
	return tempMatrix;
}
Matrix3x3 Plus(Matrix3x3 _M3x3_1, Matrix3x3 _M3x3_2)//행렬 덧셈
{
	Matrix3x3 tempMatrix;
	for (int i = 0; i < 3; i++)//초기화
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] += _M3x3_1.m[i][j] + _M3x3_2.m[i][j];
		}
	}
	return tempMatrix;
}
Matrix3x3 Minus(Matrix3x3 _M3x3_1, Matrix3x3 _M3x3_2) //행렬 뺄셈
{
	Matrix3x3 tempMatrix;
	for (int i = 0; i < 3; i++)//초기화
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] += _M3x3_1.m[i][j] - _M3x3_2.m[i][j];
		}
	}
	return tempMatrix;
}
Matrix3x3 Scalar_multiply(Matrix3x3 _M3x3, int _num) //행렬 스칼라곱
{
	Matrix3x3 tempMatrix;
	for (int i = 0; i < 3; i++)//초기화
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[i][j] += _M3x3.m[i][j] * _num;
		}
	}
	return tempMatrix;
}
bool IsEqual3x3(Matrix3x3 _M3x3_1, Matrix3x3 _M3x3_2) //행렬 비교
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (_M3x3_1.m[i][j] == _M3x3_2.m[i][j])
			{
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

// 행렬[4][4] 곱하기, 더하기, 빼기, 스칼라곱
Matrix4x4 Multiple(Matrix4x4 _M4x4_1, Matrix4x4 _M4x4_2) //행렬 곱
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	//4x4 * 4x4
	for (int i = 0; i < 4; i++)//행
	{
		for (int j = 0; j < 4; j++)//열
		{
			for (int k = 0; k < 4; k++)
			{
				tempMatrix.m[i][j] += _M4x4_1.m[i][k] * _M4x4_2.m[k][j];
			}
		}
	}
	return tempMatrix;
}
Matrix4x4 Plus(Matrix4x4 _M4x4_1, Matrix4x4 _M4x4_2) //행렬 더하기
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] += _M4x4_1.m[i][j] + _M4x4_2.m[i][j];
		}
	}
	return tempMatrix;
}
Matrix4x4 Minus(Matrix4x4 _M4x4_1, Matrix4x4 _M4x4_2)//행렬 뺄셈
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] += _M4x4_1.m[i][j] - _M4x4_2.m[i][j];
		}
	}
	return tempMatrix;
}
Matrix4x4 Scalar_multiply(Matrix4x4 _M4x4, int _num) //행렬 스칼라곱
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] += _M4x4.m[i][j] * _num;
		}
	}
	return tempMatrix;
}
bool IsEqual4x4(Matrix4x4 _M4x4_1, Matrix4x4 _M4x4_2) //행렬 비교
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (_M4x4_1.m[i][j] == _M4x4_2.m[i][j])
			{
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

//// 행렬[1][n]*// 행렬[n][n] 꼴
Matrix1x3 Multiple(Matrix1x3 _M1x3, Matrix3x3 _M3x3) //행렬 곱 1*3 * 3*3 꼴
{
	Matrix1x3 tempMatrix;
	for (int i = 0; i < 3; i++)//초기화
	{
		tempMatrix.m[0][i] = 0;
	}

	// 1x3 * 3x3
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempMatrix.m[0][i] += _M1x3.m[0][j] * _M3x3.m[j][i];
		}
	}
	return tempMatrix;
}
Matrix1x4 Multiple(Matrix1x4 _M1x4, Matrix4x4 _M4x4) //행렬 곱 1*4 * 4*4 꼴
{
	Matrix1x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		tempMatrix.m[0][i] = 0;
	}

	// 1*4 * 4x4
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[0][i] += _M1x4.m[0][j] * _M4x4.m[j][i];
		}
	}
	return tempMatrix;
}

//행렬 출력
void viewMatrix(Matrix3x3 _M3x3)
{
	cout << "행렬[3][3] : " << endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << setw(5) << _M3x3.m[i][j];
		}
		cout << endl;
	}
	cout << endl;
}
void viewMatrix(Matrix4x4 _M4x4)
{
	cout << "행렬[4][4] : " << endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cout << setw(15) << _M4x4.m[i][j];
		}
		cout << endl<<endl;
	}
	cout << endl;
}
void viewMatrix(Matrix1x3 _M1x3)
{
	cout << "행렬[1][3] : " << endl;
	for (int i = 0; i < 3; i++)
	{
		cout << setw(5) << _M1x3.m[0][i];
	}
	cout << endl << endl;
}
void viewMatrix(Matrix1x4 _M1x4)
{
	cout << "행렬[1][4] : " << endl;
	for (int i = 0; i < 4; i++)
	{
		cout << setw(5) << _M1x4.m[0][i];
	}
	cout << endl << endl;
}


//스케일 행렬 생성
Matrix4x4 MatrixScaling(float scaleX, float scaleY, float scaleZ)
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
	}
	tempMatrix.m[0][0] = scaleX;
	tempMatrix.m[1][1] = scaleY;
	tempMatrix.m[2][2] = scaleZ;
	tempMatrix.m[3][3] = 1;

	return tempMatrix;
}
//x 축 회전 행렬 생성
Matrix4x4 MatrixRotateX(float angle)
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
		tempMatrix.m[i][i] = 1;
	}

	tempMatrix.m[1][1] = cos(angle* RADIAN);
	tempMatrix.m[1][2] = sin(angle* RADIAN);
	tempMatrix.m[2][1] = -sin(angle* RADIAN);
	tempMatrix.m[2][2] = cos(angle* RADIAN);

	return tempMatrix;
}
//y 축 회전 행렬 생성
Matrix4x4 MatrixRotateY(float angle)
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
		tempMatrix.m[i][i] = 1;
	}
	tempMatrix.m[0][0] = cos(angle * RADIAN);
	tempMatrix.m[0][2] = -sin(angle * RADIAN);
	tempMatrix.m[2][0] = sin(angle * RADIAN);
	tempMatrix.m[2][2] = cos(angle * RADIAN);

	return tempMatrix;
}
//z 축 회전 행렬 생성
Matrix4x4 MatrixRotateZ(float angle)
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
		tempMatrix.m[i][i] = 1;
	}
	tempMatrix.m[0][0] = cos(angle * RADIAN);
	tempMatrix.m[0][1] = sin(angle * RADIAN);
	tempMatrix.m[1][0] = -sin(angle * RADIAN);
	tempMatrix.m[1][1] = cos(angle * RADIAN);

	return tempMatrix;
}
//이동 행렬 생성
Matrix4x4 MatrixTranslate(float dx, float dy, float dz)
{
	Matrix4x4 tempMatrix;
	for (int i = 0; i < 4; i++)//초기화
	{
		for (int j = 0; j < 4; j++)
		{
			tempMatrix.m[i][j] = 0;
		}
		tempMatrix.m[i][i] = 1;
	}
	tempMatrix.m[3][0] = dx;
	tempMatrix.m[3][1]  = dy;
	tempMatrix.m[3][2] = dz;

	return tempMatrix;
}