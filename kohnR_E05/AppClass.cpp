#include "AppClass.h"
void Application::InitVariables(void)
{
	// shape of face
	const int HEIGHT = 8;
	const int WIDTH = 11;
	const float START_OFFSET_X = -10.0f;
	const float START_OFFSET_Y = 5.0f;
	int face[HEIGHT][WIDTH] = 
	{ 
	{ 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
	{ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
	{ 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
	{ 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0 }
	};

	// generate cubes in the shape of the face
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (face[i][j] == 1)
			{
				MyMesh* newCube = new MyMesh();
				newCube->GenerateCube(1.0f, C_BLACK);
				newCube->SetPosition(float(j) + START_OFFSET_X, (float(i) * -1) + START_OFFSET_Y, 0.0f);
				cubes.push_back(newCube);
			}
		}
	}

	//Make MyMesh object
	//m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	//m_pMesh1 = new MyMesh();
	//m_pMesh1->GenerateCube(1.0f, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	for (int i = 0; i < cubes.size(); i++)
	{
		// move the cubes
		vector3 currentPos = cubes[i]->GetPosition();
		cubes[i]->SetPosition(currentPos.x + 0.1f, currentPos.y, currentPos.z);

		// render the cubes
		cubes[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(cubes[i]->GetPosition()));
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// delete all the cubes
	for (int i = 0; i < cubes.size(); i++)
	{
		SafeDelete(cubes[i]);
	}

	//release GUI
	ShutdownGUI();
}