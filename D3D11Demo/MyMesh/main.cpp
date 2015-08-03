#include "MyMeshLoader.h"
void main()
{
	MyMeshLoader g_MeshLoader;
	g_MeshLoader.LoadMeshFile("robot.mesh.xml");
	SaveMeshFile(&g_MeshLoader.m_obj);
	system("pause");
}