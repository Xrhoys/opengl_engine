#ifndef CMATH_H
#define CMATH_H
#include <math.h>
#include <float.h>

// NOTE: use turns instead of radian values
// https://www.computerenhance.com/p/turns-are-better-than-radians
#define Pi 3.141592f
#define DEFAULT_ROTATION2 V2(0, 0)

union v2
{
    struct
    {
		f32 x;
        f32 y;
    };
    
    struct
    {
        f32 u;
        f32 v;
    };
	
	v2 operator*(f32 scale);
	v2 operator*(v2 array);
	v2 operator+(v2 array);
    v2 operator-(v2 array);
	v2 operator/(v2 array);
	v2 operator/(f32 scale);
	
	void operator+=(v2 array);
	void operator*=(f32 scale);
	void operator*=(v2 array);
    void operator-=(v2 array);
    
};

union v3
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
    
    f32 E[3];
    
    v3 operator+(v3 array);
    v3 operator-(v3 array);
    v3 operator*(f32 scale);
    v3 operator/(f32 scale);
    v3 operator*(v3 array);
    f32 operator[](int index);
    void operator+=(v3 array);
    void operator-=(v3 array);
	void operator*=(f32 scale);
};

// TODO(Ecy): change this to v4 instead
struct v3u8
{
	u8 x;
	u8 y;
	u8 z;
	u8 pad;
};

union v4
{
    struct
    {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    
    struct
    {
        u32 R;
        u32 G;
        u32 B;
        u32 A;
    };
    
    f32 E[4];
    
    v4 operator+(v4 vector);
    v4 operator-(v4 vector);
    v4 operator*(f32 scalar);
    v4 operator*(v4 vector);
    f32& operator[](u32 index);
};

union q4
{
	struct
	{
		f32 w;
		f32 x;
		f32 y;
		f32 z;
	};
	
	f32 E[4];
	
	q4 operator+(q4 quat);
	q4 operator-(q4 quat);
	q4 operator*(q4 quat);
	q4 operator*(f32 coef);
	f32& operator[](u32 index);
};

union matrix
{
	f32 v[16];
	f32 R[4][4];
    v4 E[4];
    struct
    {
        v4 a1;
        v4 a2;
        v4 a3;
        v4 a4;
    };
    
    matrix operator*(matrix &mat);
    matrix operator*=(matrix &mat);
    
    v4& operator[](u32 index);
};

inline v2
V2(f32 x, f32 y)
{
    v2 Result;
    Result.x = x;
    Result.y = y;
    
    return Result;
}

inline v2
v2::operator+(v2 a)
{
	v2 res = { x + a.x, y + a.y };
	
	return res;
}

inline void
v2::operator+=(v2 a)
{
	x += a.x;
	y += a.y;
}

inline v2
v2::operator-(v2 a)
{
	v2 res = { x - a.x, y - a.y };
	
	return res;
}

inline v2
v2::operator/(v2 a)
{
	v2 res = { x / a.x, y / a.y };
	
	return res;
}

inline v2
v2::operator/(f32 scale)
{
	v2 res = { x / scale, y / scale };
	
	return res;
}

inline void
v2::operator-=(v2 a)
{
	x -= a.x;
	y -= a.y;
}


inline v2
v2::operator*(v2 scale)
{
	v2 res = { x * scale.x, y * scale.y };
	
	return res;
}

inline v2
v2::operator*(f32 scale)
{
	v2 res = { x * scale, y * scale };
	
	return res;
}

inline v2
operator*(f32 scale, v2 v)
{
	v2 res = { v.x * scale, v.y * scale };
	return res;
}

inline void
v2::operator*=(f32 scale)
{
	x *= scale;
	y *= scale;
}

inline void
v2::operator*=(v2 array)
{
	x *= array.x;
	y *= array.y;
}

inline f32
Norm(v2 *vector)
{
    return sqrt(vector->x*vector->x + vector->y*vector->y);
}

inline f32
Norm(f32 x, f32 y)
{
    return sqrt(x*x + y*y);
}

inline v2
Normalize(f32 x, f32 y)
{
    v2 Result;
    f32 lengthSquare = x*x + y*y;
    
    // NOTE: fast square root / intrinsics
    f32 length = sqrt(lengthSquare);
    Result.x      = x/length;
    Result.y      = y/length;
    
    return Result;
}

inline v2
Normalize(v2 v)
{
    v2 res = V2(0, 0);
	
	f32 length = Norm(&v);
	if(length == 0.f)
	{
		return res;
	}
	
    res.x = v.x / length;
    res.y = v.y / length;
    
    return res;
}

inline v2
Tangeant(v2 a)
{
	return V2(-a.y, a.x);
}

inline f32
Dot(v2 *vector1, v2 *vector2)
{
    f32 Result = vector1->x*vector2->x + 
        vector1->y*vector2->y;
    
    return Result;
}

inline f32
Length(v2 vector)
{
	return sqrt(vector.x*vector.x + vector.y*vector.y);
}

inline v2
Reflect(v2 v, v2 n)
{
	return v - n * (2 * Dot(&v, &n));
}

inline f32
CrossProduct(v2 a, v2 b)
{
	return a.x * b.y - a.y * b.x;
};

inline f32
Dot(v3 *vector1, v3 *vector2)
{
    f32 Result = vector1->x*vector2->x + vector1->y*vector2->y + vector1->z*vector2->z;
    
    return Result;
}

inline v3
V3(f32 x, f32 y, f32 z)
{
    v3 Result;
    Result.x = x;
    Result.y = y;
    Result.z = z;
    
    return Result;
}

inline v3 
v3::operator+(v3 array) 
{
    v3 Result;
    Result.x = x + array.x;
    Result.y = y + array.y;
    Result.z = z + array.z;
    
    return Result;
}

inline void 
v3::operator+=(v3 array) 
{
    x = x + array.x;
    y = y + array.y;
    z = z + array.z;
}

inline v3 
v3::operator-(v3 vector)
{
    v3 Result;
    Result.x = x - vector.x;
    Result.y = y - vector.y;
    Result.z = z - vector.z;
    
    return Result;
}

inline void 
v3::operator-=(v3 array) 
{
    x = x - array.x;
    y = y - array.y;
    z = z - array.z;
}

inline v3 
v3::operator*(f32 scale)
{
    v3 Result;
    Result.x = scale*x;
    Result.y = scale*y;
    Result.z = scale*z;
    
    return Result;
}

inline void 
v3::operator*=(f32 scale)
{
    x = scale*x;
    y = scale*y;
    z = scale*z;
}

inline v3 
v3::operator/(f32 scale)
{
    Assert(scale > 0);
    
    v3 Result;
    Result.x = x/scale;
    Result.y = y/scale;
    Result.z = z/scale;
    
    return Result;
}

inline v3
v3::operator*(v3 array)
{
    v3 Result;
    // NOTE: cross-product
    Result.x = y*array.z - z*array.y;
    Result.y = z*array.x - x*array.z;
    Result.z = x*array.y - y*array.x;
    
    return Result;
}

inline f32
v3::operator[](int index)
{
    return E[index];
}

inline f32
Length(v3 vector)
{
	return sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

inline v3
Normalize(v3 *vector)
{
    v3 Result = {};
    f32 lengthSquare = vector->x*vector->x + 
        vector->y*vector->y + vector->z*vector->z;
    
    // NOTE: fast square root / intrinsics
    f32 length = sqrt(lengthSquare);
	if(fabs(length) < FLT_EPSILON)
	{
		return Result;
	}
	
    Result.x      = vector->x/length;
    Result.y      = vector->y/length;
    Result.z      = vector->z/length;
    
    return Result;
}

inline v3
NormalizeOrZero(v3 *vector)
{
    v3 Result = {};
    f32 lengthSquare = vector->x*vector->x + 
        vector->y*vector->y + vector->z*vector->z;
    
    // NOTE: fast square root / intrinsics
    f32 length = sqrt(lengthSquare);
	
	if(length == 0.0f)
	{
		return Result;
	}
	
    Result.x      = vector->x/length;
    Result.y      = vector->y/length;
    Result.z      = vector->z/length;
    
    return Result;
}

inline v3
Normalize(f32 x, f32 y, f32 z)
{
    v3 Result;
    f32 lengthSquare = x*x + y*y + z*z;
    
    // NOTE: fast square root / intrinsics
    f32 length = sqrt(lengthSquare);
    Result.x      = x/length;
    Result.y      = y/length;
    Result.z      = z/length;
    
    return Result;
}

inline v3
Reflect(v3 v, v3 n)
{
	return v - n * (2 * Dot(&v, &n));
}

inline b32
CompareGT(v3 *A, v3 *B)
{
    // NOTE: idea for branchless, A - B then applies 3 masks to the sign bit
    return A->x > B->x && A->y > B->y && A->z > B->z;
}

inline b32
CompareLT(v3 *A, v3 *B)
{
    return A->x < B->x && A->y < B->y && A->z < B->z;
}

inline v4
V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 vector;
    vector.x = x;
    vector.y = y;
    vector.z = z;
    vector.w = w;
    
    return vector;
}

inline v4
V4(v3 *array, f32 w)
{
    v4 vector;
    vector.x = array->x;
    vector.y = array->y;
    vector.z = array->z;
    vector.w = w;
    
    return vector;
}

inline v4 
v4::operator*(f32 scale)
{
    v4 Result;
    
    Result.x = x*scale;
    Result.y = y*scale;
    Result.z = z*scale;
    Result.w = w*scale;
    
    return Result;
}

inline v4
v4::operator*(v4 vector)
{
    v4 Result;
    // NOTE: NOT cross nor dot product, component x component
    Result.x = x*vector.x;
    Result.y = y*vector.y;
    Result.z = z*vector.z;
    Result.w = w*vector.w;
    
    return Result;
}

inline v4 
v4::operator+(v4 vector)
{
    v4 Result;
    
    Result.x = x + vector.x;
    Result.y = y + vector.y;
    Result.z = z + vector.z;
    Result.w = w + vector.w;
    
    return Result;
}

inline v4 
v4::operator-(v4 vector)
{
    v4 Result;
    
    Result.x = x - vector.x;
    Result.y = y - vector.y;
    Result.z = z - vector.z;
    Result.w = w - vector.w;
    
    return Result;
}

inline f32&
v4::operator[](u32 Index)
{
    return E[Index];
}

inline v4
Normalize(v4 *vector)
{
    v4 Result;
    f32 lengthSquare = vector->x*vector->x + vector->y*vector->y + vector->z*vector->z + vector->w*vector->w;
    
    f32 length = sqrt(lengthSquare);
    Result.x = vector->x/length;
    Result.y = vector->y/length;
    Result.z = vector->z/length;
    Result.w = vector->w/length;
    
    return Result;
}

inline v4&
matrix::operator[](u32 Index)
{
    return E[Index];
}

inline matrix
matrixzero()
{
    matrix Result = {};
    return Result;
}

inline matrix
MatrixIdentity()
{
    matrix Result = {};
    
    Result[0][0] = 1.0f;
    Result[1][1] = 1.0f;
    Result[2][2] = 1.0f;
    Result[3][3] = 1.0f;
    
    return Result;
}

inline matrix
Transpose(matrix *mat)
{
	matrix Result = 
	{
		mat->E[0][0], mat->E[1][0], mat->E[2][0], mat->E[3][0], 
		mat->E[0][1], mat->E[1][1], mat->E[2][1], mat->E[3][1], 
		mat->E[0][2], mat->E[1][2], mat->E[2][2], mat->E[3][2], 
		mat->E[0][3], mat->E[1][3], mat->E[2][3], mat->E[3][3],
	};
	
    return Result;
}

inline matrix
matrix::operator*(matrix &mat)
{
    // TODO: Change to pointer instead
	// SMID AVX
    matrix Result;
    
    f32 x11 = E[0][0]*mat[0][0] + E[0][1]*mat[1][0] + E[0][2]*mat[2][0] + E[0][3]*mat[3][0];
    f32 x12 = E[0][0]*mat[0][1] + E[0][1]*mat[1][1] + E[0][2]*mat[2][1] + E[0][3]*mat[3][1];
    f32 x13 = E[0][0]*mat[0][2] + E[0][1]*mat[1][2] + E[0][2]*mat[2][2] + E[0][3]*mat[3][2];
    f32 x14 = E[0][0]*mat[0][3] + E[0][1]*mat[1][3] + E[0][2]*mat[2][3] + E[0][3]*mat[3][3];
    f32 x21 = E[1][0]*mat[0][0] + E[1][1]*mat[1][0] + E[1][2]*mat[2][0] + E[1][3]*mat[3][0];
    f32 x22 = E[1][0]*mat[0][1] + E[1][1]*mat[1][1] + E[1][2]*mat[2][1] + E[1][3]*mat[3][1];
    f32 x23 = E[1][0]*mat[0][2] + E[1][1]*mat[1][2] + E[1][2]*mat[2][2] + E[1][3]*mat[3][2];
    f32 x24 = E[1][0]*mat[0][3] + E[1][1]*mat[1][3] + E[1][2]*mat[2][3] + E[1][3]*mat[3][3];
    f32 x31 = E[2][0]*mat[0][0] + E[2][1]*mat[1][0] + E[2][2]*mat[2][0] + E[2][3]*mat[3][0];
    f32 x32 = E[2][0]*mat[0][1] + E[2][1]*mat[1][1] + E[2][2]*mat[2][1] + E[2][3]*mat[3][1];
    f32 x33 = E[2][0]*mat[0][2] + E[2][1]*mat[1][2] + E[2][2]*mat[2][2] + E[2][3]*mat[3][2];
    f32 x34 = E[2][0]*mat[0][3] + E[2][1]*mat[1][3] + E[2][2]*mat[2][3] + E[2][3]*mat[3][3];
    f32 x41 = E[3][0]*mat[0][0] + E[3][1]*mat[1][0] + E[3][2]*mat[2][0] + E[3][3]*mat[3][0];
    f32 x42 = E[3][0]*mat[0][1] + E[3][1]*mat[1][1] + E[3][2]*mat[2][1] + E[3][3]*mat[3][1];
    f32 x43 = E[3][0]*mat[0][2] + E[3][1]*mat[1][2] + E[3][2]*mat[2][2] + E[3][3]*mat[3][2];
    f32 x44 = E[3][0]*mat[0][3] + E[3][1]*mat[1][3] + E[3][2]*mat[2][3] + E[3][3]*mat[3][3];
    
    Result[0] = V4( x11, x12, x13, x14 );
    Result[1] = V4( x21, x22, x23, x24 );
    Result[2] = V4( x31, x32, x33, x34 );
    Result[3] = V4( x41, x42, x43, x44 );
    
    return Result;
}

inline matrix 
matrix::operator*=(matrix &mat)
{
    (*this) = (*this) * mat;
	return *this;
}

inline matrix
MatrixScale(f32 x, f32 y, f32 z)
{
    matrix mat = 
	{
		x,    0.0f, 0.0f, 0.0f,
		0.0f,    y, 0.0f, 0.0f,
		0.0f, 0.0f,   z,  0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
    
    return mat;
}

inline void
Scaling(matrix *mat, v3 *scale)
{
    matrix Scalematrix;
    
    Scalematrix[0] = V4( scale->x,     0.0f,     0.0f, 0.0f );
    Scalematrix[1] = V4(     0.0f, scale->y,     0.0f, 0.0f );
    Scalematrix[2] = V4(     0.0f,     0.0f, scale->z, 0.0f );
    Scalematrix[3] = V4(     0.0f,     0.0f,     0.0f, 1.0f );
    
    *mat = Scalematrix * *mat;
}

inline void
Rotationx(matrix *mat, f32 AngleRadian)
{
    matrix Rotationmatrix;
    
    Rotationmatrix[0] = V4( 1.0f,             0.0f,              0.0f, 0.0f );
    Rotationmatrix[1] = V4( 0.0f,  cos(AngleRadian), sin(AngleRadian), 0.0f );
    Rotationmatrix[2] = V4( 0.0f, -sin(AngleRadian), cos(AngleRadian), 0.0f );
    Rotationmatrix[3] = V4( 0.0f,              0.0f,             0.0f, 1.0f );
    
    *mat = *mat * Rotationmatrix;
}

inline void
Rotationy(matrix *mat, f32 AngleRadian)
{
    matrix Rotationmatrix;
    
    f32 cosA = cos(AngleRadian);
    f32 sinA = sin(AngleRadian);
    
    Rotationmatrix[0] = V4(  cosA, 0.0f, sinA, 0.0f );
    Rotationmatrix[1] = V4(  0.0f, 1.0f, 0.0f, 0.0f );
    Rotationmatrix[2] = V4( -sinA, 0.0f, cosA, 0.0f );
    Rotationmatrix[3] = V4(  0.0f, 0.0f, 0.0f, 1.0f );
    
    *mat = *mat * Rotationmatrix;
}

inline void
Rotationy(matrix *mat, f32 angleCos, f32 angleSin)
{
    matrix Rotationmatrix;
    
    Rotationmatrix[0] = V4( angleCos, 0.0f, -angleSin, 0.0f );
    Rotationmatrix[1] = V4(     0.0f, 1.0f,      0.0f, 0.0f );
    Rotationmatrix[2] = V4( angleSin, 0.0f,  angleCos, 0.0f );
    Rotationmatrix[3] = V4(     0.0f, 0.0f,      0.0f, 1.0f );
    
    *mat = *mat * Rotationmatrix;
}

inline void
Rotationz(matrix *mat, f32 AngleRadian)
{
    matrix Rotationmatrix;
    
    Rotationmatrix[0] = V4(  cos(AngleRadian), sin(AngleRadian), 0.0f, 0.0f );
    Rotationmatrix[1] = V4( -sin(AngleRadian), cos(AngleRadian), 0.0f, 0.0f );
    Rotationmatrix[2] = V4(              0.0f,             0.0f, 1.0f, 0.0f );
    Rotationmatrix[3] = V4(              0.0f,             0.0f, 0.0f, 1.0f );
    
    *mat = *mat * Rotationmatrix;
}

inline void
RotationAxisMat(matrix *mat, v3 Axis, f32 AngleRadian)
{
    matrix Rotationmatrix;
}

inline matrix
MatrixTranslate(f32 Tx, f32 Ty, f32 Tz)
{
    matrix mat = 
	{
		1.0f, 0.0f, 0.0f, Tx,
		0.0f, 1.0f, 0.0f, Ty,
		0.0f, 0.0f, 1.0f, Tz,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
    
    return mat;
}

inline void
Translation(matrix *mat, v3 *position)
{
    matrix Translation;
    
    Translation[0] = V4( 1.0f, 0.0f, 0.0f, position->x );
    Translation[1] = V4( 0.0f, 1.0f, 0.0f, position->y );
    Translation[2] = V4( 0.0f, 0.0f, 1.0f, position->z );
    Translation[3] = V4( 0.0f, 0.0f, 0.0f, 1.0f );
    
    *mat = Translation * *mat;
}

inline v4
ProductmatrixByV4(matrix *mat, v4 *vector)
{
    v4 Result;
    Result.x = mat->E[0][0]*vector->x + mat->E[0][1]*vector->y + mat->E[0][2]*vector->z + mat->E[0][3]*vector->w;
    Result.y = mat->E[1][0]*vector->x + mat->E[1][1]*vector->y + mat->E[1][2]*vector->z + mat->E[1][3]*vector->w;
    Result.z = mat->E[2][0]*vector->x + mat->E[2][1]*vector->y + mat->E[2][2]*vector->z + mat->E[2][3]*vector->w;
    Result.w = mat->E[3][0]*vector->x + mat->E[3][1]*vector->y + mat->E[3][2]*vector->z + mat->E[3][3]*vector->w;
    
    return Result;
}

inline v3
TransformCoordV3(matrix *mat, v3 *vector)
{
    v4 Result;
    v4 tempVector = V4( vector->x, vector->y, vector->z, 1.0);
    Result = ProductmatrixByV4(mat, &tempVector);
    
    v3 ReturnedResult = { Result.x/Result.w, Result.y/Result.w, Result.z/Result.w };
    return ReturnedResult;
}

inline v2
TransformCoordV2(matrix *mat, v2 vector)
{
    v4 result;
    v4 tempVector = V4( vector.x, vector.y, 0.f, 1.f);
    result = ProductmatrixByV4(mat, &tempVector);
    
    return V2(result.x, result.y) / result.w;
}

inline f32
GetDistanceSquareV3(v3 *vector)
{
    f32 Result = vector->x*vector->x + vector->y*vector->y + vector->z*vector->z;
    return Result;
}

inline f32
Determinant3x3(v3 mat[3])
{
    f32 A = mat[0].x;
    f32 B = mat[0].y;
    f32 C = mat[0].z;
    f32 D = mat[1].x;
    f32 E = mat[1].y;
    f32 F = mat[1].z;
    f32 G = mat[2].x;
    f32 H = mat[2].y;
    f32 I = mat[2].z;
    return A*E*I + B*F*G + C*D*H - C*E*G - B*D*I - A*F*H;
}

inline matrix
Invmatrix(f32 *Determinant, matrix *mat)
{
    v3 TempMat[3] = 
    {
        V3(mat->E[1][1], mat->E[1][2], mat->E[1][3]),
        V3(mat->E[2][1], mat->E[2][2], mat->E[2][3]),
        V3(mat->E[3][1], mat->E[3][2], mat->E[3][3])
    };
    f32 A = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][2]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][2]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][2]);
    f32 B = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][3]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][3]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][3]);
    f32 C = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][2]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][2]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][2]);
    f32 D = Determinant3x3(TempMat);
    
    // NOTE: Case det = 0
    
    *Determinant = mat->E[0][0]*A - mat->E[0][1]*B + mat->E[0][3]*C - mat->E[0][4]*D;
    
    matrix Result;
    if(*Determinant == 0)
    {
        return Result;
    }
    
    f32 DetInv = 1.0f / *Determinant;
    
    Result = Transpose(mat);
    Result[0] = Result[0] * DetInv;
    Result[1] = Result[1] * DetInv;
    Result[2] = Result[2] * DetInv;
    Result[3] = Result[3] * DetInv;
    
    return Result;
}

inline matrix
MatrixInverse(matrix *mat)
{
#if 0
	// TODO(Ecy): this matrix inverse is wrong, copy this one instead
	// https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
	v3 TempMat[3] = 
    {
        V3(mat->E[1][1], mat->E[1][2], mat->E[1][3]),
        V3(mat->E[2][1], mat->E[2][2], mat->E[2][3]),
        V3(mat->E[3][1], mat->E[3][2], mat->E[3][3])
    };
    f32 A = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][1], mat->E[1][2], mat->E[1][3]);
    TempMat[1] = V3(mat->E[2][1], mat->E[2][2], mat->E[2][3]);
    TempMat[2] = V3(mat->E[3][1], mat->E[3][2], mat->E[3][3]);
    f32 B = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][3]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][3]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][3]);
    f32 C = Determinant3x3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][2]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][2]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][2]);
    f32 D = Determinant3x3(TempMat);
	
    f32 Determinant = mat->E[0][0]*A - mat->E[0][1]*B + mat->E[0][2]*C - mat->E[0][3]*D;
    
    // NOTE: Case det = 0
    if(fabs(Determinant) < FLT_EPSILON)
    {
        return Result;
    }
    
    f32 DetInv = 1.0f / Determinant;
    
    Result = Transpose(mat);
    Result[0] = Result.E[0] * DetInv;
    Result[1] = Result.E[1] * DetInv;
    Result[2] = Result.E[2] * DetInv;
    Result[3] = Result.E[3] * DetInv;
#endif
	
	matrix Result = {};
    
	f32 s0 = mat->E[0][0] * mat->E[1][1] - mat->E[1][0] * mat->E[0][1];
	f32 s1 = mat->E[0][0] * mat->E[1][2] - mat->E[1][0] * mat->E[0][2];
	f32 s2 = mat->E[0][0] * mat->E[1][3] - mat->E[1][0] * mat->E[0][3];
	f32 s3 = mat->E[0][1] * mat->E[1][2] - mat->E[1][1] * mat->E[0][2];
	f32 s4 = mat->E[0][2] * mat->E[1][3] - mat->E[1][1] * mat->E[0][3];
	f32 s5 = mat->E[0][3] * mat->E[1][3] - mat->E[1][2] * mat->E[0][3];
	
	f32 c5 = mat->E[2][2] * mat->E[3][3] - mat->E[3][2] * mat->E[2][3];
	f32 c4 = mat->E[2][1] * mat->E[3][3] - mat->E[3][1] * mat->E[2][3];
	f32 c3 = mat->E[2][1] * mat->E[3][2] - mat->E[3][1] * mat->E[2][2];
	f32 c2 = mat->E[2][0] * mat->E[3][3] - mat->E[3][0] * mat->E[2][3];
	f32 c1 = mat->E[2][0] * mat->E[3][2] - mat->E[3][0] * mat->E[2][2];
	f32 c0 = mat->E[2][0] * mat->E[3][1] - mat->E[3][0] * mat->E[2][1];
	
	f32 invdet = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
	
	Result.E[0][0] = (mat->E[1][1] * c5 - mat->E[1][2] * c4 + mat->E[1][3] * c3) * invdet;
    Result.E[0][1] = (-mat->E[0][1] * c5 + mat->E[0][2] * c4 - mat->E[0][3] * c3) * invdet;
    Result.E[0][2] = (mat->E[3][1] * s5 - mat->E[3][2] * s4 + mat->E[3][3] * s3) * invdet;
    Result.E[0][3] = (-mat->E[2][1] * s5 + mat->E[2][2] * s4 - mat->E[2][3] * s3) * invdet;
	
    Result.E[1][0] = (-mat->E[1][0] * c5 + mat->E[1][2] * c2 - mat->E[1][3] * c1) * invdet;
    Result.E[1][1] = (mat->E[0][0] * c5 - mat->E[0][2] * c2 + mat->E[0][3] * c1) * invdet;
    Result.E[1][2] = (-mat->E[3][0] * s5 + mat->E[3][2] * s2 - mat->E[3][3] * s1) * invdet;
    Result.E[1][3] = (mat->E[2][0] * s5 - mat->E[2][2] * s2 + mat->E[2][3] * s1) * invdet;
	
    Result.E[2][0] = (mat->E[1][0] * c4 - mat->E[1][1] * c2 + mat->E[1][3] * c0) * invdet;
    Result.E[2][1] = (-mat->E[0][0] * c4 + mat->E[0][1] * c2 - mat->E[0][3] * c0) * invdet;
    Result.E[2][2] = (mat->E[3][0] * s4 - mat->E[3][1] * s2 + mat->E[3][3] * s0) * invdet;
    Result.E[2][3] = (-mat->E[2][0] * s4 + mat->E[2][1] * s2 - mat->E[2][3] * s0) * invdet;
	
    Result.E[3][0] = (-mat->E[1][0] * c3 + mat->E[1][1] * c1 - mat->E[1][2] * c0) * invdet;
    Result.E[3][1] = (mat->E[0][0] * c3 - mat->E[0][1] * c1 + mat->E[0][2] * c0) * invdet;
    Result.E[3][2] = (-mat->E[3][0] * s3 + mat->E[3][1] * s1 - mat->E[3][2] * s0) * invdet;
    Result.E[3][3] = (mat->E[2][0] * s3 - mat->E[2][1] * s1 + mat->E[2][2] * s0) * invdet;
	
	return Result; 
}

inline v4
v4Reciprocal(v4 *vect)
{
    v4 Result;
    
    Result.x = 1.0f / vect->x;
    Result.y = 1.0f / vect->y;
    Result.z = 1.0f / vect->z;
    Result.w = 1.0f / vect->w;
    
    return Result;
}

// MAx and MIN for f32 to be calls in intrinsics 
// for now just plain if statements
inline f32
Min(f32 A, f32 B)
{
    if(A < B) return A;
    
    return B;
}

inline f32
Max(f32 A, f32 B)
{
    if(A < B) return B;
    
    return A;
}

inline f32
NormSquared(v3 *vector)
{
    return vector->x*vector->x + vector->y*vector->y + vector->z*vector->z;
}

inline f32
Norm(v3 *vector)
{
    return sqrt(vector->x*vector->x + vector->y*vector->y + vector->z*vector->z);
}

inline f32
Norm(f32 x, f32 y, f32 z)
{
    return sqrt(x*x + y*y + z*z);
}

inline v3
Lerp(v3 init, v3 final, f64 t)
{
    return init*(1 - t) + final*t;
};

inline v4
Lerp(v4 init, v4 final, f32 t)
{
    return init*(1 - t) + final*t;
};

inline v3
Wedge(v3 *a, v3 *b)
{
    v3 u = *a;
    v3 v = *b;
    
    v3 result = 
    {
        u[0]*v[1] - u[1]*v[0], // xy
		u[0]*v[2] - u[2]*v[0], // xz
		u[1]*v[2] - u[2]*v[1]  // yz
    };
    
    return result;
}

inline v3
Cross(v3 *a, v3 *b)
{
    v3 u = *a;
    v3 v = *b;
    
    v3 result = 
    {
        u[1]*v[2] - u[2]*v[1], 
		u[2]*v[0] - u[0]*v[2], 
		u[0]*v[1] - u[1]*v[0],
    };
    
    return result;
}

inline v3
Scale(v3 v, v3 scale)
{
	v3 result = { v.x * scale.x, v.y * scale.y, v.z * scale.z };
	return result;
}

// Generic transform matrix, generate from translation, rotation and scaling 
inline void
Transform(matrix* mtx, v3* pos, v3* rot, v3* scale)
{
	// Lazy implementation and doesn't work 
	Scaling(mtx, scale);
	Translation(mtx, pos);
	Rotationx(mtx, rot->x);
	Rotationy(mtx, rot->y);
	Rotationz(mtx, rot->z);
}

inline f32
dtof(f64 value)
{
	return 0;
}

////////////////////////////////
// Quaternion API 
////////////////////////////////

inline q4
q4::operator+(q4 quat)
{
	q4 result;
	
	result.w = w + quat.w;
	result.x = x + quat.x;
	result.y = y + quat.y;
	result.z = z + quat.z;
	
	return result;
}

inline q4
q4::operator-(q4 quat)
{
	q4 result;
	
	result.w = w - quat.w;
	result.x = x - quat.x;
	result.y = y - quat.y;
	result.z = z - quat.z;
	
	return result;
}

inline q4
q4::operator*(q4 quat)
{
	// NOTE: AVX SMID this :D
	// 16 muls ( 8 / 8 )
	// Ideally, if an instruction exists for flipping the sign + mul
	// Then the last 4 adds can be done in 8 wide
	q4 result;
	
	f32 w1w2 = w * quat.w;
	f32 x1x2 = - x * quat.x;
	f32 y1y2 = - y * quat.y;
	f32 z1z2 = - z * quat.z;
	
	f32 w1x2 = w * quat.x;
	f32 x1w2 = x * quat.w;
	f32 y1z2 = y * quat.z;
	f32 z1y2 = - z * quat.y;
	
	f32 w1y2 = w * quat.y;
	f32 x1z2 = - x * quat.z;
	f32 y1w2 = y * quat.w;
	f32 z1x2 = z * quat.x;
	
	f32 w1z2 = w * quat.z;
	f32 x1y2 = x * quat.y;
	f32 y1x2 = - y * quat.x;
	f32 z1w2 = z * quat.w;
	
	result.w = w1w2 + x1x2 + y1y2 + z1z2;
	result.x = w1x2 + x1w2 + y1z2 + z1y2;
	result.y = w1y2 + x1z2 + y1w2 + z1x2;
	result.z = w1z2 + x1y2 + y1x2 + z1w2;
	
	return result;
}

inline q4
q4::operator*(f32 scalar)
{
	q4 result;
	
	result.w = w * scalar;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	
	return result;
}

inline f32&
q4::operator[](u32 index)
{
	return E[index];
}

inline q4
Q4(f32 x, f32 y, f32 w, f32 z)
{
	q4 result;
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	
	return result;
}

inline q4
Q4Negate(q4 rot)
{
	q4 result;
	result.x = -rot.x;
	result.y = -rot.y;
	result.z = -rot.z;
	result.w = -rot.w;
	
	return result;
}

inline q4
Q4Normalize(q4 *quat)
{
	q4 result;
    f32 lengthSquare = quat->x*quat->x + quat->y*quat->y + quat->z*quat->z + quat->w*quat->w;
    
    f32 length = sqrt(lengthSquare);
    result.w = quat->w/length;
    result.x = quat->x/length;
    result.y = quat->y/length;
    result.z = quat->z/length;
    
	return result;
}

inline f32
Q4Dot(q4 a, q4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline q4
Q4Generate(f32 angle, v3 *axis)
{
	q4 result;
	
	f32 t = sin(angle / 2.0f);
	
	result.w = cos(angle / 2.0f);
	result.x = axis->x * t;
	result.y = axis->y * t;
	result.z = axis->z * t;
	
	return Q4Normalize(&result);
}

inline q4
Q4Identity()
{
	v3 axis = V3(0.0f, 0.0f, 0.0f);
	return Q4Generate(0.0f, &axis);
}

inline q4
Q4QuarterTurn(v3 *axis)
{
	return Q4Generate(Pi/2.0f, axis);
}

inline q4
Q4Turn45(v3 *axis)
{
	return Q4Generate(Pi/4.0f, axis);
}

inline q4
Q4Turn30(v3 *axis)
{
	return Q4Generate(Pi/6.0f, axis);
}

inline q4
Q4Turn60(v3 *axis)
{
	return Q4Generate(Pi/3.0f, axis);
}

inline q4
Q4HalfTurn(v3 *axis)
{
	return Q4Generate(Pi, axis);
}


inline q4
Q4FullTurn(v3 *axis)
{
	return Q4Generate(2.0f * Pi, axis);
}

internal matrix
GenerateTransform(v3 *pos, v3 *scale, q4 *rot)
{
	matrix mat = {};
	
	f32 x2 = rot->x * rot->x;
	f32 y2 = rot->y * rot->y;
	f32 z2 = rot->z * rot->z;
	
	f32 xy = rot->x * rot->y;
	f32 xz = rot->x * rot->z;
	f32 wz = rot->w * rot->z;
	f32 wx = rot->w * rot->x;
	f32 wy = rot->w * rot->y;
	f32 yz = rot->y * rot->z;
	
	// will be an interesting task to SMID :D
	mat.v[0]  = scale->x * ( 1.0f - 2*y2 - 2*z2 );
	mat.v[1]  = scale->x * ( 2*xy - 2*wz );
	mat.v[2]  = scale->x * ( 2*xz + 2*wy );
	mat.v[3]  = pos->x;
	
	mat.v[4]  = scale->y * ( 2*xy + 2*wz );
	mat.v[5]  = scale->y * ( 1.0f - 2*x2 - 2*z2 );
	mat.v[6]  = scale->y * ( 2*yz - 2*wx );
	mat.v[7]  = pos->y;
	
	mat.v[8]  = scale->z * ( 2*xz - 2*wy );
	mat.v[9]  = scale->z * ( 2*yz + 2*wx );
	mat.v[10] = scale->z * ( 1.0f - 2*x2 - 2*y2 );
	mat.v[11] = pos->z;
	
	mat.v[12] = 0.0f;
	mat.v[13] = 0.0f;
	mat.v[14] = 0.0f;
	mat.v[15] = 1.0f;
	
	return mat;
}

inline f32
Q4DotProduct(q4 p, q4 q)
{
	return p.w * q.w + p.x * q.x + p.y * q.y + p.z * q.z;
}

// NOTE(Ecy): http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
// TODO(Ecy): implement nlerp
#define DOT_THRESHOLD 0.9995f
inline q4
SLerp(q4 p, q4 q, f64 t)
{
	f32 dot = Q4DotProduct(p, q);
	if(dot > DOT_THRESHOLD)
	{
		q4 result = p + ((q - p) * t);
		Q4Normalize(&result);
		return result;
	}
	
	Clamp(dot, -1, 1);
	f32 theta0 = acos(dot);
	f32 theta1 = theta0 * t;
	
	q4 result = q - p*dot;
	Q4Normalize(&result);
	
    return p*cos(theta1) + result*sin(theta1);
}

////////////////////////////////
// Matrix 3x3 API
////////////////////////////////
union matrix3
{
	f32 E_[9];
	struct
	{
		f32 m00;
		f32 m01;
		f32 m02;
		f32 m11;
		f32 m12;
		f32 m13;
		f32 m21;
		f32 m22;
		f32 m23;
	};
	
    matrix3 operator+(matrix &mat);
    matrix3 operator+=(matrix &mat);
	matrix3 operator-(matrix &mat);
    matrix3 operator-=(matrix &mat);  
    matrix3 operator*(matrix &mat);
    matrix3 operator*=(matrix &mat);
    matrix3 operator*(f32 coef);
    matrix3 operator*=(f32 coef);
};

inline matrix3
matrix3_Transform(v2 position, v2 rotation, v2 scale)
{
	matrix3 res = 
	{
		scale.x, 0, position.x,
		0, scale.y, position.y,
		0, 0, 1,
	};
	return res;
}

inline matrix
GenerateTransform3(v2 position, v2 rotation, v2 scale)
{
	matrix res = 
	{
		scale.x, 0, position.x, 0,
		0, scale.y, position.y, 0,
		0, 0, 0, 0,
		0, 0, 0, 1,
	};
	
	return res;
}

inline i32
RoundF32ToI32(f32 real)
{
	i32 result = (i32)(real + .5f);
	
	return result;
}

#endif //CMATH_H
