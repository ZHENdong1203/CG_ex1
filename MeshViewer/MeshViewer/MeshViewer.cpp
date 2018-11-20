#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <gl\glui.h>
#include <math.h>
#include "Obj.h"
#include "common.h"

//�������

/*	Create checkerboard texture	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

#ifdef GL_VERSION_1_1
static GLuint texName;
#endif

void makeCheckImage(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 100;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
			checkImage[i][j][3] = (GLubyte) 100;
		} 
	}
}

//�������

int g_xform_mode = TRANSFORM_NONE;
int   g_main_window;
double g_windows_width, g_windows_height;

CObj g_obj;
//the lighting
static GLfloat g_light0_ambient[] =  {0.0f, 0.0f, 0.0f, 1.0f};//������
static GLfloat g_light0_diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};//ɢ���
static GLfloat g_light0_specular[] = {1.0f,1.0f,1.0f,1.0f}; //�����
static GLfloat g_light0_position[] = {0.0f, 0.0f, 100.0f, 0.0f};//��Դ��λ�á���4������Ϊ1����ʾ���Դ����4��������Ϊ0����ʾƽ�й���{0.0f, 0.0f, 10.0f, 0.0f}

static GLfloat g_material[] = {0.96f, 0.8f, 0.69f, 1.0f};//����
static GLfloat g_rquad = 0;

//ADD
float rotationX = 0.0f, rotationY = 0.0f;
float translateX = 0.0f, translateY = 0.0f;

static float g_x_offset   = 0.0;
static float g_y_offset   = 0.0;
static float g_z_offset   = 0.0;
static float g_scale_size = 1; 
static int  g_press_x; //��갴��ʱ��x����
static int  g_press_y; //��갴��ʱ��y����

const int n = 1000;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
int g_view_type = VIEW_FLAT;
int g_draw_content = SHAPE_TRIANGLE;


void DrawTriangle() 
{//����������
	glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
		glVertex3f( 0.0f, 1.0f, 0.0f);                    // �϶���
		glVertex3f(-1.0f,-1.0f, 0.0f);                    // ����
		glVertex3f( 1.0f,-1.0f, 0.0f);                    // ����
	glEnd();
}

void DrawCube() 
{//����������
	glBegin(GL_QUADS);  
		glNormal3f( 0.0f, 0.0f, 1.0f);  //ָ���淨��
		glVertex3f( 1.0f, 1.0f,1.0f);   //�о��涥�����ݣ���ʱ��˳��
		glVertex3f(-1.0f, 1.0f, 1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
	//ǰ----------------------------  
		glNormal3f( 0.0f, 0.0f,-1.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
	//��----------------------------  
		glNormal3f( 0.0f, 1.0f, 0.0f);  
		glVertex3f( 1.0f, 1.0f, 1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
		glVertex3f(-1.0f, 1.0f, 1.0f);  
	//��----------------------------  
		glNormal3f( 0.0f,-1.0f, 0.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
	//��----------------------------  
	glNormal3f( 1.0f, 0.0f, 0.0f);  
		glVertex3f( 1.0f, 1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f, 1.0f);  
		glVertex3f( 1.0f,-1.0f,-1.0f);  
		glVertex3f( 1.0f, 1.0f,-1.0f);  
	//��----------------------------  
	glNormal3f(-1.0f, 0.0f, 0.0f);  
		glVertex3f(-1.0f,-1.0f,-1.0f);  
		glVertex3f(-1.0f,-1.0f, 1.0f);  
		glVertex3f(-1.0f, 1.0f, 1.0f);  
		glVertex3f(-1.0f, 1.0f,-1.0f);  
	//��----------------------------*/  
	glEnd();  
}

void DrawCircle() 
{//����Բ
	glBegin(GL_POLYGON);
		glNormal3f(0.0f, 0.0f, 1.0f);
		for(int i=0; i<n; ++i)
			glVertex2f(R*cos(2*Pi/n*i), R*sin(2*Pi/n*i));
	glEnd();
}



void DrawCylinder() {
	float h = 1.0f;
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < n; i++) {//�������Բ
		glTexCoord2f(cos(2 * Pi*i / n) + 0.5, sin(2 * Pi*i / n) + 0.5);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), h);
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0, 0, h);
		glTexCoord2f(cos(2 * Pi*(i + 1) / n) + 0.5, sin(2 * Pi*(i + 1) / n) + 0.5);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), h);
	}
	glEnd();

	glBegin(GL_TRIANGLES);//�������Բ
	glNormal3f(0.0f, 0.0f, -1.0f);
	for (int i = 0; i < n; i++) {
		glTexCoord2f(0, 1);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), -h);
		glTexCoord2f(0, 0);
		glVertex3f(0, 0, -h);
		glTexCoord2f(0, 1);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), -h);
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int i = 0; i < n; i++) {
		glNormal3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), 0);
		glTexCoord2f((float)i / n, 0);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), -h);
		glTexCoord2f((float)i / n, 1);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), h);
		glTexCoord2f((float)(i + 1) / n, 1);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), h);
		glTexCoord2f((float)(i + 1) / n, 0);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), -h);
	}
	glEnd();
}

void DrawCone() {
	float h = 1.0f;
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, -1.0f);
	for (int i = 0; i < n; i++) {
		glTexCoord2f(cos(2 * Pi*i / n) + 0.5, sin(2 * Pi*i / n) + 0.5);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), 0);
		glTexCoord2f(cos(2 * Pi*(i + 1) / n) + 0.5, sin(2 * Pi*(i + 1) / n) + 0.5);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), 0);
		glTexCoord2f(0.5, 0.5);
		glVertex3f(0, 0, 0);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < n; i++) {
		Vector3 v1(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), 0);
		Vector3 v2(0, 0, h);
		Vector3 v3(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), 0);
		Vector3 norm = Cross((v2 - v3), (v1 - v2));
		glNormal3f(norm.fX, norm.fY, norm.fZ);

		//glTexCoord2f(0.5-cos(2*Pi*i/n),sin(2*Pi*i/n));
		glTexCoord2f(0, 1);
		glVertex3f(R*cos((2 * Pi)*i / n), R*sin((2 * Pi)*i / n), 0);
		//glTexCoord2f(0.5 - cos(2 * Pi*(i+1) / n), sin(2 * Pi*(i+1) / n));
		glTexCoord2f(0, 1);
		glVertex3f(R*cos((2 * Pi)*(i + 1) / n), R*sin((2 * Pi)*(i + 1) / n), 0);
		glTexCoord2f(0, 0);
		glVertex3f(0, 0, h);
	}
	glEnd();
}

void DrawModel(CObj &model)
{//TODO: ����ģ��
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < model.m_faces.size(); i++) {
		glNormal3d(model.m_faces[i].normal.fX, model.m_faces[i].normal.fY, model.m_faces[i].normal.fZ);
		for (int j = 0; j < 3; j++) {
			glVertex3d(model.m_pts[model.m_faces[i].pts[j]].pos.fX, model.m_pts[model.m_faces[i].pts[j]].pos.fY, model.m_pts[model.m_faces[i].pts[j]].pos.fZ);
		}
	}
	glEnd();

}

void myInit()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//�ð�ɫ����

	glLightfv(GL_LIGHT0, GL_AMBIENT, g_light0_ambient);//���ó����Ļ�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light0_diffuse);//���ó�����ɢ���
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_position);//���ó�����λ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, g_material);//ָ�����ڹ��ռ���ĵ�ǰ��������
	
	glEnable(GL_LIGHTING);//�����ƹ�
	glEnable(GL_LIGHT0);//��������0
	glShadeModel(GL_SMOOTH); //������ɫģʽΪ�⻬��ɫ
	glEnable(GL_DEPTH_TEST);//������Ȳ���

	glMatrixMode(GL_MODELVIEW); //ָ����ǰ����Ϊģ���Ӿ�����
	glLoadIdentity(); //����ǰ���û�����ϵ��ԭ���Ƶ�����Ļ���ģ�������һ����λ����
	gluLookAt(0.0, 0.0, 8.0, 0, 0, 0, 0, 1.0, 0);//�ú�������һ����ͼ���󣬲��뵱ǰ�������.
	//��һ��eyex, eyey,eyez ��������������λ��;�ڶ���centerx,centery,centerz �����ͷ��׼�����������������λ��;������upx,upy,upz ������ϵķ��������������еķ���

	//ADD TEXTURE
	makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//��ʹ�ö���

#ifdef GL_VERSION_1_1
	glGenTextures(1, &texName);//�����������ָ����������
	glBindTexture(GL_TEXTURE_2D, texName);//������
#endif
	//ȷ���������Ӧ�õ�ÿ�������ϣ�������˺���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S������ͼ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//T������ͼ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//���Թ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//���Բ�ֵ
#ifdef GL_VERSION_1_1
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);//Ϊ�������ָ������
#else
	glTexImage2D(GL_TEXTURE_2D, 0, 4, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
#endif
}

void loadObjFile(void)
{//����ģ��

	//����ϵͳ�Ի���
	OPENFILENAME  fname; 
	ZeroMemory(&fname, sizeof(fname));
	char strfile[200] = "*.obj";             
	char szFilter[]=TEXT("OBJ Files(*.OBJ)\0");
	fname.lStructSize = sizeof(OPENFILENAME); 
	fname.hwndOwner = NULL; 
	fname.hInstance = NULL; 
	fname.lpstrFilter = szFilter; 
	fname.lpstrCustomFilter = NULL; 
	fname.nFilterIndex = 0; 
	fname.nMaxCustFilter = 0; 
	fname.lpstrFile = strfile; 
	fname.nMaxFile = 200; 
	fname.lpstrFileTitle = NULL; 
	fname.nMaxFileTitle = 0;
	fname.lpstrTitle = NULL; 
	fname.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT; 
	fname.nFileOffset = 0; 
	fname.nFileExtension = 0; 
	fname.lpstrDefExt = 0; 
	fname.lCustData  = NULL; 
	fname.lpfnHook = NULL; 
	fname.lpTemplateName  = NULL;
	fname.lpstrInitialDir = NULL; 
	HDC hDC = wglGetCurrentDC(); 
	HGLRC hRC = wglGetCurrentContext(); 
	GetOpenFileName(&fname); 
	wglMakeCurrent(hDC, hRC); 

	g_obj.ReadObjFile(fname.lpstrFile); //����ģ���ļ�
}

void myGlutDisplay() //��ͼ������ ����ϵͳ�ڱ�Ҫʱ�̾ͻ�Դ���������»��Ʋ���
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //�����ɫ�����Լ���Ȼ���
	glEnable(GL_NORMALIZE); //�򿪷���������һ����ȷ���˷��ߵĳ���Ϊ1
	
	glMatrixMode(GL_MODELVIEW);//ģ����ͼ����
	glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -6.0f);  
	glTranslatef(translateX, translateY, -6.0f);
	glRotatef(rotationX, 1.0, 0.0, 0.0);//�ط���������1,0,0����תrotationX��
	glRotatef(rotationY, 0.0, 1.0, 0.0);////�ط���������0,1,0����תrotationY��
	glScalef(g_scale_size, g_scale_size, g_scale_size);//��x,y,z������
	glPushMatrix(); //ѹ�뵱ǰ�����ջ


	//ADD TEXTURE
	//glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);//������ͼ����ʻ��
#ifdef GL_VERSION_1_1
	glBindTexture(GL_TEXTURE_2D, texName);
#endif
	//END TEXTURE


	if (g_draw_content == SHAPE_MODEL)
	{//����ģ��
		DrawModel(g_obj);
	}
	else if (g_draw_content == SHAPE_TRIANGLE)  //��������
	{
		//glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -6.0f);  
		DrawTriangle();
	}
	else if(g_draw_content == SHAPE_CUBE)  //��������
	{
		//glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -6.0f);  
		//glRotatef(g_rquad, g_rquad, g_rquad, 1.0f);	// ��XYZ������ת������
		DrawCube();
		//g_rquad+=0.2f;// ������ת����
	}
	else if (g_draw_content == SHAPE_CIRCLE) // ��Բ
	{
		//glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -6.0f);  
		DrawCircle();
	}
	else if (g_draw_content == SHAPE_CYLINDER)  
	{//TODO: ��ӻ�Բ���Ĵ���
		DrawCylinder();

	}
	else if (g_draw_content == SHAPE_CONE) 
	{//TODO����ӻ�Բ׶�Ĵ���
		DrawCone();

	}
	glPopMatrix();
	glutSwapBuffers(); //˫����
}

void myGlutReshape(int x,int y) //���ı䴰�ڴ�Сʱ�Ļص�����
{
	if (y == 0)
	{
		y = 1;
	}
	
	g_windows_width = x;
	g_windows_height = y;
	double xy_aspect = (float)x / (float)y;
	GLUI_Master.auto_set_viewport(); //�Զ������ӿڴ�С

	glMatrixMode( GL_PROJECTION );//��ǰ����ΪͶӰ����
	glLoadIdentity();
	gluPerspective(60.0, xy_aspect, 0.01, 1000.0);//�Ӿ���

	glutPostRedisplay(); //��ǵ�ǰ������Ҫ���»���
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{//����ʱ��ص�����

}

void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) //����״̬Ϊ����
	{
		g_press_x = x;
		g_press_y = y; 
		if (button == GLUT_LEFT_BUTTON) 
		{//�������������ʾ��ģ�ͽ�����ת����
			g_xform_mode = TRANSFORM_ROTATE;
		}
		else if (button == GLUT_MIDDLE_BUTTON)  
		{//���������Ҽ���ʾ��ģ�ͽ���ƽ�Ʋ���
			g_xform_mode = TRANSFORM_TRANSLATE; 
		}
		else if (button ==  GLUT_RIGHT_BUTTON) 
		{//�������Ļ��ֱ�ʾ���������Ҽ���ʾ��ģ�ͽ������Ų���
			g_xform_mode = TRANSFORM_SCALE; 
		}
	}
	else if (state == GLUT_UP)  
	{//���û�а���꣬�򲻶�ģ�ͽ����κβ���
		g_xform_mode = TRANSFORM_NONE; 
	}
}

void myGlutMotion(int x, int y) //������������ʱ,����϶����¼�,�������Ϊ���Ĵ�������
{
	if (g_xform_mode == TRANSFORM_ROTATE) //��ת
	{//TODO:�������ƶ�����ģ����ת�����Ĵ���

		rotationX += (float)(y - g_press_y);
		rotationY += (float)(x - g_press_x);
		g_press_x = x;
		g_press_y = y;

	}
	else if(g_xform_mode == TRANSFORM_SCALE) //����
	{//TODO:�������ƶ�����ģ�����Ų����Ĵ���

		g_scale_size *= (1 + (y - g_press_y) / 1000.0);
		g_scale_size *= (1 + (x - g_press_x) / 1000.0);
		if (g_scale_size < 0.0001) {
			g_scale_size = 0.0001;
		}
		g_press_y = y;
		g_press_x = x;

	}
	else if(g_xform_mode == TRANSFORM_TRANSLATE) //ƽ��
	{//TODO:�������ƶ�����ģ��ƽ�Ʋ����Ĵ���

		translateX += (x - g_press_x) / 100.0;
		translateY -= (y - g_press_y) / 100.0;
		g_press_x = x;
		g_press_y = y;

	}

	// force the redraw function
	glutPostRedisplay(); 
}

void myGlutIdle(void) //���лص�����
{
	if ( glutGetWindow() != g_main_window ) 
		glutSetWindow(g_main_window);  

	glutPostRedisplay();
}

void glui_control(int control ) //����ؼ��ķ���ֵ
{
	switch(control)
	{
	case CRTL_LOAD://ѡ��open���ؼ�
		loadObjFile();
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_CHANGE://ѡ��Type���
		if (g_view_type == VIEW_POINT)  
		{
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // ���������Ϊ������Ʒ�ʽ
		}
		else if (g_view_type == VIEW_WIRE)
		{
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // ���������Ϊ�߶λ��Ʒ�ʽ
		}
		else if (g_view_type == VIEW_FLAT)
		{
			glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ��������Ϊ��䷽ʽ
		}
		else
		{
			glEnable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ��������Ϊ��䷽ʽ
		}	
		break;
	case CRTL_TRIANGLE:
		g_draw_content = SHAPE_TRIANGLE;
        break;
	case CRTL_CUBE:
		g_draw_content = SHAPE_CUBE;
		break;
	case CRTL_CIRCLE:
		g_draw_content = SHAPE_CIRCLE;
		break;
	case CRTL_CYLINDER:
		g_draw_content = SHAPE_CYLINDER;
		break;
	case CRTL_CONE:
		g_draw_content = SHAPE_CONE;
		break;
	case CRTL_MODEL:
		g_draw_content = SHAPE_MODEL;
		break;
	default:
		break;
	}
}

void myGlui()
{
	GLUI_Master.set_glutDisplayFunc( myGlutDisplay ); //ע����Ⱦ�¼��ص������� ϵͳ����Ҫ�Դ���������»��Ʋ���ʱ����
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  //ע�ᴰ�ڴ�С�ı��¼��ص�����
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );//ע����������¼��ص�����
	glutMotionFunc( myGlutMotion);//ע������ƶ��¼��ص�����
	GLUI_Master.set_glutMouseFunc( myGlutMouse );//ע��������¼��ص�����
	GLUI_Master.set_glutIdleFunc(myGlutIdle); //ΪGLUIע��һ����׼��GLUT���лص���������ϵͳ���ڿ���ʱ,�ͻ���ø�ע��ĺ���

	//GLUI
	GLUI *glui = GLUI_Master.create_glui_subwindow( g_main_window, GLUI_SUBWINDOW_RIGHT); //�½��Ӵ��壬λ����������Ҳ� 
	new GLUI_StaticText(glui, "GLUI" ); //��GLUI���½�һ����̬�ı����������Ϊ��GLUI��
	new GLUI_Separator(glui); //�½��ָ���
	new GLUI_Button(glui,"Open", CRTL_LOAD, glui_control); //�½���ť�ؼ��������ֱ�Ϊ���������塢���֡�ID���ص�����������ť������ʱ,���ᱻ����.
	new GLUI_Button(glui, "Quit", 0,(GLUI_Update_CB)exit );//�½��˳���ť������ť������ʱ,�˳�����

	GLUI_Panel *type_panel = glui->add_panel("Type" ); //���Ӵ���glui���½���壬����Ϊ��Type��
	GLUI_RadioGroup *radio = glui->add_radiogroup_to_panel(type_panel, &g_view_type, CRTL_CHANGE, glui_control); //��Type��������һ�鵥ѡ��ť
	glui->add_radiobutton_to_group(radio, "points"); 
	glui->add_radiobutton_to_group(radio, "wire");
	glui->add_radiobutton_to_group(radio, "flat");
	glui->add_radiobutton_to_group(radio, "texture");

	GLUI_Panel *draw_panel = glui->add_panel("Draw" ); //���Ӵ���glui���½���壬����Ϊ��Draw��
	new GLUI_Button(draw_panel,"Triangle",CRTL_TRIANGLE,glui_control);
	new GLUI_Button(draw_panel,"Cube",CRTL_CUBE,glui_control);
	new GLUI_Button(draw_panel,"Circle",CRTL_CIRCLE,glui_control);
	new GLUI_Button(draw_panel,"Cylinder",CRTL_CYLINDER,glui_control);
	new GLUI_Button(draw_panel,"Cone",CRTL_CONE,glui_control);
	new GLUI_Button(draw_panel,"Model",CRTL_MODEL,glui_control);

	glui->set_main_gfx_window(g_main_window ); //���Ӵ���glui��������main_window�󶨣�������glui�еĿؼ���ֵ�������ı䣬���glui���ڱ��ػ�
	GLUI_Master.set_glutIdleFunc( myGlutIdle ); 
}

int main(int argc, char* argv[]) //�������
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/

  freopen("log.txt", "w", stdout);//�ض�λ�����������log.txt�ļ���
  glutInit(&argc, argv);//��ʼ��glut
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );//��ʼ����Ⱦģʽ
  glutInitWindowPosition(200, 200 ); //��ʼ������λ��
  glutInitWindowSize(800, 600 ); //��ʼ�����ڴ�С
 
  g_main_window = glutCreateWindow("Model Viewer" ); //����������Model Viewer

  myGlui();
  myInit();

  glutMainLoop();//����glut��Ϣѭ��

  return EXIT_SUCCESS;
}

