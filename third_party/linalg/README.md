**NOTE: This is the development branch for `linalg.h v3.0`. Breaking changes may occur until the `v3.0` tag is published.**

[![Release is 3.0-beta](https://img.shields.io/badge/version-3.0--beta-blue.svg)](http://raw.githubusercontent.com/sgorsten/linalg/v3/linalg.h)
[![License is Unlicense](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)
[![Travis CI build status](http://travis-ci.org/sgorsten/linalg.svg?branch=v3)](https://travis-ci.org/sgorsten/linalg)
[![Appveyor build status](http://ci.appveyor.com/api/projects/status/l4bfv5omodkajuc9?svg=true)](https://ci.appveyor.com/project/sgorsten/linalg)

[`linalg.h`](/linalg.h) is a [single header](http://github.com/nothings/stb/blob/master/docs/other_libs.md), [public domain](http://unlicense.org/) linear algebra library for [C++11](http://en.cppreference.com/w/). It is inspired by the syntax of popular shading and compute languages and is intended to serve as a lightweight alternative to projects such as [GLM](http://glm.g-truc.net/0.9.7/), [Boost.QVM](https://www.boost.org/doc/libs/1_66_0/libs/qvm/doc/index.html) or [Eigen](http://eigen.tuxfamily.org/) in the domains such as computer graphics, computational geometry, and physical simulation. It allows you to easily write programs like the following:

```cpp
#include <linalg.h>
using namespace linalg::aliases;

// Compute the equation of a plane containing points a, b, and c
float4 compute_plane(float3 a, float3 b, float3 c)
{
    float3 n = cross(b-a, c-a);
    return {n, -dot(n,a)};
}
```

* [Data structures](#data-structures)
  * [Vectors](#vectors)
  * [Matrices](#matrices)
  * [Quaternions](#quaternions)
* [Function listing](#function-listing)
  * [Vector algebra](#vector-algebra)
  * [Matrix algebra](#matrix-algebra)
  * [Quaternion algebra](#quaternion-algebra)
  * [Component-wise operations](#component-wise-operations)
  * [Reductions](#reductions)
* [Optional features](#optional-features)
  * [Type aliases](#type-aliases)
  * [`ostream` overloads](#ostream-overloads)
  * [User-defined conversions](#user-defined-conversions)
  * [Extensions in `linalg-ext.h`](#extensions-in-linalgxh)
* [Higher order functions](#higher-order-functions)
* [Design rationale](#design-rationale)
* [Stability guarantees](#stability-guarantees)
* [Changes from v2.1](#changes-from-v21)

## Data structures

#### Vectors

`linalg::vec<T,M>` defines a fixed-length vector containing exactly `M` elements of type `T`. Convenience aliases such as `float3`, `float4`, or `int2` are provided in the [`linalg::aliases` namespace](#type-aliases). This data structure can be used to store a wide variety of types of data, including geometric vectors, points, homogeneous coordinates, plane equations, colors, texture coordinates, or any other situation where you need to manipulate a small sequence of numbers. As such, `vec<T,M>` is supported by a set of [algebraic](#vector-algebra) and [component-wise](#component-wise-operations) functions, as well as a set of standard [reductions](#reductions).

`vec<T,M>`:
* is [`DefaultConstructible`](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible):
  ```cpp
  float3 v; // v contains 0,0,0
  ```
* is constructible from `M` elements of type `T`:
  ```cpp
  float3 v {1,2,3}; // v contains 1,2,3
  ```
* is [`CopyConstructible`](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) and [`CopyAssignable`](https://en.cppreference.com/w/cpp/named_req/CopyAssignable): 
  ```cpp
  float3 v {1,2,3}; // v contains 1,2,3
  float3 u {v};     // u contains 1,2,3
  float3 w;         // w contains 0,0,0 
  w = u;            // w contains 1,2,3
  ```
* is [`EqualityComparable`](https://en.cppreference.com/w/cpp/named_req/EqualityComparable) and [`LessThanComparable`](https://en.cppreference.com/w/cpp/named_req/LessThanComparable):
  ```cpp
  if(v == y) cout << "v and u contain equal elements in the same positions" << endl;
  if(v < u) cout << "v precedes u lexicographically" << endl;
  ```  
* is **explicitly** constructible from a single element of type `T`:
  ```cpp
  float3 v = float3{4}; // v contains 4,4,4
  ```
* is **explicitly** constructible from a `vec<U,M>` of some other type `U`:
  ```cpp
  float3 v {1.1f,2.3f,3.5f}; // v contains 1.1,2.3,3.5
  int3 u = int3{v};          // u contains 1,2,3
  ```
* supports indexing: 
  ```cpp
  float x = v[0]; // x contains first element of v
  v[2] = 5;       // third element of v set to 5
  ```
* supports named accessors `x,y,z,w`, `r,g,b,a`, or `s,t,p,q` and swizzles:
  ```cpp
  float y = point.y;    // y contains second element of point
  pixel.a = 0.5;        // fourth element of pixel set to 0.5
  float s = tc.s;       // s contains first element of tc
  float3 c = pixel.bgr; // c contains pixel[2],pixel[1],pixel[0]
  ```
* supports unary operators `+`, `-`, `!` and `~` in component-wise fashion: 
  ```cpp
  auto v = -float{2,3}; // v is float2{-2,-3}
  ```
* supports binary operators `+`, `-`, `*`, `/`, `%`, `|`, `&`, `^`, `<<` and `>>` in component-wise fashion: 
  ```cpp
  auto v = float2{1,1} + float2{2,3}; // v is float2{3,4}
  ```
* supports binary operators with a scalar on the left or the right:
  ```cpp
  auto v = 2 * float3{1,2,3}; // v is float3{2,4,6}
  auto u = float3{1,2,3} + 1; // u is float3{2,3,4}
  ```
* supports operators `+=`, `-=`, `*=`, `/=`, `%=`, `|=`, `&=`, `^=`, `<<=` and `>>=` with vectors or scalars on the right:
  ```cpp
  float2 v {1,2}; v *= 3; // v is float2{3,6}
  ```
* supports operations on mixed element types: 
  ```cpp
  auto v = float3{1,2,3} + int3{4,5,6}; // v is float3{5,7,9}
  ```
* supports [range-based for](https://en.cppreference.com/w/cpp/language/range-for):
  ```cpp
  for(auto elem : float3{1,2,3}) cout << elem << ' '; // prints "1 2 3 "
  ```
* has a flat memory layout: 
  ```cpp
  float3 v {1,2,3}; 
  float * p = v.data(); // &v[i] == p+i
  p[1] = 4; // v contains 1,4,3
  ```

#### Matrices

`linalg::mat<T,M,N>` defines a fixed-size matrix containing exactly `M` rows and `N` columns of type `T`, in column-major order. Convenience aliases such as `float4x4` or `double3x3` are provided in the [`linalg::aliases` namespace](#type-aliases). Unlike `vec<T,M>`, this data structure is **not** intended for general storage of two dimensional arrays of data, and is supported only by a set of [algebraic](#matrix-algebra) functions. However, component-wise and reduction operations can be invoked explicitly via [higher-order functions](#higher-order-functions).

`mat<T,M,N>`:
* is [`DefaultConstructible`](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible):
  ```cpp
  float2x2 m; // m contains columns 0,0; 0,0
  ```
* is constructible from `N` columns of type `vec<T,M>`: 
  ```cpp
  float2x2 m {{1,2},{3,4}}; // m contains columns 1,2; 3,4
  ```
* is constructible from `linalg::identity`:
  ```cpp
  float3x3 m = linalg::identity; // m contains columns 1,0,0; 0,1,0; 0,0,1
  ```
* is [`CopyConstructible`](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) and [`CopyAssignable`](https://en.cppreference.com/w/cpp/named_req/CopyAssignable): 
  ```cpp
  float2x2 m {{1,2},{3,4}}; // m contains columns 1,2; 3,4
  float2x2 n {m};           // n contains columns 1,2; 3,4
  float2x2 p;               // p contains columns 0,0; 0,0
  p = n;                    // p contains columns 1,2; 3,4
  ```
* is [`EqualityComparable`](https://en.cppreference.com/w/cpp/named_req/EqualityComparable) and [`LessThanComparable`](https://en.cppreference.com/w/cpp/named_req/LessThanComparable):
  ```cpp
  if(m == n) cout << "m and n contain equal elements in the same positions" << endl;
  if(m < n) cout << "m precedes n lexicographically when compared in column-major order" << endl;
  ```  
* is **explicitly** constructible from a single element of type `T`: 
  ```cpp
  float2x2 m {5}; // m contains columns 5,5; 5,5
  ```
* is **explicitly** constructible from a `mat<U,M,N>` of some other type `U`: 
  ```cpp
  float2x2 m {int2x2{{5,6},{7,8}}}; // m contains columns 5,6; 7,8
  ```
* supports indexing into *columns*: 
  ```cpp
  float2x3 m {{1,2},{3,4},{5,6}}; // m contains columns 1,2; 3,4; 5,6
  float2 c = m[0];                // c contains 1,2
  m[1]     = {7,8};               // m contains columns 1,2; 7,8; 5,6
  ```
* supports retrieval of rows:
  ```cpp
  float2x3 m {{1,2},{3,4},{5,6}}; // m contains columns 1,2; 3,4; 5,6
  float3 r = m.row(1);            // r contains 2,4,6
  ```
* supports unary operators `+`, `-`:
  ```cpp
  float2x2 m {{1,2},{3,4}}; // m contains columns 1,2; 3,4
  float2x2 n = -m;          // n contains columns -1,-2; -3,-4
  ```
* supports binary operators `+`, `-` between matrices of the same size: 
  ```cpp
  float2x2 a {{0,0},{2,2}}; // a contains columns 0,0; 2,2
  float2x2 b {{1,2},{1,2}}; // b contains columns 1,2; 1,2
  float2x2 c = a + b;       // c contains columns 1,2; 3,4
  ```
* supports operator `*` with a scalar on the left or on the right
* supports operator `/` with a scalar on the right
* supports operator `*` with a `vec<T,N>` on the right (matrix product)
* supports operator `*` with a `mat<T,N,P>` on the right (matrix product)
* supports operators `+=`, `-=`, `*=`, `/=` with appropriate types on the right
* supports operations on mixed element types
* supports [range-based for](https://en.cppreference.com/w/cpp/language/range-for) over columns
* has a flat memory layout

**TODO: Finish explaining `linalg::mat<T,M,N>`**

#### Quaternions

`linalg::quat<T>` defines a quaternion using four elements of type `T`, representing the quantity *xi + yi + zj + w* in `x,y,z,w` order. Convenience aliases such as `quatf` and `quatd` are provided in the [`linalg::aliases` namespace](#type-aliases). Unlike `vec<T,4>`, this data structure is **not** intended for general storage of four dimensional quantities, and is supported only by a set of [algebraic](#quaternion-algebra) functions. However, component-wise and reduction operations can be invoked explicitly via [higher-order functions](#higher-order-functions).

`quat<T>`:
* is [`DefaultConstructible`](https://en.cppreference.com/w/cpp/named_req/DefaultConstructible): to **zero**:
  ```cpp
  quatf q; // q contains 0,0,0,0
  ```
* is constructible from four elements of type `T`:
  ```cpp
  quatf q {1,0,0,0}; // q contains 1,0,0,0
  ```
* is constructible from `linalg::identity`:
  ```cpp
  quatf q = linalg::identity; // q contains 0,0,0,1
  ```  
* is constructible from a `vec<T,3>` and a `T`:
  ```cpp
  float3 axis {0,1,0};
  float angle = 2;
  quatf q {axis*sinf(angle/2), cosf(angle/2)}; // q represents rotation of angle about axis
  ```  
* is [`CopyConstructible`](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) and [`CopyAssignable`](https://en.cppreference.com/w/cpp/named_req/CopyAssignable): 
  ```cpp
  quatf q {0,1,0,0}; // q contains 0,1,0,0
  quatf r {q};       // r contains 0,1,0,0
  quatf s;           // s contains 0,0,0,0
  s = r;             // s contains 0,1,0,0
  ```
* is [`EqualityComparable`](https://en.cppreference.com/w/cpp/named_req/EqualityComparable) and [`LessThanComparable`](https://en.cppreference.com/w/cpp/named_req/LessThanComparable):
  ```cpp
  if(q == r) cout << "q and r are equal quaternions" << endl;
  if(q < r) cout << "q precedes r lexicographically in x,y,z,w order" << endl;
  ```  
* is **explicitly** constructible from a `vec<T,4>`:
  ```cpp
  float4 v {1,2,3,4}; // v contains 1,2,3,4
  quatf q {v};        // q contains 1,2,3,4
  ```
* is **explicitly** constructible from a `quat<U>` of some other type `U`:
  ```cpp
  quatf q {quatd{1,0,0,0}}; // q contains 1,0,0,0`
  ```
* has named fields `x`,`y`,`z`,`w`:
  ```cpp
  float real = q.w; // real contains the real-valued part of q
  ```
* supports unary operators `+`, `-`
* supports binary operators `+`, `-` between quaternions
* supports operator `*` with quaternions or scalars
* supports operator `/` with a scalar on the right
* supports operators `+=`, `-=`, `*=`, `/=` with appropriate types on the right
* supports operations on mixed element types
* exposes explicit cast to `vec<T,4>` and member function `xyz()`

**TODO: Finish explaining `linalg::quat<T>`**

## Function listing

#### Vector algebra

**TODO: Explain `cross`, `dot`, `length`, `length2`, `normalize`, `distance`, `distance2`, `angle`, `uangle`, `nlerp`, `slerp`**

#### Matrix algebra

**TODO: Explain `diagonal`, `trace`, `outerprod`, `transpose`, `adjugate`, `comatrix`, `determinant`, `inverse`**

#### Quaternion algebra

**TODO: Explain `exp`, `log`, `pow`, `conjugate`, `dot`, `length`, `length2`, `inverse`, `normalize`, `uangle`, `lerp`, `nlerp`, `slerp`**
**TODO: Explain `qxdir`, `qydir`, `qzdir`, `qmat`, `qrot`, `qangle`, `qaxis`**

#### Component-wise operations

The unary functions `abs`, `floor`, `ceil`, `exp`, `log`, `log10`, `sqrt`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `round` accept a vector-valued argument and produce a vector-valued result by passing individual elements to the function of the same name in the `std::` namespace, as defined by `<cmath>` or `<cstdlib>`.

```cpp
float4 a {1,-4,9,-16}; // a contains 1,-4,9,-16
float4 b = abs(a);     // b contains 1,4,9,16
float4 c = sqrt(b);    // c contains 1,2,3,4
```

The binary functions `fmod`, `pow`, `atan2`, and `copysign` function similarly, except that either argument can be a vector or a scalar.

```cpp
float2 a {5,4}, b {2,3};
float2 c = pow(a, 2);    // c contains 25,16
float2 d = pow(2, b);    // d contains 4,8
float2 e = pow(a, b);    // e contains 25,64
```

The binary functions `equal`, `nequal`, `less`, `greater`, `lequal`, and `gequal` apply operators `==`, `!=`, `<`, `>`, `<=` and `>=` respectively in a component-wise fashion, returning a `vec<bool,M>`. As before, either argument can be a vector or a scalar.

```cpp
int2 a {2,5}, b {3,4};
bool2 c = less(a,3);    // c contains true, false
bool2 d = equal(4,b);   // d contains false, true
bool2 e = greater(a,b); // e contains false, true
```

**TODO: Explain `min`, `max`, `clamp`, `select`, `lerp`**

#### Reductions

* `any :: vec<bool,M> => bool` returns true if any element of the vector is true
* `all :: vec<bool,M> => bool` returns true if all elements of the vector are true
* `sum :: vec<T,M> => T` returns the sum of all elements in the vector
* `product :: vec<T,M> => T` returns the product of all elements in the vector
* `minelem :: vec<T,M> => T` returns the **value** of the least element in the vector
* `maxelem :: vec<T,M> => T` returns the **value** of the greatest element in the vector
* `argmin :: vec<T,M> => int` returns the **index** of the least element in the vector
* `argmax :: vec<T,M> => int` returns the **index** of the greatest element in the vector

#### Comparisons

**TODO: Explain `compare`**

## Optional features

#### Type aliases

By default, `linalg.h` does not define any symbols in the global namespace, and a three-element vector of single-precision floating point values must be spelled `linalg::vec<float,3>`. In various libraries and shading languages, such a type might be spelled `float3`, `vec3`, `vec3f`, `point3f`, `simd_float3`, or any one of a hundred other possibilities. `linalg.h` provides a collection of useful aliases in the `linalg::aliases` namespace. If the names specified in this namespace are suitable for a user's purposes, they can quickly be brought into scope as follows:

```cpp
#include <linalg.h>
using namespace linalg::aliases;

float3 a_vector;
float4x4 a_matrix;
```

Note that this **only** brings the type aliases into global scope. The core types and all functions and operator overloads defined by the library remain in `namespace linalg`. 

If the spellings in `namespace linalg::aliases` conflict with other types that have been defined in the global namespace or in other namespaces of interest, the user can choose to omit the `using namespace` directive and instead define their own aliases as desired.

```cpp
#include <linalg.h>
using v3f = linalg::vec<float,3>;
using m44f = linalg::mat<float,4,4>;

v3f a_vector;
m44f a_matrix;
```

It is, of course, always possible to use the core `linalg.h` types directly if operating in an environment where no additional symbols should be defined.

```cpp
#include <linalg.h>

linalg::vec<float,3> a_vector;
linalg::mat<float,4,4> a_matrix;
```

The set of type aliases defined in `namespace linalg::aliases` is as follows:

* `vec<float,M>` aliased to *floatM*, as in: `float1`, `float2`, `float3`, `float4`
* `vec<double,M>` aliased to *doubleM*, as in: `double1`, `double2`, `double3`, `double4`
* `vec<int,M>` aliased to *intM* as in: `int1`, `int2`, `int3`, `int4`
* `vec<bool,M>` aliased to *boolM* as in: `bool1`, `bool2`, `bool3`, `bool4`
* `mat<float,M,N>` aliased to *floatMxN* as in: `float1x3`, `float3x2`, `float4x4`, etc.
* `mat<double,M,N>` aliased to *doubleMxN* as in: `double1x3`, `double3x2`, `double4x4`, etc.
* `mat<int,M,N>` aliased to *intMxN* as in: `int1x3`, `int3x2`, `int4x4`, etc.
* `quat<float>` aliased to `quatf`
* `quat<double>` aliased to `quatd`
* `quat<int>` aliased to `quati`

#### `ostream` overloads

**TODO: Explain `namespace linalg::ostream_overloads`**

#### User-defined conversions

**TODO: Explain `converter<T,U>`**

#### Extensions in `linalg-ext.h`

A second header `linalg-ext.h` is under development, providing functionality that is not essential to the use of `linalg.h`. This header will be considered unstable even after the publication of `v3.0`, but should stabilize shortly thereafter.

## Higher order functions

#### `linalg::fold(f, a, b)`

`fold(f, a, b)` is a higher order function which accepts a function of the form `A,B => A` and repeatedly invokes `a = f(a, element_of_b)` until all elements have been consumed, before returning `a`. It is approximately equal to a [left fold with an initial value](https://en.wikipedia.org/wiki/Fold_(higher-order_function)). When `b` is a `vec<T,M>`, elements are folded from least to greatest index. When `b` is a `mat<T,M,N>`, elements are folded in column-major order. When `b` is a `quat<T>`, elements are folded in `x,y,z,w` order.

See also: [Reductions](#reductions)

#### `linalg::apply(f, a...)`

`apply(f, a...)` is a higher order function which accepts a function of the form `A... => T` and applies it to component-wise sets of elements from data structures of compatible shape and dimensions. It is approximately equal to a [convolution](https://en.wikipedia.org/wiki/Convolution_(computer_science)) followed by a [map](https://en.wikipedia.org/wiki/Map_(higher-order_function)). The shape of the result (that is, whether it is a scalar, vector, matrix, or quaternion, and the dimensions thereof) is determined by the arguments. If more than one argument is a non-scalar, the shape of those arguments must agree. Scalars can be freely intermixed with non-scalars, and element types can also be freely mixed. The element type of the returned value is determined by the return type of the provided mapping function `f`. The supported call signatures are enumerated in the following table:

| call             | type of `a`  | type of `b`  | type of `c` | result type  | result elements          |
|------------------|--------------|--------------|-------------|--------------|--------------------------|
| `apply(f,a)`     | `A`          |              |             | `T`          | `f(a)`                   |
| `apply(f,a)`     | `vec<A,M>`   |              |             | `vec<T,M>`   | `f(a[i])...`             |
| `apply(f,a)`     | `mat<A,M,N>` |              |             | `mat<T,M,N>` | `f(a[j][i])...`          |
| `apply(f,a)`     | `quat<A>`    |              |             | `quat<T>`    | `f(a.x)...`              |
| `apply(f,a,b)`   | `A`          | `B`          |             | `T`          | `f(a, b)...`             |
| `apply(f,a,b)`   | `A`          | `vec<B,M>`   |             | `vec<T,M>`   | `f(a, b[i])...`          |
| `apply(f,a,b)`   | `vec<A,M>`   | `B`          |             | `vec<T,M>`   | `f(a[i], b)...`          |
| `apply(f,a,b)`   | `vec<A,M>`   | `vec<B,M>`   |             | `vec<T,M>`   | `f(a[i], b[i])...`       |
| `apply(f,a,b)`   | `A`          | `mat<B,M,N>` |             | `mat<T,M,N>` | `f(a, b[j][i])...`       |
| `apply(f,a,b)`   | `mat<A,M,N>` | `B`          |             | `mat<T,M,N>` | `f(a[j][i], b)...`       |
| `apply(f,a,b)`   | `mat<A,M,N>` | `mat<B,M,N>` |             | `mat<T,M,N>` | `f(a[j][i], b[j][i])...` |
| `apply(f,a,b)`   | `A`          | `quat<B>`    |             | `quat<T>`    | `f(a, b.x)...`           |
| `apply(f,a,b)`   | `quat<A>`    | `B`          |             | `quat<T>`    | `f(a.x, b)...`           |
| `apply(f,a,b)`   | `quat<A>`    | `quat<B>`    |             | `quat<T>`    | `f(a.x, b.x)...`         |
| `apply(f,a,b,c)` | `A`          | `B`          | `C`         | `T`          | `f(a, b, c)...`          |
| `apply(f,a,b,c)` | `A`          | `B`          | `vec<C,M>`  | `vec<T,M>`   | `f(a, b, c[i])...`       |
| `apply(f,a,b,c)` | `A`          | `vec<B,M>`   | `C`         | `vec<T,M>`   | `f(a, b[i], c)...`       |
| `apply(f,a,b,c)` | `A`          | `vec<B,M>`   | `vec<C,M>`  | `vec<T,M>`   | `f(a, b[i], c[i])...`    |
| `apply(f,a,b,c)` | `vec<A,M>`   | `B`          | `C`         | `vec<T,M>`   | `f(a[i], b, c)...`       |
| `apply(f,a,b,c)` | `vec<A,M>`   | `B`          | `vec<C,M>`  | `vec<T,M>`   | `f(a[i], b, c[i])...`    |
| `apply(f,a,b,c)` | `vec<A,M>`   | `vec<B,M>`   | `C`         | `vec<T,M>`   | `f(a[i], b[i], c)...`    |
| `apply(f,a,b,c)` | `vec<A,M>`   | `vec<B,M>`   | `vec<C,M>`  | `vec<T,M>`   | `f(a[i], b[i], c[i])...` |

**TODO: Explain `apply_t<F, A...>` and SFINAE helpers.**

See also: [Component-wise operations](#component-wide-operations)

## Design rationale

This space exists for me to explain the design decisions that went into `linalg.h`.

## Stability guarantees

As of the `v3.0` tag, the functionality provided by `linalg.h` will be considered stable, except for:
* The contents of the `linalg::detail` namespace
* The type or implementation details of any types which begin with an underscore, such as `_scalar<...>`, `_lswizzle<...>`, `_rswizzle<...>`
* Direct access to any member variable whose name begins with an underscore, such as `vec<T,M>::_` or `mat<T,M,N>::_`

The `linalg-ext.h` header will continue to experience breaking changes. As functionality in `linalg-ext.h` stabilizes, it may be migrated into `linalg.h`, in which case it will be considered stable.

## Changes from `v2.1`

**TODO: Rework this to include a migration guide for users coming from previous versions of `linalg.h`.**

#### Improvements in `v3.0`

* `mat<T,M,N>` now defines `operator *` as the matrix product
* New type `quat<T>` models quaternions, and defines `operator *` as the quaternion product
* `map(a,f)` and `zip(a,b,f)` subsumed by new `apply(f,a...)`
  * `apply(...)` supports unary, binary, and ternary operations for `vec`
  * `apply(...)` supports unary and binary operations for `mat` and `quat`
  * `apply(...)` can also be invoked exclusively with scalars, and supports arbitrary numbers of arguments
  * `apply(...)` supports mixed element types
  * Template type alias `apply_t<F,A...>` provides the return type of `apply(f,a...)`
* `vec<T,1>` and `mat<T,M,1>` specializations are now provided
* `compare(a,b)` provide three-way comparison between compatible types
* `clamp(a,b,c)` can be invoked with three distinct (but compatible) types
* `select(a,b,c)` provides the a component-wise equivalent to `a ? b : c`
* `lerp(a,b,t)` has been generalized to a component-wise operation where any of `a`, `b`, and `t` can be vectors or scalars
* `vec<T,M>` elements can be referred to via `x`,`y`,`z`,`w` or `r`,`g`,`b`,`a` or `s`,`t`,`p`,`q`
* `vec<T,M>` and `mat<T,M,N>` contain a member function `.data()` which returns a pointer to the elements
* Groups of `vec<T,M>` elements can be accessed via named swizzles, such as `xyz`, `bgr`, or `pq`
  * All swizzles can be used as rvalues
  * Only permutation swizzles (where no accessor is repeated) can be used as lvalues
* User can specialize `converter<T,U>` to enable implicit conversions from `U` to `T`, if either type is a `vec`, `mat`, or `quat`
  * `identity` is implemented using this facility to serve as an in-library example
* No undefined behavior according to the C++11 standard
* Almost all operations which do not internally call `<cmath>` functions are `constexpr`, except for `argmin` and `argmax`
* No lambdas are used in `linalg.h`, avoidng potential ODR violations

#### Breaking changes in `v3.0`

* `linalg.h` no longer supports Visual Studio 2013. However, it is known to work on GCC 4.9+, Clang 3.5+ in C++11 mode and Visual Studio 2015+.
* `vec<T,M>` and `mat<T,M,N>` may only be used with a `T` which is an [arithmetic type](https://en.cppreference.com/w/c/language/arithmetic_types)
  * This requirement will likely be relaxed by the official release of `v3.0` but will require specializing some trait type to indicate additional scalar types
* As stated above, the semantics of `operator *` for `mat<T,M,N>` has changed
  * Additional, the set of operator overloads on matrices has been reduced to those with an obvious arithmetic meaning.
  * `mat+mat`, `mat-mat`, `mat*scalar`, `scalar*mat`, and `mat/scalar` are implemented as elementwise operations
  * `mat*mat` and `mat*vec` computes the matrix product
  * All other binary matrix overloads have been deleted
  * Most componentwise and reduction functions no longer apply to matrices
  * However, componentwise operations can still be invoked manually with the `apply(...)` function
* `vec<T,M>` and `mat<T,M,N>` are now defined in terms of arrays, instead of explicit fields `x`, `y`, `z`, `w`
  * However, `vec<T,M>` can still be accessed with `x`,`y`,`z`,`w` due to special scalar accessor types which should silently convert to `T` in most scenarios
* Some constructors have been removed from `vec` and `mat`
  * `vec<T,M>` no longer has an implicit constructor from `std::array<T,M>`
  * `vec<T,M>` and `mat<T,M,N>` no longer have an explicit constructor from `const T *`
  * These capabilities can be added by specializing `converter<T,U>`, as shown in [test-user-defined-conversions.cpp](tests/test-user-defined-conversions.cpp) 
* The functions `vec::xy()` and `vec::xyz()` have been replaced by the swizzles `vec::xy` and `vec::xyz`
* Some functionality has been moved from `linalg.h` to optional `linalg-ext.h` header
  * quat/matrix factory functions for 3D transformations
  * std::hash<...> specializations
