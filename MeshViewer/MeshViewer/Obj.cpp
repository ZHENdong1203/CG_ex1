#include "Obj.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using std::min;
using std::max;

Vector3 operator + (const Vector3& one, const Vector3& two) //两个向量相加
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //两个向量相减
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //向量与数的乘操作
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //向量与数的除操作
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two) 
{//计算两个向量的叉积
	Vector3 vCross;								

	vCross.fX = ((one.fY * two.fZ) - (one.fZ * two.fY));
	vCross.fY = ((one.fZ * two.fX) - (one.fX * two.fZ));
	vCross.fZ = ((one.fX * two.fY) - (one.fY * two.fX));

	return vCross;		
}

CObj::CObj(void)
{
}


CObj::~CObj(void)
{
}

bool CObj::ReadObjFile(const char* pcszFileName) 
{//读取模型文件

	FILE* fpFile = fopen(pcszFileName, "r"); //以只读方式打开文件
	if (fpFile == NULL)
	{
		return false;
	}

	m_pts.clear(); 
	m_faces.clear();

	//TODO：将模型文件中的点和面数据分别存入m_pts和m_faces中
	const int MLEN = 300;//字符串最大长度
	char str[MLEN];//字符型数组
	Point pt;
	Face f;
	while (fgets(str, MLEN, fpFile)) {
		std::stringstream ss(str);
		char temp[10];

		ss >> temp;
		if (temp[0] == 'v') {
			ss >> pt.pos.fX >> pt.pos.fY >> pt.pos.fZ;
			m_pts.push_back(pt);//将点添加到m_pts中
		}
		else if (temp[0] == 'f') {
			ss >> f.pts[0] >> f.pts[1] >> f.pts[2];
			f.pts[0]--; f.pts[1]--; f.pts[2]--;
			m_faces.push_back(f);//将面添加到m_faces中
		}
	}
	fclose(fpFile);
	UnifyModel(); //将模型归一化
	for (int i = 0; i < m_faces.size(); i++) {
		ComputeFaceNormal(m_faces[i]);
	}
	return true;
}

void CObj::UnifyModel()
{//为统一显示不同尺寸的模型，将模型归一化，将模型尺寸缩放到0.0-1.0之间
//原理：找出模型的边界最大和最小值，进而找出模型的中心
//以模型的中心点为基准对模型顶点进行缩放
//TODO:添加模型归一化代码
	Vector3 ma, mi;
	ma = mi = m_pts[0].pos;
	for (int i = 1; i < m_pts.size(); i++) {
		mi.fX = min(mi.fX, m_pts[i].pos.fX);
		mi.fY = min(mi.fY, m_pts[i].pos.fY);
		mi.fZ = min(mi.fZ, m_pts[i].pos.fZ);
		ma.fX = max(ma.fX, m_pts[i].pos.fX);
		ma.fY = max(ma.fY, m_pts[i].pos.fY);
		ma.fZ = max(ma.fZ, m_pts[i].pos.fZ);
	}
	Vector3 cen = (ma + mi)*0.5;
	double fScale = (ma - mi).Length();

	fScale = 10.0 / fScale;
	for (int i = 0; i < m_pts.size(); i++) {
		Vector3& p = m_pts[i].pos;
		p = (p - cen)*fScale;
	}
}

void CObj::ComputeFaceNormal(Face& f)
{//TODO:计算面f的法向量，并保存
	Vector3 cb;
	Vector3 ab;

	cb = m_pts[f.pts[1]].pos - m_pts[f.pts[0]].pos;
	ab = m_pts[f.pts[2]].pos - m_pts[f.pts[0]].pos;
	cb = Cross(cb, ab);
	f.normal = cb;
	f.normal.Normalize();

}