#include "Octant2.h"
using namespace Simplex;
//  Octant2
void Octant2::Init(void)
{
	m_nData = 0;

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}
void Octant2::Swap(Octant2& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void Octant2::Release(void)
{
	m_lData.clear();
	SafeDelete(m_pRigidBody);
	for (uint i = 0; i < 8; i++)
	{
		SafeDelete(m_pChild[i]);
	}
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
	// calculate center and size of children octants (dividing the parent into eight suboctants)
	vector3 v3Center = m_pRigidBody->GetCenterLocal();
	vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();
	float fSize = v3HalfWidth.x / 2.0f;
	float fCenters = fSize;

	// create children octants
	m_pChild[0] = new Octant2(v3Center + vector3(fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new Octant2(v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new Octant2(v3Center + vector3(-fCenters, -fCenters, fCenters), fSize);
	m_pChild[3] = new Octant2(v3Center + vector3(fCenters, -fCenters, fCenters), fSize);

	m_pChild[4] = new Octant2(v3Center + vector3(fCenters, fCenters, -fCenters), fSize);
	m_pChild[5] = new Octant2(v3Center + vector3(-fCenters, fCenters, -fCenters), fSize);
	m_pChild[6] = new Octant2(v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize);
	m_pChild[7] = new Octant2(v3Center + vector3(fCenters, -fCenters, -fCenters), fSize);

	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->level = level + 1;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->maxLevels = maxLevels;
		long newDimension = std::stol((std::to_string(dimension) + std::to_string(i)));
		m_pChild[i]->dimension = newDimension;
	}
}

void Octant2::CalcTree(void)
{
	typedef MyEntity* PEntity; //MyEntity Pointer
	PEntity* entity_Array = m_pEntityMngr->GetEntityArray();
	uint iEntityCount = m_pEntityMngr->GetEntityCount();
	std::vector<MyEntity*> collidingEntities;

	// find the entities that collide with the octant
	for (int i = 0; i < iEntityCount; i++)
	{
		MyRigidBody* pRB = entity_Array[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			// add the octant's dimension to those entities
			entity_Array[i]->AddDimension(dimension);
			collidingEntities.push_back(entity_Array[i]);
		}
	}

	// if subdivision is legal by given specification, subdivide and calculate new children octants
	if (collidingEntities.size() > 1 && level < maxLevels)
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

	// find the entities that collide with the octant
	for (int i = 0; i < colEntities.size(); i++)
	{
		MyRigidBody* pRB = colEntities[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			// add the octant's dimension to those entities
			colEntities[i]->RemoveDimension(m_pParent->dimension);
			colEntities[i]->AddDimension(dimension);
			collidingEntities.push_back(colEntities[i]);
		}
	}

	// if subdivision is legal by given specification, subdivide and calculate new children octants
	if (collidingEntities.size() > 1 && level < maxLevels)
	{
		Subdivide();
		for (int i = 0; i < 8; i++)
		{
			m_pChild[i]->CalcTree(collidingEntities);
		}
	}
}

void Simplex::Octant2::SetMaxLevels(int ml)
{
	if (ml > 0 && ml < 7)
	{
		maxLevels = ml;
	}
}

Octant2::Octant2(vector3 a_v3Center, float a_fSize)
{
	Init();
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(vector3(a_fSize) + a_v3Center);
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
}

//The big 3
Octant2::Octant2()
{
	Init();
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
	m_pRigidBody->MakeCubic();
}
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

