/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __OCTANT2_H_
#define __OCTANT2_H_

#include "Simplex\Simplex.h"
#include "MyEntityManager.h"

namespace Simplex
{
	//System Class
	class Octant2
	{
		int m_nData = 0; //Number of elements in the list of elements
		long dimension; // what dimension the octant assigns to entities in it
		int level; // the level in the octree the octant is
		int maxLevels = 1; // the maximum number of times the octree can be subdivided

		std::vector<int> m_lData; //list of elements
		MeshManager* m_pMeshMngr = nullptr; //Pointer to Mesh manager
		MyEntityManager* m_pEntityMngr = nullptr;
		MyRigidBody* m_pRigidBody = nullptr;
		Octant2* m_pParent = nullptr;
		Octant2* m_pChild[8];
	public:
		/*
		Usage: Displays the octants
		Arguments: ---
		Output: ---
		*/
		void Display(void);
		/*
		Usage: Creates the 8 children octants, subdividing the original space
		Arguments: ---
		Output: ---
		*/
		void Subdivide(void);
		/*
		Usage: Recursively assigns dimensions to entities, starting from root
		Arguments: ---
		Output: ---
		*/
		void CalcTree(void);
		/*
		Usage: Recursively assigns dimensions to entities 
		Arguments: List of possible colliding entities from parent octant
		Output: ---
		*/
		void CalcTree(std::vector<MyEntity*> colEntities);
		/*
		Usage: Sets maxLevels attribute
		Arguments: New max level
		Output: ---
		*/
		void SetMaxLevels(int ml);
		/*
		Usage: Constructor for creating suboctants
		Arguments: center and size of suboctant
		Output: class object instance
		*/
		Octant2(vector3 center, float size);
		/*
		Usage: Constructor
		Arguments: ---
		Output: class object instance
		*/
		Octant2(void);
		/*
		Usage: Copy Constructor
		Arguments: class object to copy
		Output: class object instance
		*/
		Octant2(Octant2 const& other);
		/*
		Usage: Copy Assignment Operator
		Arguments: class object to copy
		Output: ---
		*/
		Octant2& operator=(Octant2 const& other);
		/*
		Usage: Destructor
		Arguments: ---
		Output: ---
		*/
		~Octant2(void);

		/*
		Usage: Changes object contents for other object's
		Arguments: other -> object to swap content from
		Output: ---
		*/
		void Swap(Octant2& other);

		/*
		Usage: Gets data member
		Arguments: ---
		Output: data
		*/
		int GetData(void);
		/*
		Usage: Sets data member
		Arguments: int a_nData = 1 -> data to set
		Output: ---
		*/
		void SetData(int a_nData = 1);
		/* Property */
		//__declspec(property(get = GetData, put = SetData)) int Data;

		/*
		Usage: adds data on vector
		Arguments: int a_nData -> data to add
		Output: ---
		*/
		void SetDataOnVector(int a_nData);
		/*
		Usage: gets data on vector at the specified entry
		Arguments: int a_nIndex -> entry index
		Output: entry's content
		*/
		int& GetDataOnVector(int a_nIndex);

	private:
		/*
		Usage: Deallocates member fields
		Arguments: ---
		Output: ---
		*/
		void Release(void);
		/*
		Usage: Allocates member fields
		Arguments: ---
		Output: ---
		*/
		void Init(void);
	};//class

} //namespace Simplex

#endif //__OCTANT_H_

  /*
  USAGE:
  ARGUMENTS: ---
  OUTPUT: ---
  */
