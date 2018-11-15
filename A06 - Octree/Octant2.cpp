#include "Octant2.h"
using namespace Simplex;
//  Octant2
void Octant2::Init(void)
{
	m_nData = 0;
	dimension = 1;
	level = 1;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	typedef MyEntity* PEntity; //MyEntity Pointer
	PEntity* entity_Array = m_pEntityMngr->GetEntityArray();
	
	uint iEntityCount = m_pEntityMngr->GetEntityCount();
	std::vector<vector3> v3MaxMin_list;
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = entity_Array[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
	}
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	CalcTree();
}
void Octant2::Swap(Octant2& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void Octant2::Release(void)
{
	m_lData.clear();
}
void Simplex::Octant2::Display(void)
{
	m_pRigidBody->AddToRenderList();
	for (int i = 0; i < 8; i++)
	{
		if (m_pChild[i] != nullptr)
		{
			m_pChild[i]->Display();
		}
	}
}

void Octant2::Subdivide()
{
	if (m_pChild[0] != nullptr)
	{
		return;
	}

	vector3 min = m_pRigidBody->GetMinGlobal();
	vector3 max = m_pRigidBody->GetMaxGlobal();
	vector3 center = m_pRigidBody->GetCenterGlobal();

	for (uint i = 0; i < 8; i++)
	{
		vector3 subMin;
		vector3 subMax;
		switch (i)
		{
		case 0: // front top left
			subMin = vector3(min.x, center.y, min.z); 
			subMax = vector3(center.x, max.y, center.z);
			break;
		case 1: // back top left
			subMin = vector3(min.x, center.y, center.z);
			subMax = vector3(center.x, max.y, max.z);
			break;
		case 2: // front bottom left
			subMin = vector3(min.x, min.y, min.z);
			subMax = vector3(center.x, center.y, center.z);
			break;
		case 3: // back bottom left
			subMin = vector3(min.x, min.y, center.z);
			subMax = vector3(center.x, center.y, max.z);
			break;
		case 4: // front top right
			subMin = vector3(center.x, center.y, min.z);
			subMax = vector3(max.x, max.y, center.z);
			break;
		case 5: // back top right
			subMin = vector3(center.x, center.y, center.z);
			subMax = vector3(max.x, max.y, max.z);
			break;
		case 6: // front bottom right
			subMin = vector3(center.x, min.y, min.z);
			subMax = vector3(max.x, center.y, center.z);
			break;
		case 7: // back bottom right
			subMin = vector3(center.x, min.y, center.z);
			subMax = vector3(max.x, center.y, max.z);
			break;
		default:
			break;
		}

		std::vector<vector3> minMaxList;
		minMaxList.push_back(subMin);
		minMaxList.push_back(subMax);

		int nextLevel = level++;
		int newDimension = std::stoi((std::to_string(dimension) + std::to_string(i)));

		m_pChild[i] = new Octant2(newDimension, nextLevel);
		m_pChild[i]->m_pRigidBody = new MyRigidBody(minMaxList);
	}
}

void Octant2::CalcTree(void)
{
	typedef MyEntity* PEntity; //MyEntity Pointer
	PEntity* entity_Array = m_pEntityMngr->GetEntityArray();
	uint iEntityCount = m_pEntityMngr->GetEntityCount();
	std::vector<MyEntity*> collidingEntities;

	for (int i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRB = entity_Array[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			entity_Array[i]->AddDimension(dimension);
			collidingEntities.push_back(entity_Array[i]);
		}
	}

	if (collidingEntities.size() > 1 && level < 4)
	{
		Subdivide();
		for (int i = 0; i < 8; i++)
		{
			m_pChild[i]->CalcTree(collidingEntities);
		}
	}
}

void Octant2::CalcTree(std::vector<MyEntity*> colEntities)
{
	std::vector<MyEntity*> collidingEntities;

	for (int i = 0; i < colEntities.size(); i++)
	{
		MyRigidBody* pRB = colEntities[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			colEntities[i]->AddDimension(dimension);
			collidingEntities.push_back(colEntities[i]);
		}
	}

	if (collidingEntities.size() > 1 && level < 4)
	{
		Subdivide();
		for (int i = 0; i < 8; i++)
		{
			m_pChild[i]->CalcTree(collidingEntities);
		}
	}
}

Octant2::Octant2(int dim, int lev)
{
	dimension = dim;
	level = lev;
}

//The big 3
Octant2::Octant2(){Init();}
Octant2::Octant2(Octant2 const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
Octant2& Octant2::operator=(Octant2 const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		Octant2 temp(other);
		Swap(temp);
	}
	return *this;
}
Octant2::~Octant2(){Release();};
//Accessors
void Octant2::SetData(int a_nData){ m_nData = a_nData; }
int Octant2::GetData(void){ return m_nData; }
void Octant2::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& Octant2::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

