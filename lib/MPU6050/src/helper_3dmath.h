// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class, 3D math helper
// 6/5/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-05 - add 3D math helper file to DMP6 example sketch

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#ifndef _HELPER_3DMATH_H_
#define _HELPER_3DMATH_H_

#include "Arduino.h"
#include <math.h>

class Quaternion {
    public:
        float w;
        float x;
        float y;
        float z;

        Quaternion(): w(1.f), x(0.f), y(0.f), z(0.f) {}

        Quaternion(float nw, float nx, float ny, float nz): w(nw), x(nw), y(nw), z(nw) {}

        Quaternion getProduct(const Quaternion& q) const {
            // Quaternion multiplication is defined by:
            //     (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
            //     (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
            //     (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
            //     (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
            return Quaternion(
                w*q.w - x*q.x - y*q.y - z*q.z,  // new w
                w*q.x + x*q.w + y*q.z - z*q.y,  // new x
                w*q.y - x*q.z + y*q.w + z*q.x,  // new y
                w*q.z + x*q.y - y*q.x + z*q.w); // new z
        }

        Quaternion getConjugate() const {
            return Quaternion(w, -x, -y, -z);
        }

        float getMagnitude() const {
            return sqrt(w*w + x*x + y*y + z*z);
        }

        void normalize() {
            float m = getMagnitude();
            w /= m;
            x /= m;
            y /= m;
            z /= m;
        }

        Quaternion getNormalized() const {
            Quaternion r(w, x, y, z);
            r.normalize();
            return r;
        }
};

template<typename T>
class VectorBase {
public:
    T x;
    T y;
    T z;

    VectorBase<T>(): x(), y(), z() {}
    VectorBase<T>(T nx, T ny, T nz): x(nx), y(ny), z(nz) {}
    VectorBase<T>(const VectorBase<T>& o): x(o.x), y(o.y), z(o.z) {}

    VectorBase<T>& operator =(const VectorBase<T>& o) {
      if(this == &o) return *this;
      x = o.x;
      y = o.y;
      z = o.z;
      return *this;
    }

    operator VectorBase<float>() {
      return VectorBase<float>(x, y, z);
    }

    float getMagnitude() {
      return sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        float m = getMagnitude();
        x /= m;
        y /= m;
        z /= m;
    }

    VectorBase<T> getNormalized() {
        VectorBase<T> r(x, y, z);
        r.normalize();
        return r;
    }

    void rotate(const Quaternion& q) {
        // http://www.cprogramming.com/tutorial/3d/quaternions.html
        // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
        // http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
        // ^ or: http://webcache.googleusercontent.com/search?q=cache:xgJAp3bDNhQJ:content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation&hl=en&gl=us&strip=1

        // P_out = q * P_in * conj(q)
        // - P_out is the output vector
        // - q is the orientation quaternion
        // - P_in is the input vector (a*aReal)
        // - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])
        Quaternion p(0, x, y, z);

        // quaternion multiplication: q * p, stored back in p
        p = q.getProduct(p);

        // quaternion multiplication: p * conj(q), stored back in p
        p = p.getProduct(q.getConjugate());

        // p quaternion is now [0, x', y', z']
        x = p.x;
        y = p.y;
        z = p.z;
    }

    VectorBase<T> getRotated(const Quaternion& q) const {
        VectorBase<T> r(x, y, z);
        r.rotate(q);
        return r;
    }

    // vector arithmetics
    VectorBase<T>& operator+=(const VectorBase<T>& o) {
      x += o.x;
      y += o.y;
      z += o.z;
      return *this;
    }

    VectorBase<T> operator+(const VectorBase<T>& o) {
      VectorBase<T> r(*this);
      r += o;
      return r;
    }

    VectorBase<T>& operator-=(const VectorBase<T>& o) {
      x -= o.x;
      y -= o.y;
      z -= o.z;
      return *this;
    }

    VectorBase<T> operator-(const VectorBase<T>& o) {
      VectorBase<T> r(*this);
      r -= o;
      return r;
    }

    // scalar operations
    VectorBase<T>& operator*=(T o) {
      x *= o;
      y *= o;
      z *= o;
      return *this;
    }

    VectorBase<T> operator*(T o) {
      VectorBase<T> r(*this);
      r *= o;
      return r;
    }

    VectorBase<T>& operator/=(T o) {
      x /= o;
      y /= o;
      z /= o;
      return *this;
    }

    VectorBase<T> operator/(T o) {
      VectorBase<T> r(*this);
      r /= o;
      return r;
    }
};

typedef VectorBase<float> VectorFloat;
typedef VectorBase<int16_t> VectorInt16;


#endif /* _HELPER_3DMATH_H_ */
