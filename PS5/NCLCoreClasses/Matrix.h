/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Maths.h"
#include "Vector.h"

namespace NCL::Maths {

    template <typename T, uint32_t r, uint32_t c>
    struct NewMatrix    {
        T array[c][r];

        NewMatrix() {
            for (int i = 0; i < c; ++i) {
                for (int j = 0; j < r; ++j) {
                    T a = (i == j) ? T(1) : T(0);
                    array[i][j] = a;
                }
            }
        }

        constexpr NewVector<T, c> GetRow(uint32_t rr) const {
            NewVector<T, c> out;

            for (unsigned int cc = 0; cc < c; ++cc) {
                out.array[cc] = (array[rr][cc]);
            }

            return out;
        }

        constexpr NewVector<T, r> GetColumn(uint32_t cc) const {
            NewVector<T, r> out;

            for (unsigned int rr = 0; rr < r; ++rr) {
                out.array[rr] = (array[rr][cc]);
            }

            return out;
        }

        void SetRow(uint32_t rr, const NewVector<T, r>& vec) {
            for (unsigned int cc = 0; cc < c; ++cc) {
                array[cc][rr] = vec[cc];
            }
        }

        void SetColumn(uint32_t cc, const NewVector<T, c>& vec) {
            for (unsigned int rr = 0; rr < r; ++rr) {
                array[cc][rr] = vec[rr];
            }
        }
    };

    using Matrix2 = NewMatrix<float, 2, 2>;
    using Matrix3 = NewMatrix<float, 3, 3>;
    using Matrix4 = NewMatrix<float, 4, 4>;

    using Matrix2i = NewMatrix<int, 2, 2>;
    using Matrix3i = NewMatrix<int, 3, 3>;
    using Matrix4i = NewMatrix<int, 4, 4>;

    using Matrix2d = NewMatrix<double, 2, 2>;
    using Matrix3d = NewMatrix<double, 3, 3>;
    using Matrix4d = NewMatrix<double, 4, 4>;

    template <typename T, uint32_t r, uint32_t c>
    constexpr NewMatrix<T,r,c> operator*(const NewMatrix<T, r, c>& a, const NewMatrix<T, r, c>& b) {
        NewMatrix<T, r, c> out;
        for (unsigned int cc = 0; cc < c; ++cc) {
            for (unsigned int rr = 0; rr < r; ++rr) {
                out.array[cc][rr] = T(0);
                for (unsigned int i = 0; i < r; ++i) {
                    out.array[cc][rr] += a.array[i][rr] * b.array[cc][i];
                }
            }
        }
        return out;
    }

    template <typename T, uint32_t r, uint32_t c>
    NewVector<T, c> operator*(const NewMatrix<T, r, c>& mat, const NewVector<T, c>& v) {
        NewVector<T, c> vec;

        for (int rr = 0; rr < r; ++rr) {
            for (int cc = 0; cc < c; ++cc) {
                vec[rr] += v[cc] * mat.array[cc][rr];
            }
        }

        return vec;
    }

    template <typename T>
    NewVector<T, 3> operator*(const NewMatrix<T, 3, 3>& mat, const NewVector<T, 3>& v) {
        return  NewVector<T, 3>(
            v.x * mat.array[0][0] + v.y * mat.array[1][0] + v.z * mat.array[2][0],
            v.x * mat.array[0][1] + v.y * mat.array[1][1] + v.z * mat.array[2][1],
            v.x * mat.array[0][2] + v.y * mat.array[1][2] + v.z * mat.array[2][2]
        );
    }

    template <typename T>
    NewVector<T, 4> operator*(const NewMatrix<T, 4, 4>& mat, const NewVector<T, 4>&v) {
    	return  NewVector<T, 4>(
    		v.x*mat.array[0][0] + v.y*mat.array[1][0] + v.z*mat.array[2][0]  + v.w * mat.array[3][0] ,
    		v.x*mat.array[0][1] + v.y*mat.array[1][1] + v.z*mat.array[2][1]  + v.w * mat.array[3][1] ,
    		v.x*mat.array[0][2] + v.y*mat.array[1][2] + v.z*mat.array[2][2]  + v.w * mat.array[3][2] ,
    		v.x*mat.array[0][3] + v.y*mat.array[1][3] + v.z*mat.array[2][3]  + v.w * mat.array[3][3] 
    	);
    }

    namespace Matrix {
        template <typename T, uint32_t r, uint32_t c>
        constexpr NewMatrix<T, r, c> Absolute(const NewMatrix<T, r, c>& a) {
            NewMatrix<T, r, c> out;
            for (unsigned int cc = 0; cc < c; ++cc) {
                for (unsigned int rr = 0; rr < r; ++rr) {
                    out.array[cc][rr] = std::abs(a.array[cc][rr]);
                }
            }
            return out;
        }

        template <typename T, uint32_t r, uint32_t c>
        constexpr NewMatrix<T, r, c> Transpose(const NewMatrix<T, r, c>& a) {
            NewMatrix<T, r, c> out;
            for (unsigned int cc = 0; cc < c; ++cc) {
                for (unsigned int rr = 0; rr < r; ++rr) {
                    out.array[cc][rr] = (a.array[rr][cc]);
                }
            }
            return out;
        }

        //Matrix Specialisations
        template <typename T>
        constexpr NewMatrix<T, 2, 2> Inverse(const NewMatrix<T, 2, 2>& mat) {
            NewMatrix<T, 2, 2> outMat;

            float determinant = (mat.array[0][0] * mat.array[1][1]) - (mat.array[0][1] * mat.array[1][0]);
            float invDet = 1.0f / determinant; //Turn our divides into multiplies!

            outMat.array[0][0] = mat.array[1][1] * invDet;
            outMat.array[0][1] = -mat.array[1][0] * invDet;
            outMat.array[1][0] = -mat.array[0][1] * invDet;
            outMat.array[1][1] = mat.array[0][0] * invDet;

            return outMat;
        }

        template <typename T>
        constexpr NewMatrix<T, 3, 3> Inverse(const NewMatrix<T, 3, 3>& mat) {
            NewMatrix<T, 3, 3> outMat;
            return outMat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> Inverse(const NewMatrix<T, 4, 4>& mat) {
            float det = 0.0f;
            float invDet = 0.0f;

            // 2x2 sub-determinants required to calculate 4x4 determinant
            float det2_01_01 = mat.array[0][0] * mat.array[1][1] - mat.array[0][1] * mat.array[1][0];
            float det2_01_02 = mat.array[0][0] * mat.array[1][2] - mat.array[0][2] * mat.array[1][0];
            float det2_01_03 = mat.array[0][0] * mat.array[1][3] - mat.array[0][3] * mat.array[1][0];
            float det2_01_12 = mat.array[0][1] * mat.array[1][2] - mat.array[0][2] * mat.array[1][1];
            float det2_01_13 = mat.array[0][1] * mat.array[1][3] - mat.array[0][3] * mat.array[1][1];
            float det2_01_23 = mat.array[0][2] * mat.array[1][3] - mat.array[0][3] * mat.array[1][2];

            // 3x3 sub-determinants required to calculate 4x4 determinant
            float det3_201_012 = mat.array[2][0] * det2_01_12 - mat.array[2][1] * det2_01_02 + mat.array[2][2] * det2_01_01;
            float det3_201_013 = mat.array[2][0] * det2_01_13 - mat.array[2][1] * det2_01_03 + mat.array[2][3] * det2_01_01;
            float det3_201_023 = mat.array[2][0] * det2_01_23 - mat.array[2][2] * det2_01_03 + mat.array[2][3] * det2_01_02;
            float det3_201_123 = mat.array[2][1] * det2_01_23 - mat.array[2][2] * det2_01_13 + mat.array[2][3] * det2_01_12;

            det = (-det3_201_123 * mat.array[3][0] + det3_201_023 * mat.array[3][1] - det3_201_013 * mat.array[3][2] + det3_201_012 * mat.array[3][3]);

            invDet = 1.0f / det;

            // remaining 2x2 sub-determinants
            float det2_03_01 = mat.array[0][0] * mat.array[3][1] - mat.array[0][1] * mat.array[3][0];
            float det2_03_02 = mat.array[0][0] * mat.array[3][2] - mat.array[0][2] * mat.array[3][0];
            float det2_03_03 = mat.array[0][0] * mat.array[3][3] - mat.array[0][3] * mat.array[3][0];
            float det2_03_12 = mat.array[0][1] * mat.array[3][2] - mat.array[0][2] * mat.array[3][1];
            float det2_03_13 = mat.array[0][1] * mat.array[3][3] - mat.array[0][3] * mat.array[3][1];
            float det2_03_23 = mat.array[0][2] * mat.array[3][3] - mat.array[0][3] * mat.array[3][2];

            float det2_13_01 = mat.array[1][0] * mat.array[3][1] - mat.array[1][1] * mat.array[3][0];
            float det2_13_02 = mat.array[1][0] * mat.array[3][2] - mat.array[1][2] * mat.array[3][0];
            float det2_13_03 = mat.array[1][0] * mat.array[3][3] - mat.array[1][3] * mat.array[3][0];
            float det2_13_12 = mat.array[1][1] * mat.array[3][2] - mat.array[1][2] * mat.array[3][1];
            float det2_13_13 = mat.array[1][1] * mat.array[3][3] - mat.array[1][3] * mat.array[3][1];
            float det2_13_23 = mat.array[1][2] * mat.array[3][3] - mat.array[1][3] * mat.array[3][2];

            // remaining 3x3 sub-determinants
            float det3_203_012 = mat.array[2][0] * det2_03_12 - mat.array[2][1] * det2_03_02 + mat.array[2][2] * det2_03_01;
            float det3_203_013 = mat.array[2][0] * det2_03_13 - mat.array[2][1] * det2_03_03 + mat.array[2][3] * det2_03_01;
            float det3_203_023 = mat.array[2][0] * det2_03_23 - mat.array[2][2] * det2_03_03 + mat.array[2][3] * det2_03_02;
            float det3_203_123 = mat.array[2][1] * det2_03_23 - mat.array[2][2] * det2_03_13 + mat.array[2][3] * det2_03_12;

            float det3_213_012 = mat.array[2][0] * det2_13_12 - mat.array[2][1] * det2_13_02 + mat.array[2][2] * det2_13_01;
            float det3_213_013 = mat.array[2][0] * det2_13_13 - mat.array[2][1] * det2_13_03 + mat.array[2][3] * det2_13_01;
            float det3_213_023 = mat.array[2][0] * det2_13_23 - mat.array[2][2] * det2_13_03 + mat.array[2][3] * det2_13_02;
            float det3_213_123 = mat.array[2][1] * det2_13_23 - mat.array[2][2] * det2_13_13 + mat.array[2][3] * det2_13_12;

            float det3_301_012 = mat.array[3][0] * det2_01_12 - mat.array[3][1] * det2_01_02 + mat.array[3][2] * det2_01_01;
            float det3_301_013 = mat.array[3][0] * det2_01_13 - mat.array[3][1] * det2_01_03 + mat.array[3][3] * det2_01_01;
            float det3_301_023 = mat.array[3][0] * det2_01_23 - mat.array[3][2] * det2_01_03 + mat.array[3][3] * det2_01_02;
            float det3_301_123 = mat.array[3][1] * det2_01_23 - mat.array[3][2] * det2_01_13 + mat.array[3][3] * det2_01_12;

            NewMatrix<T, 4, 4> outMat;
            outMat.array[0][0] = -det3_213_123 * invDet;
            outMat.array[1][0] = +det3_213_023 * invDet;
            outMat.array[2][0] = -det3_213_013 * invDet;
            outMat.array[3][0] = +det3_213_012 * invDet;

            outMat.array[0][1] = +det3_203_123 * invDet;
            outMat.array[1][1] = -det3_203_023 * invDet;
            outMat.array[2][1] = +det3_203_013 * invDet;
            outMat.array[3][1] = -det3_203_012 * invDet;

            outMat.array[0][2] = +det3_301_123 * invDet;
            outMat.array[1][2] = -det3_301_023 * invDet;
            outMat.array[2][2] = +det3_301_013 * invDet;
            outMat.array[3][2] = -det3_301_012 * invDet;

            outMat.array[0][3] = -det3_201_123 * invDet;
            outMat.array[1][3] = +det3_201_023 * invDet;
            outMat.array[2][3] = -det3_201_013 * invDet;
            outMat.array[3][3] = +det3_201_012 * invDet;

            return outMat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> Translation(const NewVector<T, 3>& v) {
            NewMatrix<T, 4, 4> mat;

            mat.array[3][0] = T(v.x);
            mat.array[3][1] = T(v.y);
            mat.array[3][2] = T(v.z);

            return mat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> Scale(const NewVector<T, 3>& v) {
            NewMatrix<T, 4, 4> mat;

            mat.array[0][0] = T(v.x);
            mat.array[1][1] = T(v.y);
            mat.array[2][2] = T(v.z);

            return mat;
        }


        template <typename T>
        constexpr NewMatrix<T, 3, 3> Scale3x3(const NewVector<T, 3>& v) {
            NewMatrix<T, 3, 3> mat;

            mat.array[0][0] = T(v.x);
            mat.array[1][1] = T(v.y);
            mat.array[2][2] = T(v.z);

            return mat;
        }


        template <typename T>
        constexpr NewMatrix<T, 2, 2> Rotation(T degrees) {
            NewMatrix<T, 2, 2> mat;

            float radians = Maths::DegreesToRadians(degrees);
            float s = sin(radians);
            float c = cos(radians);

            mat.array[0][0] = c;
            mat.array[0][1] = s;
            mat.array[1][0] = -s;
            mat.array[1][1] = c;

            return mat;
        }

        template <typename T>
        constexpr NewMatrix<T, 3, 3> RotationMatrix3x3(T degrees, const NewVector<T, 3>& inAxis) {
            NewMatrix<T, 3, 3> mat;

            const NewVector<T, 3> axis = Vector::Normalise(inAxis);

            float c = cos((float)Maths::DegreesToRadians(degrees));
            float s = sin((float)Maths::DegreesToRadians(degrees));

            mat.array[0][0] = (axis.x * axis.x) * (1.0f - c) + c;
            mat.array[0][1] = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
            mat.array[0][2] = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

            mat.array[1][0] = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
            mat.array[1][1] = (axis.y * axis.y) * (1.0f - c) + c;
            mat.array[1][2] = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

            mat.array[2][0] = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
            mat.array[2][1] = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
            mat.array[2][2] = (axis.z * axis.z) * (1.0f - c) + c;

            return mat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> Rotation(T degrees, const NewVector<T, 3>& inAxis) {
            NewMatrix<T, 4, 4> mat;

            const NewVector<T, 3> axis = Vector::Normalise(inAxis);

            float c = cos((float)Maths::DegreesToRadians(degrees));
            float s = sin((float)Maths::DegreesToRadians(degrees));

            mat.array[0][0] = (axis.x * axis.x) * (1.0f - c) + c;
            mat.array[0][1] = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
            mat.array[0][2] = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

            mat.array[1][0] = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
            mat.array[1][1] = (axis.y * axis.y) * (1.0f - c) + c;
            mat.array[1][2] = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

            mat.array[2][0] = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
            mat.array[2][1] = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
            mat.array[2][2] = (axis.z * axis.z) * (1.0f - c) + c;

            return mat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> Perspective(T zNear, T zFar, float aspectRatio, float fov, bool ndc01 = false)
        {
            NewMatrix<T, 4, 4> mat;

            const float h = 1.0f / tan(Maths::DegreesToRadians(fov) / 2.0f);
            float negDepth = zNear - zFar;

            mat.array[0][0] = h / aspectRatio;
            mat.array[1][1] = h;
            mat.array[2][2] = (zFar + zNear) / negDepth;
            mat.array[2][3] = -1.0f;
            mat.array[3][2] = 2.0f * (zNear * zFar) / negDepth;
            mat.array[3][3] = 0.0f;

            return mat;
        }

        //http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
        template <typename T>
        constexpr NewMatrix<T, 4, 4> Orthographic(T xMin, T xMax, T yMin, T yMax, T zMin, T zMax, bool ndc01 = false)
        {
            NewMatrix<T, 4, 4> mat;

            mat.array[0][0] = 2.0f / (xMax - xMin);
            mat.array[1][1] = 2.0f / (yMax - yMin);
            mat.array[2][2] = -2.0f / (zMax - zMin);

            mat.array[3][0] = -(xMax + xMin) / (xMax - xMin);
            mat.array[3][1] = -(yMax + yMin) / (yMax - yMin);
            mat.array[3][2] = -(zMax + zMin) / (zMax - zMin);
            mat.array[3][3] = 1.0f;

            return mat;
        }

        template <typename T>
        constexpr NewMatrix<T, 4, 4> View(
            const NewVector<T, 3>& cameraPos,
            const NewVector<T, 3>& lookAtPos,
            const NewVector<T, 3>& upAxis = NewVector<T, 3>(0, 1, 0)
        )
        {
            NewMatrix<T, 4, 4> tMat = Translation(-cameraPos);

            NewVector<T, 3> zDir = Vector::Normalise(lookAtPos - cameraPos);

            NewVector<T, 3> xDir = Vector::Cross(zDir, upAxis);
            xDir = Vector::Normalise(xDir);

            NewVector<T, 3> yDir = Vector::Cross(xDir, zDir);
            yDir = Vector::Normalise(yDir);

            NewMatrix<T, 4, 4> rMat;

            rMat.array[0][0] = xDir.x;
            rMat.array[1][0] = xDir.y;
            rMat.array[2][0] = xDir.z;

            rMat.array[0][1] = yDir.x;
            rMat.array[1][1] = yDir.y;
            rMat.array[2][1] = yDir.z;

            rMat.array[0][2] = -zDir.x;
            rMat.array[1][2] = -zDir.y;
            rMat.array[2][2] = -zDir.z;

            return rMat * tMat;
        }
    }
}