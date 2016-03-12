//***************************************************************************************
// GeometryGenerator.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

/* Modified for Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "common/typedefs.h"

#include "scene/geometry_generator.h"

#include <algorithm>
#include <cmath>


namespace Lantern {

void CreateBox(float width, float height, float depth, Mesh *mesh) {
	// Create the vertex data
	mesh->Vertices.resize(24);
	mesh->Normals.resize(24);
	mesh->Tangents.resize(24);
	mesh->TexCoords.resize(24);

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	mesh->Vertices[0] = Vertex(-w2, -h2, +d2, 1.0f); mesh->Normals[0] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[0] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[0] = float2(0.0f, 1.0f);
	mesh->Vertices[1] = Vertex(-w2, +h2, +d2, 1.0f); mesh->Normals[1] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[1] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[1] = float2(0.0f, 0.0f);
	mesh->Vertices[2] = Vertex(+w2, +h2, +d2, 1.0f); mesh->Normals[2] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[2] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[2] = float2(1.0f, 0.0f);
	mesh->Vertices[3] = Vertex(+w2, -h2, +d2, 1.0f); mesh->Normals[3] = float3(0.0f, 0.0f, 1.0f); mesh->Tangents[3] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[3] = float2(1.0f, 1.0f);


	// Fill in the back face vertex data.
	mesh->Vertices[4] = Vertex(-w2, -h2, -d2, 1.0f); mesh->Normals[4] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[4] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[4] = float2(1.0f, 1.0f);
	mesh->Vertices[5] = Vertex(+w2, -h2, -d2, 1.0f); mesh->Normals[5] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[5] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[5] = float2(0.0f, 1.0f);
	mesh->Vertices[6] = Vertex(+w2, +h2, -d2, 1.0f); mesh->Normals[6] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[6] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[6] = float2(0.0f, 0.0f);
	mesh->Vertices[7] = Vertex(-w2, +h2, -d2, 1.0f); mesh->Normals[7] = float3(0.0f, 0.0f, -1.0f); mesh->Tangents[7] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[7] = float2(1.0f, 0.0f);

	// Fill in the top face vertex data.
	mesh->Vertices[8] = Vertex(-w2, +h2, +d2, 1.0f); mesh->Normals[8] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[8] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[8] = float2(0.0f, 1.0f);
	mesh->Vertices[9] = Vertex(-w2, +h2, -d2, 1.0f); mesh->Normals[9] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[9] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[9] = float2(0.0f, 0.0f);
	mesh->Vertices[10] = Vertex(+w2, +h2, -d2, 1.0f); mesh->Normals[10] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[10] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[10] = float2(1.0f, 0.0f);
	mesh->Vertices[11] = Vertex(+w2, +h2, +d2, 1.0f); mesh->Normals[11] = float3(0.0f, 1.0f, 0.0f); mesh->Tangents[11] = float3(1.0f, 0.0f, 0.0f); mesh->TexCoords[11] = float2(1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	mesh->Vertices[12] = Vertex(-w2, -h2, +d2, 1.0f); mesh->Normals[12] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[12] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[12] = float2(1.0f, 1.0f);
	mesh->Vertices[13] = Vertex(+w2, -h2, +d2, 1.0f); mesh->Normals[13] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[13] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[13] = float2(0.0f, 1.0f);
	mesh->Vertices[14] = Vertex(+w2, -h2, -d2, 1.0f); mesh->Normals[14] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[14] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[14] = float2(0.0f, 0.0f);
	mesh->Vertices[15] = Vertex(-w2, -h2, -d2, 1.0f); mesh->Normals[15] = float3(0.0f, -1.0f, 0.0f); mesh->Tangents[15] = float3(-1.0f, 0.0f, 0.0f); mesh->TexCoords[15] = float2(1.0f, 0.0f);

	// Fill in the left face vertex data.
	mesh->Vertices[16] = Vertex(-w2, -h2, -d2, 1.0f); mesh->Normals[16] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[16] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[16] = float2(0.0f, 1.0f);
	mesh->Vertices[17] = Vertex(-w2, +h2, -d2, 1.0f); mesh->Normals[17] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[17] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[17] = float2(0.0f, 0.0f);
	mesh->Vertices[18] = Vertex(-w2, +h2, +d2, 1.0f); mesh->Normals[18] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[18] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[18] = float2(1.0f, 0.0f);
	mesh->Vertices[19] = Vertex(-w2, -h2, +d2, 1.0f); mesh->Normals[19] = float3(-1.0f, 0.0f, 0.0f); mesh->Tangents[19] = float3(0.0f, 0.0f, 1.0f); mesh->TexCoords[19] = float2(1.0f, 1.0f);

	// Fill in the right face vertex data.
	mesh->Vertices[20] = Vertex(+w2, -h2, +d2, 1.0f); mesh->Normals[20] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[20] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[20] = float2(0.0f, 1.0f);
	mesh->Vertices[21] = Vertex(+w2, +h2, +d2, 1.0f); mesh->Normals[21] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[21] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[21] = float2(0.0f, 0.0f);
	mesh->Vertices[22] = Vertex(+w2, +h2, -d2, 1.0f); mesh->Normals[22] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[22] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[22] = float2(1.0f, 0.0f);
	mesh->Vertices[23] = Vertex(+w2, -h2, -d2, 1.0f); mesh->Normals[23] = float3(1.0f, 0.0f, 0.0f); mesh->Tangents[23] = float3(0.0f, 0.0f, -1.0f); mesh->TexCoords[23] = float2(1.0f, 1.0f);


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
	// Compute the vertices stating at the top pole and moving down the stacks

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere

	// Push top pole
	mesh->Vertices.emplace_back(0.0f, +radius, 0.0f, 1.0f);
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

			mesh->Vertices.emplace_back(x, y, z, 1.0f);
			mesh->Normals.push_back(normalize(float3(x, y, z)));

			// Partial derivative of P with respect to theta
			float dx = radius * std::sinf(phi) * std::cosf(theta);
			float dy = radius * std::cosf(phi);
			float dz = radius * std::sinf(phi) * -std::sinf(theta);

			mesh->Tangents.push_back(normalize(float3(dx, dy, dz)));

			mesh->TexCoords.emplace_back(theta / (2.0f * M_PI), phi / M_PI);
		}
	}

	// Push the bottom pole
	mesh->Vertices.emplace_back(0.0f, -radius, 0.0f, 1.0f);
	mesh->Normals.emplace_back(0.0f, -1.0f, 0.0f);
	mesh->Tangents.emplace_back(1.0f, 0.0f, 0.0f);
	mesh->TexCoords.emplace_back(0.0f, 1.0f);

	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring
	for (uint i = 1; i <= sliceCount; ++i) {
		mesh->Indices.push_back(0);
		mesh->Indices.push_back(i + 1);
		mesh->Indices.push_back(i);
	}

	// Compute indices for inner stacks (not connected to poles)

	// Offset the indices to the index of the first vertex in the first ring
	// This is just skipping the top pole vertex
	uint baseIndex = 1;
	uint ringVertexCount = sliceCount + 1;
	for (uint i = 0; i < stackCount - 2; ++i) {
		for (uint j = 0; j < sliceCount; ++j) {
			mesh->Indices.push_back(baseIndex + i * ringVertexCount + j);
			mesh->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			mesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			mesh->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring

	// South pole vertex was added last.
	uint southPoleIndex = (uint)mesh->Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint i = 0; i < sliceCount; ++i) {
		mesh->Indices.push_back(southPoleIndex);
		mesh->Indices.push_back(baseIndex + i);
		mesh->Indices.push_back(baseIndex + i + 1);
	}
}

void Subdivide(Mesh *mesh) {
	// Save a copy of the input geometry
	Mesh inputCopy = *mesh;


	mesh->Vertices.resize(0);
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
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];


		// Generate the midpoints
		// For subdivision, we just care about the position component
		// We derive the other vertex components in CreateGeosphere
		Vertex m0(
			0.5f*(v0.x + v1.x),
			0.5f*(v0.y + v1.y),
			0.5f*(v0.z + v1.z),
			1.0f);

		Vertex m1(
			0.5f*(v1.x + v2.x),
			0.5f*(v1.y + v2.y),
			0.5f*(v1.z + v2.z),
			1.0f);

		Vertex m2(
			0.5f*(v0.x + v2.x),
			0.5f*(v0.y + v2.y),
			0.5f*(v0.z + v2.z),
			1.0f);

		// Add new geometry.

		mesh->Vertices.push_back(v0); // 0
		mesh->Vertices.push_back(v1); // 1
		mesh->Vertices.push_back(v2); // 2
		mesh->Vertices.push_back(m0); // 3
		mesh->Vertices.push_back(m1); // 4
		mesh->Vertices.push_back(m2); // 5

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
	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	Vertex pos[12] = {
		Vertex(-X, 0.0f, -Z, 1.0f),  Vertex(X, 0.0f, -Z, 1.0f),
		Vertex(-X, 0.0f, Z, 1.0f), Vertex(X, 0.0f, Z, 1.0f),
		Vertex(0.0f, Z, -X, 1.0f),   Vertex(0.0f, Z, X, 1.0f),
		Vertex(0.0f, -Z, -X, 1.0f),  Vertex(0.0f, -Z, X, 1.0f),
		Vertex(Z, X, 0.0f, 1.0f),   Vertex(-Z, X, 0.0f, 1.0f),
		Vertex(Z, -X, 0.0f, 1.0f),  Vertex(-Z, -X, 0.0f, 1.0f)
	};

	DWORD k[60] = {
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh->Vertices.resize(12);
	mesh->Indices.resize(60);
	for (uint i = 0; i < 12; ++i) {
		mesh->Vertices[i] = pos[i];
	}
	for (uint i = 0; i < 60; ++i) {
		mesh->Indices[i] = k[i];
	}

	// Tesselate
	for (uint i = 0; i < numSubdivisions; ++i) {
		Subdivide(mesh);
	}

	std::size_t numVertices = mesh->Vertices.size();
	mesh->Normals.resize(numVertices);
	mesh->Tangents.resize(numVertices);
	mesh->TexCoords.resize(numVertices);

	// Project vertices onto sphere and scale.
	for (uint i = 0; i < numVertices; ++i) {
		// Project onto unit sphere.
		float3 n = normalize(mesh->Vertices[i].xyz());
		Vertex p = Vertex(radius * n, 1.0f);

		// Project onto sphere.
		mesh->Vertices[i] = p;
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
//			Vertex Vertex;
//
//			float c = cosf(j*dTheta);
//			float s = sinf(j*dTheta);
//
//			Vertex.Position = Vertex(r*c, y, r*s);
//
//			Vertex.TexC.x = (float)j / sliceCount;
//			Vertex.TexC.y = 1.0f - (float)i / stackCount;
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
//			Vertex.TangentU = Vertex(-s, 0.0f, c);
//
//			float dr = bottomRadius - topRadius;
//			Vertex bitangent(dr*c, -height, dr*s);
//
//			XMVECTOR T = XMLoadFloat3(&Vertex.TangentU);
//			XMVECTOR B = XMLoadFloat3(&bitangent);
//			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
//			XMStoreFloat3(&Vertex.Normal, N);
//
//			mesh.Vertices.push_back(Vertex);
//		}
//	}
//
//	// Add one because we duplicate the first and last Vertex per ring
//	// since the texture coordinates are different.
//	uint ringVertexCount = sliceCount + 1;
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
//		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
//	}
//
//	// Cap center Vertex.
//	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
//
//	// Index of center Vertex.
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
//		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
//	}
//
//	// Cap center Vertex.
//	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
//
//	// Cache the index of center Vertex.
//	uint centerIndex = (uint)mesh.Vertices.size() - 1;
//
//	for (uint i = 0; i < sliceCount; ++i) {
//		mesh.Indices.push_back(centerIndex);
//		mesh.Indices.push_back(baseIndex + i);
//		mesh.Indices.push_back(baseIndex + i + 1);
//	}
//}
//
//void CreateGrid(float width, float depth, uint m, uint n, mesh& mesh) {
//	uint VertexCount = m*n;
//	uint faceCount = (m - 1)*(n - 1) * 2;
//
//	//
//	// Create the vertices.
//	//
//
//	float halfWidth = 0.5f*width;
//	float halfDepth = 0.5f*depth;
//
//	float dx = width / (n - 1);
//	float dz = depth / (m - 1);
//
//	float du = 1.0f / (n - 1);
//	float dv = 1.0f / (m - 1);
//
//	mesh.Vertices.resize(VertexCount);
//	for (uint i = 0; i < m; ++i) {
//		float z = halfDepth - i*dz;
//		for (uint j = 0; j < n; ++j) {
//			float x = -halfWidth + j*dx;
//
//			mesh.Vertices[i*n + j].Position = Vertex(x, 0.0f, z);
//			mesh.Vertices[i*n + j].Normal = Vertex(0.0f, 1.0f, 0.0f);
//			mesh.Vertices[i*n + j].TangentU = Vertex(1.0f, 0.0f, 0.0f);
//
//			// Stretch texture over grid.
//			mesh.Vertices[i*n + j].TexC.x = j*du;
//			mesh.Vertices[i*n + j].TexC.y = i*dv;
//		}
//	}
//
//	//
//	// Create the indices.
//	//
//
//	mesh.Indices.resize(faceCount * 3); // 3 indices per face
//
//											// Iterate over each quad and compute indices.
//	uint k = 0;
//	for (uint i = 0; i < m - 1; ++i) {
//		for (uint j = 0; j < n - 1; ++j) {
//			mesh.Indices[k] = i*n + j;
//			mesh.Indices[k + 1] = i*n + j + 1;
//			mesh.Indices[k + 2] = (i + 1)*n + j;
//
//			mesh.Indices[k + 3] = (i + 1)*n + j;
//			mesh.Indices[k + 4] = i*n + j + 1;
//			mesh.Indices[k + 5] = (i + 1)*n + j + 1;
//
//			k += 6; // next quad
//		}
//	}
//}
//
//void CreateFullscreenQuad(mesh& mesh) {
//	mesh.Vertices.resize(4);
//	mesh.Indices.resize(6);
//
//	// Position coordinates specified in NDC space.
//	mesh.Vertices[0] = Vertex(
//		-1.0f, -1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f,
//		1.0f, 0.0f, 0.0f,
//		0.0f, 1.0f);
//
//	mesh.Vertices[1] = Vertex(
//		-1.0f, +1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f,
//		1.0f, 0.0f, 0.0f,
//		0.0f, 0.0f);
//
//	mesh.Vertices[2] = Vertex(
//		+1.0f, +1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f,
//		1.0f, 0.0f, 0.0f,
//		1.0f, 0.0f);
//
//	mesh.Vertices[3] = Vertex(
//		+1.0f, -1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f,
//		1.0f, 0.0f, 0.0f,
//		1.0f, 1.0f);
//
//	mesh.Indices[0] = 0;
//	mesh.Indices[1] = 1;
//	mesh.Indices[2] = 2;
//
//	mesh.Indices[3] = 0;
//	mesh.Indices[4] = 2;
//	mesh.Indices[5] = 3;
//}

} // End of namespace Lantern
