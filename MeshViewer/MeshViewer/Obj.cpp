#include "Obj.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using std::min;
using std::max;

Vector3 operator + (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //���������ĳ˲���
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //���������ĳ�����
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two) 
{//�������������Ĳ��
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
{//��ȡģ���ļ�

	FILE* fpFile = fopen(pcszFileName, "r"); //��ֻ����ʽ���ļ�
	if (fpFile == NULL)
	{
		return false;
	}

	m_pts.clear(); 
	m_faces.clear();

	//TODO����ģ���ļ��еĵ�������ݷֱ����m_pts��m_faces��
	const int MLEN = 300;//�ַ�����󳤶�
	char str[MLEN];//�ַ�������
	Point pt;
	Face f;
	while (fgets(str, MLEN, fpFile)) {
		std::stringstream ss(str);
		char temp[10];

		ss >> temp;
		if (temp[0] == 'v') {
			ss >> pt.pos.fX >> pt.pos.fY >> pt.pos.fZ;
			m_pts.push_back(pt);//������ӵ�m_pts��
		}
		else if (temp[0] == 'f') {
			ss >> f.pts[0] >> f.pts[1] >> f.pts[2];
			f.pts[0]--; f.pts[1]--; f.pts[2]--;
			m_faces.push_back(f);//������ӵ�m_faces��
		}
	}
	fclose(fpFile);
	UnifyModel(); //��ģ�͹�һ��
	for (int i = 0; i < m_faces.size(); i++) {
		ComputeFaceNormal(m_faces[i]);
	}
	return true;
}

void CObj::UnifyModel()
{//Ϊͳһ��ʾ��ͬ�ߴ��ģ�ͣ���ģ�͹�һ������ģ�ͳߴ����ŵ�0.0-1.0֮��
//ԭ���ҳ�ģ�͵ı߽�������Сֵ�������ҳ�ģ�͵�����
//��ģ�͵����ĵ�Ϊ��׼��ģ�Ͷ����������
//TODO:���ģ�͹�һ������
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
{//TODO:������f�ķ�������������
	Vector3 cb;
	Vector3 ab;

	cb = m_pts[f.pts[1]].pos - m_pts[f.pts[0]].pos;
	ab = m_pts[f.pts[2]].pos - m_pts[f.pts[0]].pos;
	cb = Cross(cb, ab);
	f.normal = cb;
	f.normal.Normalize();

}