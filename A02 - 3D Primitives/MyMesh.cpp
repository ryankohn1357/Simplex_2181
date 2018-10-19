#include "MyMesh.h"
#include <iostream>

void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// calculate the angle per subdivision
	float anglePerSubdivision = 360.0f / a_nSubdivisions;
	std::vector<vector3> edgePoints;
	vector3 center(0, 0, 0);
	vector3 top = vector3(0, 0, -1 * a_fHeight);

	// for the base, at each subdivision angle, calculate the x and y coordinates of the vertices using sin and cos
	for (float i = 0.0f; i <= 360.0f; i += anglePerSubdivision)
	{
		float radians = (i * PI) / 180;
		vector3 edgePoint(cos(radians) * a_fRadius, sin(radians) * a_fRadius, 0);
		edgePoints.push_back(edgePoint);
	}

	// create all the tris
	for (int i = 0; i < edgePoints.size() - 1; i++)
	{
		AddTri(edgePoints[i + 1], edgePoints[i], center);
		AddTri(edgePoints[i], edgePoints[i + 1], top);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// calculate the angle per subdivision
	float anglePerSubdivision = 360.0f / a_nSubdivisions;
	std::vector<vector3> bottomEdgePoints;
	std::vector<vector3> topEdgePoints;

	vector3 bottomCenter(0.0f, 0.0f, 0.0f);
	vector3 topCenter = vector3(0.0f, 0.0f, -1 * a_fHeight);

	// for the top and bottom circles, at each subdivision angle, calculate the x and y coordinates of the vertices using sin and cos
	for (float i = 0.0f; i <= 360.0f; i += anglePerSubdivision)
	{
		float radians = (i * PI) / 180;
		vector3 bottomEdgePoint(cos(radians) * a_fRadius, sin(radians) * a_fRadius, 0);
		vector3 topEdgePoint(cos(radians) * a_fRadius, sin(radians) * a_fRadius, -1 * a_fHeight);
		bottomEdgePoints.push_back(bottomEdgePoint);
		topEdgePoints.push_back(topEdgePoint);
	}

	// create all the tris and quads
	for (int i = 0; i < bottomEdgePoints.size() - 1; i++)
	{
		AddTri(topEdgePoints[i], topCenter, topEdgePoints[i + 1]); // top circle
		AddQuad(bottomEdgePoints[i + 1], bottomEdgePoints[i], topEdgePoints[i + 1], topEdgePoints[i]); // connector
		AddTri(bottomEdgePoints[i], bottomEdgePoints[i + 1], bottomCenter); // bottom circle
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// calculate the angle per subdivision
	float anglePerSubdivision = 360.0f / a_nSubdivisions;
	std::vector<vector3> bOEdgePoints; // bottom outer circle
	std::vector<vector3> tOEdgePoints; // top outer circle
	std::vector<vector3> bIEdgePoints; // bottom inner circle
	std::vector<vector3> tIEdgePoints; // top inner circle

	// for each circle, at each subdivision angle, calculate the x and y coordinates of the vertices using sin and cos
	for (float i = 0.0f; i <= 360.0f; i += anglePerSubdivision)
	{
		float radians = (i * PI) / 180;
		vector3 bOEdgePoint(cos(radians) * a_fOuterRadius, sin(radians) * a_fOuterRadius, 0);
		vector3 tOEdgePoint(cos(radians) * a_fOuterRadius, sin(radians) * a_fOuterRadius, -1 * a_fHeight);
		vector3 bIEdgePoint(cos(radians) * a_fInnerRadius, sin(radians) * a_fInnerRadius, 0);
		vector3 tIEdgePoint(cos(radians) * a_fInnerRadius, sin(radians) * a_fInnerRadius, -1 * a_fHeight);

		bOEdgePoints.push_back(bOEdgePoint);
		tOEdgePoints.push_back(tOEdgePoint);
		bIEdgePoints.push_back(bIEdgePoint);
		tIEdgePoints.push_back(tIEdgePoint);
	}

	// create quads using the vertices
	for (int i = 0; i < bOEdgePoints.size() - 1; i++)
	{
		AddQuad(bIEdgePoints[i], bIEdgePoints[i + 1], tIEdgePoints[i], tIEdgePoints[i + 1]); // inside connector
		AddQuad(bOEdgePoints[i], bOEdgePoints[i + 1], bIEdgePoints[i], bIEdgePoints[i + 1]); // bottom
		AddQuad(bOEdgePoints[i + 1], bOEdgePoints[i], tOEdgePoints[i + 1], tOEdgePoints[i]); // outside connector
		AddQuad(tOEdgePoints[i + 1], tOEdgePoints[i], tIEdgePoints[i + 1], tIEdgePoints[i]); // top
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();


	/* **********
	Create a subdivided cube
	*************/

	float quadLength = (2.0f * a_fRadius) / (a_nSubdivisions + 1); // how big each of the faces on a side of the cube are
	std::vector<vector3> vertices;

	// front side
	vector3 startVertex = vector3(-1 * a_fRadius, -1 * a_fRadius, a_fRadius); // start at the bottom left corner of the side

	// going row by row, calculate the position of each vertex and add it to vertices
	// vertices on each face are added in the order bottom left -> bottom right -> top left -> top right
	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x + i * quadLength, startVertex.y + j * quadLength, startVertex.z);
			vector3 bottomRight = vector3(startVertex.x + (i + 1) * quadLength, startVertex.y + j * quadLength, startVertex.z);
			vector3 topLeft = vector3(startVertex.x + i * quadLength, startVertex.y + (j + 1) * quadLength, startVertex.z);
			vector3 topRight = vector3(startVertex.x + (i + 1) * quadLength, startVertex.y + (j + 1) * quadLength, startVertex.z);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}
	
	// top side
	startVertex = vector3(a_fRadius, a_fRadius, a_fRadius);

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x - j * quadLength, startVertex.y, startVertex.z - i * quadLength);
			vector3 bottomRight = vector3(startVertex.x - j * quadLength, startVertex.y, startVertex.z - (i + 1) * quadLength);
			vector3 topLeft = vector3(startVertex.x - (j + 1) * quadLength, startVertex.y, startVertex.z - i * quadLength);
			vector3 topRight = vector3(startVertex.x - (j + 1) * quadLength, startVertex.y, startVertex.z - (i + 1) * quadLength);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}
	
	// back side
	startVertex = vector3(a_fRadius, -1 * a_fRadius, -1 * a_fRadius);

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x - i * quadLength, startVertex.y + j * quadLength, startVertex.z);
			vector3 bottomRight = vector3(startVertex.x - (i + 1) * quadLength, startVertex.y + j * quadLength, startVertex.z);
			vector3 topLeft = vector3(startVertex.x - i * quadLength, startVertex.y + (j + 1) * quadLength, startVertex.z);
			vector3 topRight = vector3(startVertex.x - (i + 1) * quadLength, startVertex.y + (j + 1) * quadLength, startVertex.z);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}

	// left side
	startVertex = vector3(-1 * a_fRadius, -1 * a_fRadius, -1 * a_fRadius);

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x, startVertex.y + j * quadLength, startVertex.z + i * quadLength);
			vector3 bottomRight = vector3(startVertex.x, startVertex.y + j * quadLength, startVertex.z + (i + 1) * quadLength);
			vector3 topLeft = vector3(startVertex.x, startVertex.y + (j + 1) * quadLength, startVertex.z + i * quadLength);
			vector3 topRight = vector3(startVertex.x, startVertex.y + (j + 1) * quadLength, startVertex.z + (i + 1) * quadLength);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}
	
	// bottom side
	startVertex = vector3(-1 * a_fRadius, -1 * a_fRadius, a_fRadius);

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x + j * quadLength, startVertex.y, startVertex.z - i * quadLength);
			vector3 bottomRight = vector3(startVertex.x + j * quadLength, startVertex.y, startVertex.z - (i + 1) * quadLength);
			vector3 topLeft = vector3(startVertex.x + (j + 1) * quadLength, startVertex.y, startVertex.z - i * quadLength);
			vector3 topRight = vector3(startVertex.x + (j + 1) * quadLength, startVertex.y, startVertex.z - (i + 1) * quadLength);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}
	
	// right side
	startVertex = vector3(a_fRadius, -1 * a_fRadius, a_fRadius);

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			vector3 bottomLeft = vector3(startVertex.x, startVertex.y + j * quadLength, startVertex.z - i * quadLength);
			vector3 bottomRight = vector3(startVertex.x, startVertex.y + j * quadLength, startVertex.z - (i + 1) * quadLength);
			vector3 topLeft = vector3(startVertex.x, startVertex.y + (j + 1) * quadLength, startVertex.z - i * quadLength);
			vector3 topRight = vector3(startVertex.x, startVertex.y + (j + 1) * quadLength, startVertex.z - (i + 1) * quadLength);
			vertices.push_back(bottomLeft);
			vertices.push_back(bottomRight);
			vertices.push_back(topLeft);
			vertices.push_back(topRight);
		}
	}
	
	// position each vertex so they are radius units away from the center of the sphere
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = normalize(vertices[i]) * a_fRadius;
	}
	
	// add all the vertices in groups of four as quads
	for (int i = 0; i <= vertices.size() - 4; i += 4)
	{
		AddQuad(vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3]);
	}

	
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}