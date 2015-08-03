#include "MyMeshLoader.h"


MyMeshLoader::MyMeshLoader()
{

}


MyMeshLoader::~MyMeshLoader()
{

}

bool MyMeshLoader::LoadMeshFile(const STRING& filename)
{
	TiXmlDocument doc;
	if (!doc.LoadFile(filename.c_str()))
	{
		throw std::logic_error("Error, Can't load .mesh file! Please make sure it's in a xml format!");
		return false;
	}

	m_obj.VB.clear();
	m_obj.IB.clear();

	TiXmlElement* submeshNode = doc.FirstChildElement("mesh")->FirstChildElement("submeshes")->FirstChildElement("submesh");

	//读取面信息
	{
		TiXmlElement* facesNode = submeshNode->FirstChildElement("faces");
		int nFace = 0;
		facesNode->Attribute("count", &nFace);

		m_obj.IB.resize(nFace * 3);

		int curPos = 0;
		TiXmlElement* faceNode = facesNode->FirstChildElement("face");
		while (faceNode)
		{
			int v1, v2, v3;
			faceNode->Attribute("v1", &v1);
			faceNode->Attribute("v2", &v2);
			faceNode->Attribute("v3", &v3);

			m_obj.IB[curPos++] = v1;
			m_obj.IB[curPos++] = v2;
			m_obj.IB[curPos++] = v3;

			faceNode = faceNode->NextSiblingElement("face");
		}
	}

	//读取顶点数据
		{
			TiXmlElement* geometryNode = submeshNode->FirstChildElement("geometry");
			int nVert = 0;
			geometryNode->Attribute("vertexcount", &nVert);

			m_obj.VB.resize(nVert);

			TiXmlElement* vbNode = geometryNode->FirstChildElement("vertexbuffer");
			if (vbNode->Attribute("positions") != STRING("true"))
			{
				throw std::logic_error("Error, the .mesh file doesn't even have vertex position info!");
				return false;
			}

			int curPos = 0;
			TiXmlElement* vertNode = vbNode->FirstChildElement("vertex");
			while (vertNode)
			{
				TiXmlElement* posNode = vertNode->FirstChildElement("position");

				double x, y, z;
				posNode->Attribute("x", &x);
				posNode->Attribute("y", &y);
				posNode->Attribute("z", &z);

				SR::SVertex vert;
				vert.pos = VEC4(x, y, z, 1);
				m_obj.VB[curPos++] = std::move(vert);

				vertNode = vertNode->NextSiblingElement("vertex");
			}
		}
		return true;
}

bool SaveMeshFile(const SR::SRenderObj* m_obj)
{
	//创建一个XML的文档对象。
	TiXmlDocument *myDocument = new TiXmlDocument();
	TiXmlElement *RootElement = new TiXmlElement("mesh");
	myDocument->LinkEndChild(RootElement);
	//创建一个Person元素并连接。
	TiXmlElement *submeshesElement = new TiXmlElement("submeshes");
	RootElement->LinkEndChild(submeshesElement);

	//创建一个submesh元素并连接。
	TiXmlElement *submeshElement = new TiXmlElement("submesh");
	submeshElement->SetAttribute("material", "Examples/Robot");
	submeshElement->SetAttribute("usesharedvertices", "false");
	submeshElement->SetAttribute("use32bitindexes", "false");
	submeshElement->SetAttribute("operationtype", "triangle_list");
	submeshesElement->LinkEndChild(submeshElement);

	TiXmlElement *faceElements = new TiXmlElement("faces");
	faceElements->SetAttribute("count", m_obj->IB.size()/3);
	submeshElement->LinkEndChild(faceElements);
	//写入面信息
	int curPos = 0;
	for (int i = 0; i < m_obj->IB.size() / 3;++i)
	{
		TiXmlElement* faceNode = new TiXmlElement("face");
		int v1, v2, v3;
		v1 = m_obj->IB[curPos++];
		v2 = m_obj->IB[curPos++];
		v3 = m_obj->IB[curPos++];
		faceNode->SetAttribute("v1", ( v1));
		faceNode->SetAttribute("v2", v2);
		faceNode->SetAttribute("v3", v3);
		faceElements->LinkEndChild(faceNode);
	}

	TiXmlElement *geometryElements = new TiXmlElement("geometry");
	geometryElements->SetAttribute("vertexcount", m_obj->VB.size());
	submeshElement->LinkEndChild(geometryElements);
	TiXmlElement* vertexbufferNode = new TiXmlElement("vertexbuffer");
	vertexbufferNode->SetAttribute("positions", "true");
	vertexbufferNode->SetAttribute("normals", "true");
	geometryElements->LinkEndChild(vertexbufferNode);
	for (int i = 0; i <m_obj->VB.size(); ++i)
	{
		TiXmlElement* vertexNode = new TiXmlElement("vertex");
		vertexbufferNode->LinkEndChild(vertexNode);
		TiXmlElement*positionNode = new TiXmlElement("position");
		TiXmlElement* normalNode = new TiXmlElement("normal");
		float v1, v2, v3;
		v1 = m_obj->VB[i].pos.x;
		v2 = m_obj->VB[i].pos.y;
		v3 = m_obj->VB[i].pos.z;
		char strText[128];
		sprintf_s(strText, sizeof(strText), "%f", v1);
		positionNode->SetAttribute("x", strText);
		normalNode->SetAttribute("x", strText);

		sprintf_s(strText, sizeof(strText), "%f", v2);
		positionNode->SetAttribute("y", strText);
		normalNode->SetAttribute("y", strText);

		sprintf_s(strText, sizeof(strText), "%f", v3);
		positionNode->SetAttribute("z", strText);
		normalNode->SetAttribute("y", strText);

		vertexNode->LinkEndChild(positionNode);
		vertexNode->LinkEndChild(normalNode);
	}
	myDocument->SaveFile("myMesh.mesh.xml");
	return true;
}
