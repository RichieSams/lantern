//***************************************************************************************
// GeometryGenerator.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

/* Modified for Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "math/int_types.h"

#include "scene/geometry_generator.h"

#include <algorithm>
#include <cmath>


namespace Lantern {

void CreateBox(float width, float height, float depth, Mesh *mesh) {
	// Create the float3a data
	mesh->Positions.resize(24);
	mesh->Normals.resize(24);
	mesh->Tangents.resize(24);
	mesh->TexCoords.resize(24);

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	mesh->BoundingSphere = float4(0.0f, 0.0f, 0.0f, std::max(std::max(w2, h2), d2));

	// Fill in the front face float3a data.
	mesh->Positions[0] = float3a(-w2, -h2, +d2); mesh->Normals[0] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[0] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[0] = float2(0.0f, 1.0f);
	mesh->Positions[1] = float3a(-w2, +h2, +d2); mesh->Normals[1] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[1] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[1] = float2(0.0f, 0.0f);
	mesh->Positions[2] = float3a(+w2, +h2, +d2); mesh->Normals[2] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[2] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[2] = float2(1.0f, 0.0f);
	mesh->Positions[3] = float3a(+w2, -h2, +d2); mesh->Normals[3] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[3] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[3] = float2(1.0f, 1.0f);


	// Fill in the back face float3a data.
	mesh->Positions[4] = float3a(-w2, -h2, -d2); mesh->Normals[4] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[4] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[4] = float2(1.0f, 1.0f);
	mesh->Positions[5] = float3a(+w2, -h2, -d2); mesh->Normals[5] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[5] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[5] = float2(0.0f, 1.0f);
	mesh->Positions[6] = float3a(+w2, +h2, -d2); mesh->Normals[6] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[6] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[6] = float2(0.0f, 0.0f);
	mesh->Positions[7] = float3a(-w2, +h2, -d2); mesh->Normals[7] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[7] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[7] = float2(1.0f, 0.0f);

	// Fill in the top face float3a data.
	mesh->Positions[8] = float3a(-w2, +h2, +d2); mesh->Normals[8] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[8] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[8] = float2(0.0f, 1.0f);
	mesh->Positions[9] = float3a(-w2, +h2, -d2); mesh->Normals[9] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[9] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[9] = float2(0.0f, 0.0f);
	mesh->Positions[10] = float3a(+w2, +h2, -d2); mesh->Normals[10] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[10] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[10] = float2(1.0f, 0.0f);
	mesh->Positions[11] = float3a(+w2, +h2, +d2); mesh->Normals[11] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[11] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[11] = float2(1.0f, 1.0f);

	// Fill in the bottom face float3a data.
	mesh->Positions[12] = float3a(-w2, -h2, +d2); mesh->Normals[12] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[12] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[12] = float2(1.0f, 1.0f);
	mesh->Positions[13] = float3a(+w2, -h2, +d2); mesh->Normals[13] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[13] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[13] = float2(0.0f, 1.0f);
	mesh->Positions[14] = float3a(+w2, -h2, -d2); mesh->Normals[14] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[14] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[14] = float2(0.0f, 0.0f);
	mesh->Positions[15] = float3a(-w2, -h2, -d2); mesh->Normals[15] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[15] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[15] = float2(1.0f, 0.0f);

	// Fill in the left face float3a data.
	mesh->Positions[16] = float3a(-w2, -h2, -d2); mesh->Normals[16] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[16] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[16] = float2(0.0f, 1.0f);
	mesh->Positions[17] = float3a(-w2, +h2, -d2); mesh->Normals[17] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[17] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[17] = float2(0.0f, 0.0f);
	mesh->Positions[18] = float3a(-w2, +h2, +d2); mesh->Normals[18] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[18] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[18] = float2(1.0f, 0.0f);
	mesh->Positions[19] = float3a(-w2, -h2, +d2); mesh->Normals[19] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[19] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[19] = float2(1.0f, 1.0f);

	// Fill in the right face float3a data.
	mesh->Positions[20] = float3a(+w2, -h2, +d2); mesh->Normals[20] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[20] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[20] = float2(0.0f, 1.0f);
	mesh->Positions[21] = float3a(+w2, +h2, +d2); mesh->Normals[21] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[21] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[21] = float2(0.0f, 0.0f);
	mesh->Positions[22] = float3a(+w2, +h2, -d2); mesh->Normals[22] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[22] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[22] = float2(1.0f, 0.0f);
	mesh->Positions[23] = float3a(+w2, -h2, -d2); mesh->Normals[23] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[23] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[23] = float2(1.0f, 1.0f);


	// Create the index data
	mesh->Indices.resize(36);

	// Fill in the front face index data
	mesh->Indices[0] = 0;
	mesh->Indices[1] = 1;
	mesh->Indices[2] = 2;
	mesh->Indices[3] = 0;
	mesh->Indices[4] = 2;
	mesh->Indices[5] = 3;

	// Fill in the back face index data
	mesh->Indices[6] = 4;
	mesh->Indices[7] = 5;
	mesh->Indices[8] = 6;
	mesh->Indices[9] = 4;
	mesh->Indices[10] = 6;
	mesh->Indices[11] = 7;

	// Fill in the top face index data
	mesh->Indices[12] = 8;
	mesh->Indices[13] = 9;
	mesh->Indices[14] = 10;
	mesh->Indices[15] = 8;
	mesh->Indices[16] = 10;
	mesh->Indices[17] = 11;

	// Fill in the bottom face index data
	mesh->Indices[18] = 12;
	mesh->Indices[19] = 13;
	mesh->Indices[20] = 14;
	mesh->Indices[21] = 12;
	mesh->Indices[22] = 14;
	mesh->Indices[23] = 15;

	// Fill in the left face index data
	mesh->Indices[24] = 16;
	mesh->Indices[25] = 17;
	mesh->Indices[26] = 18;
	mesh->Indices[27] = 16;
	mesh->Indices[28] = 18;
	mesh->Indices[29] = 19;

	// Fill in the right face index data
	mesh->Indices[30] = 20;
	mesh->Indices[31] = 21;
	mesh->Indices[32] = 22;
	mesh->Indices[33] = 20;
	mesh->Indices[34] = 22;
	mesh->Indices[35] = 23;
}

void CreateSphere(float radius, uint sliceCount, uint stackCount, Mesh *mesh) {
	mesh->BoundingSphere = float4(0.0f, 0.0f, 0.0f, radius);
	
	// Compute the vertices stating at the top pole and moving down the stacks

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere

	// Push top pole
	mesh->Positions.emplace_back(0.0f, +radius, 0.0f);
	mesh->Normals.emplace_back(0.0f, +1.0f, 0.0f);
	mesh->Tangents.emplace_back(1.0f, 0.0f, 0.0f);
	mesh->TexCoords.emplace_back(0.0f, 0.0f);

	float phiStep = M_PI / stackCount;
	float thetaStep = 2.0f * M_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings)
	for (uint i = 1; i <= stackCount - 1; ++i) {
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint j = 0; j <= sliceCount; ++j) {
			float theta = j * thetaStep;

			// Spherical to cartesian
			float x = radius * std::sinf(phi) * std::sinf(theta);
			float y = radius * std::cosf(phi);
			float z = radius * std::sinf(phi) * std::cosf(theta);

			mesh->Positions.emplace_back(x, y, z);
			mesh->Normals.push_back(normalize(float3(x, y, z)));

			// Partial derivative of P with respect to theta
			float dx = radius * std::sinf(phi) * std::cosf(theta);
			float dy = radius * std::cosf(phi);
			float dz = radius * std::sinf(phi) * -std::sinf(theta);

			mesh->Tangents.push_back(normalize(float3(dx, dy, dz)));

			mesh->TexCoords.emplace_back(theta / (2.0f * (float)M_PI), phi / (float)M_PI);
		}
	}

	// Push the bottom pole
	mesh->Positions.emplace_back(0.0f, -radius, 0.0f);
	mesh->Normals.emplace_back(0.0f, -1.0f, 0.0f);
	mesh->Tangents.emplace_back(1.0f, 0.0f, 0.0f);
	mesh->TexCoords.emplace_back(0.0f, 1.0f);

	// Compute indices for top stack.  The top stack was written first to the float3a buffer
	// and connects the top pole to the first ring
	for (uint i = 1; i <= sliceCount; ++i) {
		mesh->Indices.push_back(0);
		mesh->Indices.push_back(i + 1);
		mesh->Indices.push_back(i);
	}

	// Compute indices for inner stacks (not connected to poles)

	// Offset the indices to the index of the first float3a in the first ring
	// This is just skipping the top pole float3a
	uint baseIndex = 1;
	uint ringfloat3aCount = sliceCount + 1;
	for (uint i = 0; i < stackCount - 2; ++i) {
		for (uint j = 0; j < sliceCount; ++j) {
			mesh->Indices.push_back(baseIndex + i * ringfloat3aCount + j);
			mesh->Indices.push_back(baseIndex + i * ringfloat3aCount + j + 1);
			mesh->Indices.push_back(baseIndex + (i + 1) * ringfloat3aCount + j);

			mesh->Indices.push_back(baseIndex + (i + 1) * ringfloat3aCount + j);
			mesh->Indices.push_back(baseIndex + i * ringfloat3aCount + j + 1);
			mesh->Indices.push_back(baseIndex + (i + 1) * ringfloat3aCount + j + 1);
		}
	}

	// Compute indices for bottom stack.  The bottom stack was written last to the float3a buffer
	// and connects the bottom pole to the bottom ring

	// South pole float3a was added last.
	uint southPoleIndex = (uint)mesh->Positions.size() - 1;

	// Offset the indices to the index of the first float3a in the last ring.
	baseIndex = southPoleIndex - ringfloat3aCount;

	for (uint i = 0; i < sliceCount; ++i) {
		mesh->Indices.push_back(southPoleIndex);
		mesh->Indices.push_back(baseIndex + i);
		mesh->Indices.push_back(baseIndex + i + 1);
	}
}

void Subdivide(Mesh *mesh) {
	// Save a copy of the input geometry
	Mesh inputCopy = *mesh;


	mesh->Positions.resize(0);
	mesh->Indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint numTris = inputCopy.Indices.size() / 3;
	for (uint i = 0; i < numTris; ++i) {
		float3a v0 = inputCopy.Positions[inputCopy.Indices[i * 3 + 0]];
		float3a v1 = inputCopy.Positions[inputCopy.Indices[i * 3 + 1]];
		float3a v2 = inputCopy.Positions[inputCopy.Indices[i * 3 + 2]];


		// Generate the midpoints
		// For subdivision, we just care about the position component
		// We derive the other float3a components in CreateGeosphere
		float3a m0(
			0.5f*(v0.x + v1.x),
			0.5f*(v0.y + v1.y),
			0.5f*(v0.z + v1.z));

		float3a m1(
			0.5f*(v1.x + v2.x),
			0.5f*(v1.y + v2.y),
			0.5f*(v1.z + v2.z));

		float3a m2(
			0.5f*(v0.x + v2.x),
			0.5f*(v0.y + v2.y),
			0.5f*(v0.z + v2.z));

		// Add new geometry.

		mesh->Positions.push_back(v0); // 0
		mesh->Positions.push_back(v1); // 1
		mesh->Positions.push_back(v2); // 2
		mesh->Positions.push_back(m0); // 3
		mesh->Positions.push_back(m1); // 4
		mesh->Positions.push_back(m2); // 5

		mesh->Indices.push_back(i * 6 + 0);
		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 5);

		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 4);
		mesh->Indices.push_back(i * 6 + 5);

		mesh->Indices.push_back(i * 6 + 5);
		mesh->Indices.push_back(i * 6 + 4);
		mesh->Indices.push_back(i * 6 + 2);

		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 1);
		mesh->Indices.push_back(i * 6 + 4);
	}
}

inline float QuadrantAwareArcTan(float y, float x) {
	float result;

	if (x >= 0.0f) {
		// Quadrant I or IV

		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		result = std::atanf(y / x); // in [-pi/2, +pi/2]

		if (result < 0.0f) {
			result += 2.0f * M_PI; // in [0, 2*pi).
		}
	} else {
		// Quadrant II or III

		result = std::atanf(y / x) + M_PI; // in [0, 2*pi).
	}

	return result;
}

void CreateGeosphere(float radius, uint numSubdivisions, Mesh *mesh) {
	mesh->BoundingSphere = float4(0.0f, 0.0f, 0.0f, radius);
	
	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	float3a pos[12] = {
		float3a(-X, 0.0f, -Z),  float3a(X, 0.0f, -Z),
		float3a(-X, 0.0f, Z), float3a(X, 0.0f, Z),
		float3a(0.0f, Z, -X),   float3a(0.0f, Z, X),
		float3a(0.0f, -Z, -X),  float3a(0.0f, -Z, X),
		float3a(Z, X, 0.0f),   float3a(-Z, X, 0.0f),
		float3a(Z, -X, 0.0f),  float3a(-Z, -X, 0.0f)
	};

	DWORD k[60] = {
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh->Positions.resize(12);
	mesh->Indices.resize(60);
	for (uint i = 0; i < 12; ++i) {
		mesh->Positions[i] = pos[i];
	}
	for (uint i = 0; i < 60; ++i) {
		mesh->Indices[i] = k[i];
	}

	// Tesselate
	for (uint i = 0; i < numSubdivisions; ++i) {
		Subdivide(mesh);
	}

	std::size_t numVertices = mesh->Positions.size();
	mesh->Normals.resize(numVertices);
	mesh->Tangents.resize(numVertices);
	mesh->TexCoords.resize(numVertices);

	// Project vertices onto sphere and scale.
	for (uint i = 0; i < numVertices; ++i) {
		// Project onto unit sphere.
		float3 n = normalize(mesh->Positions[i]);
		float3a p = float3a(radius * n, 1.0f);

		// Project onto sphere.
		mesh->Positions[i] = p;
		mesh->Normals[i] = n;

		// Derive texture coordinates from spherical coordinates.
		float theta = QuadrantAwareArcTan(p.x, p.z);
		float phi = std::acosf(p.y / radius);

		mesh->TexCoords[i].x = theta / (2.0f * M_PI);
		mesh->TexCoords[i].y = phi / M_PI;

		// Partial derivative of P with respect to theta
		float dx = radius * std::sinf(phi) * std::cosf(theta);
		float dy = radius * std::cosf(phi);
		float dz = radius * std::sinf(phi) * -std::sinf(theta);

		mesh->Tangents[i] = normalize(float3(dx, dy, dz));
	}
}

//void CreateCylinder(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount, mesh& mesh) {
//	mesh.Vertices.clear();
//	mesh.Indices.clear();
//
//	//
//	// Build Stacks.
//	//
//
//	float stackHeight = height / stackCount;
//
//	// Amount to increment radius as we move up each stack level from bottom to top.
//	float radiusStep = (topRadius - bottomRadius) / stackCount;
//
//	uint ringCount = stackCount + 1;
//
//	// Compute vertices for each stack ring starting at the bottom and moving up.
//	for (uint i = 0; i < ringCount; ++i) {
//		float y = -0.5f*height + i*stackHeight;
//		float r = bottomRadius + i*radiusStep;
//
//		// vertices of ring
//		float dTheta = 2.0f*XM_PI / sliceCount;
//		for (uint j = 0; j <= sliceCount; ++j) {
//			float3a vertex;
//
//			float c = cosf(j*dTheta);
//			float s = sinf(j*dTheta);
//
//			vertex.Position = float3a(r*c, y, r*s);
//
//			vertex.TexC.x = (float)j / sliceCount;
//			vertex.TexC.y = 1.0f - (float)i / stackCount;
//
//			// Cylinder can be parameterized as follows, where we introduce v
//			// parameter that goes in the same direction as the v tex-coord
//			// so that the bitangent goes in the same direction as the v tex-coord.
//			//   Let r0 be the bottom radius and let r1 be the top radius.
//			//   y(v) = h - hv for v in [0,1].
//			//   r(v) = r1 + (r0-r1)v
//			//
//			//   x(t, v) = r(v)*cos(t)
//			//   y(t, v) = h - hv
//			//   z(t, v) = r(v)*sin(t)
//			//
//			//  dx/dt = -r(v)*sin(t)
//			//  dy/dt = 0
//			//  dz/dt = +r(v)*cos(t)
//			//
//			//  dx/dv = (r0-r1)*cos(t)
//			//  dy/dv = -h
//			//  dz/dv = (r0-r1)*sin(t)
//
//			// This is unit length.
//			vertex.TangentU = float3a(-s, 0.0f, c);
//
//			float dr = bottomRadius - topRadius;
//			float3a bitangent(dr*c, -height, dr*s);
//
//			XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
//			XMVECTOR B = XMLoadFloat3(&bitangent);
//			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
//			XMStoreFloat3(&vertex.Normal, N);
//
//			mesh.Vertices.push_back(vertex);
//		}
//	}
//
//	// Add one because we duplicate the first and last float3a per ring
//	// since the texture coordinates are different.
//	uint ringfloat3aCount = sliceCount + 1;
//
//	// Compute indices for each stack.
//	for (uint i = 0; i < stackCount; ++i) {
//		for (uint j = 0; j < sliceCount; ++j) {
//			mesh.Indices.push_back(i*ringVertexCount + j);
//			mesh.Indices.push_back((i + 1)*ringVertexCount + j);
//			mesh.Indices.push_back((i + 1)*ringVertexCount + j + 1);
//
//			mesh.Indices.push_back(i*ringVertexCount + j);
//			mesh.Indices.push_back((i + 1)*ringVertexCount + j + 1);
//			mesh.Indices.push_back(i*ringVertexCount + j + 1);
//		}
//	}
//
//	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
//	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
//}
//
//void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
//											uint sliceCount, uint stackCount, mesh& mesh) {
//	uint baseIndex = (uint)mesh.Vertices.size();
//
//	float y = 0.5f*height;
//	float dTheta = 2.0f*XM_PI / sliceCount;
//
//	// Duplicate cap ring vertices because the texture coordinates and normals differ.
//	for (uint i = 0; i <= sliceCount; ++i) {
//		float x = topRadius*cosf(i*dTheta);
//		float z = topRadius*sinf(i*dTheta);
//
//		// Scale down by the height to try and make top cap texture coord area
//		// proportional to base.
//		float u = x / height + 0.5f;
//		float v = z / height + 0.5f;
//
//		mesh.Vertices.push_back(float3a(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
//	}
//
//	// Cap center vertex.
//	mesh.Vertices.push_back(float3a(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
//
//	// Index of center vertex.
//	uint centerIndex = (uint)mesh.Vertices.size() - 1;
//
//	for (uint i = 0; i < sliceCount; ++i) {
//		mesh.Indices.push_back(centerIndex);
//		mesh.Indices.push_back(baseIndex + i + 1);
//		mesh.Indices.push_back(baseIndex + i);
//	}
//}
//
//void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
//											   uint sliceCount, uint stackCount, mesh& mesh) {
//	//
//	// Build bottom cap.
//	//
//
//	uint baseIndex = (uint)mesh.Vertices.size();
//	float y = -0.5f*height;
//
//	// vertices of ring
//	float dTheta = 2.0f*XM_PI / sliceCount;
//	for (uint i = 0; i <= sliceCount; ++i) {
//		float x = bottomRadius*cosf(i*dTheta);
//		float z = bottomRadius*sinf(i*dTheta);
//
//		// Scale down by the height to try and make top cap texture coord area
//		// proportional to base.
//		float u = x / height + 0.5f;
//		float v = z / height + 0.5f;
//
//		mesh.Vertices.push_back(float3a(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
//	}
//
//	// Cap center vertex.
//	mesh.Vertices.push_back(float3a(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
//
//	// Cache the index of center vertex.
//	uint centerIndex = (uint)mesh.Vertices.size() - 1;
//
//	for (uint i = 0; i < sliceCount; ++i) {
//		mesh.Indices.push_back(centerIndex);
//		mesh.Indices.push_back(baseIndex + i);
//		mesh.Indices.push_back(baseIndex + i + 1);
//	}
//}

void CreateGrid(float width, float depth, uint m, uint n, Mesh *mesh) {
	uint vertexCount = m * n;
	uint faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	mesh->BoundingSphere = float4(0.0f, 0.0f, 0.0f, std::max(halfWidth, halfDepth));

	// Create the vertices.
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	mesh->Positions.resize(vertexCount);
	mesh->Normals.resize(vertexCount);
	mesh->Tangents.resize(vertexCount);
	mesh->TexCoords.resize(vertexCount);

	for (uint i = 0; i < m; ++i) {
		float z = halfDepth - i * dz;
		for (uint j = 0; j < n; ++j) {
			float x = -halfWidth + j * dx;

			mesh->Positions[i * n + j] = float3a(x, 0.0f, -z, 1.0f);
			mesh->Normals[i * n + j] = float3(0.0f, 1.0f, 0.0f);
			mesh->Tangents[i * n + j] = float3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			mesh->TexCoords[i * n + j] = float2(j * du, i * dv);
		}
	}

	// Create the indices.
	mesh->Indices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	uint k = 0;
	for (uint i = 0; i < m - 1; ++i) {
		for (uint j = 0; j < n - 1; ++j) {
			mesh->Indices[k] = i * n + j;
			mesh->Indices[k + 1] = i * n + j + 1;
			mesh->Indices[k + 2] = (i + 1) * n + j;

			mesh->Indices[k + 3] = (i + 1) * n + j;
			mesh->Indices[k + 4] = i * n + j + 1;
			mesh->Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}
}

} // End of namespace Lantern
