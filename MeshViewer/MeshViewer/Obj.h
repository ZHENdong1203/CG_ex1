#ifndef OBJ_CLASS
#define OBJ_CLASS

#include <vector>
#include <cmath>

struct Vector3;
Vector3 operator + (const Vector3& one, const Vector3& two);
Vector3 operator - (const Vector3& one, const Vector3& two);
Vector3 operator * (const Vector3& one, double scale);
Vector3 operator / (const Vector3& one, double scale);
Vector3 Cross(Vector3& one, Vector3& two);

struct Vector3
{
	double fX;
	double fY;
	double fZ;
	Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : fX(x), fY(y), fZ(z) {}
	Vector3 operator +=(const Vector3& v) { return *this = *this + v; }
	double Length() { return sqrt(fX * fX + fY * fY + fZ * fZ); }
	void Normalize()//��һ��
	{
		double fLen = Length();
		if (fabs(fLen) > 1e-6)
		{
			fX /= fLen;
			fY /= fLen;
			fZ /= fLen;
		}
	}
};

struct Point
{
	Vector3 pos;
	Vector3 normal;
};

struct Face
{
	int pts[3];
	Vector3 normal;
};

class CObj
{
public:
	CObj(void);
	~CObj(void);

	std::vector<Point> m_pts; //����
	std::vector<Face> m_faces;//��

public:
	bool ReadObjFile(const char* pcszFileName);//����ģ���ļ�

private:
	void UnifyModel();//��λ��ģ��
	void ComputeFaceNormal(Face& f);//������ķ���
};

#endif

