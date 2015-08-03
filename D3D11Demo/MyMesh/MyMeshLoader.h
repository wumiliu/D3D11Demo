#pragma once
#include "base.h"
#include "GeometryDef.h"
bool	SaveMeshFile(const SR::SRenderObj*	m_obj);
class MyMeshLoader
{
public:
	MyMeshLoader();
	~MyMeshLoader();
public:
	bool	LoadMeshFile(const STRING& filename);
	SR::SRenderObj	m_obj;
};

