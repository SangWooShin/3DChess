#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

using namespace std;

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info);

void LoadTexture(const char* filename, int count);
GLubyte *pBytes; // �����͸� ����ų ������
BITMAPINFO *info; // ��Ʈ�� ��� ������ ����

GLuint textures[6];

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid TimerFunction(int value);
GLvoid DrawPos();
GLvoid InitObject();
GLvoid DrawRec();
GLvoid Motion(int x, int y);

void Mouse(int button, int state, int x, int y);

int iAngleX = 0;                     // ī�޶� ȸ��
int iAngleY = 0;
int iAngleZ = 0;
int MoveX = 0;                        // Ű���� �Է¹޴� �ٴ� Move��
int MoveZ = 300;
int InputX = 0;               // �����϶� �������� ��(������)
int InputZ = 0;
int CollideNum = 0;					// �浹�� ���� ��ȣ
bool CollideCount = 0;				// �浹 �Ǿ�����
int ChessMap[8][8] = { 0 };		// ���� �ִ��� ������.
int MoveObj = 0;                     // �����϶� ����ϴ� ����
int TurnY = 0;                        // �� ��ȯ �ִϸ��̼�(ȸ��)
int TimerFunCount = 0;                  // TimerFuncȣ���Ҷ����� ī��Ʈ ����
int SelectObj = 0;                     // ������ ���� ��ȣ
double dTransZ = 0;                     // ī�޶� �̵�
double dTransX = 0;
double dTransY = 0;
bool Check = 0;							// üũ �� 1
bool CheckMate = 0;						// üũ����Ʈ �� 1
bool TurnState = 0;                     // �� ����(����, �����)
bool Turn = 0;                        // �� ON/OFF
bool MoveAni = 0;                     // ������ �ִϸ��̼� ON/OFF
int MoveDraw = 0;                     // ������ ���� (0 : �ƹ��͵� ����, 1 : �̵��ϴ� �׸�, 2 : �̵������Ѱ� ǥ��)
bool MusicOn = 0;
bool IntroState = 0;
double MoveIntroY = -1000;
double MoveIntroZ = -1000;
char* string1 = { "Check" };
char* string2 = { "CheckMate  You Lose!" };
int SelMenu = 0;
GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f }; //��ü�� �����ϰ� ��Ÿ���� ��
GLfloat diffuseLight[] = { 0.01f, 0.01f, 0.01f, 1.0f }; //��ü�� �ֵ� ����
GLfloat lightPos[] = { 0.0f, 400.0f, 0.0f, 1.0f }; //������ ��ġ
GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f }; //��ü�� ���� ��� �� ��
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };

class Object {
protected:
	int x, y, z;
	int MoveObjX, MoveObjZ;
	bool state;
	int CollideAngle = 0;				// �浹 ȸ��
public:
	virtual void drawobj() = 0;            // �𵨸�
	virtual void moveobj() = 0;            // ������
	virtual void movedraw() = 0;         // �����ִ°� �׷��ֱ�
	virtual void collide() = 0;            // �浹 �ִϸ��̼�
	virtual void setY(int n) = 0;
	virtual void setZ(int n) = 0;
	virtual int getX() = 0;
	virtual int getY() = 0;
	virtual int getZ() = 0;
};
class Pawn : public Object {
private:
	bool MoveCount = 0;
public:
	Pawn(int X, int Z) {
		x = X;
		z = Z;
		y = 10;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		GLUquadric* ObjQuad = gluNewQuadric();

		glPushMatrix();

		
		glRotatef(CollideAngle, 1, 0, 0);
		glScaled(0.7, 0.7, 0.7);
		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 35.0, 10, 30, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 15.0, 15, 60, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 60, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 85, 0);
		glutSolidSphere(20, 30, 30);
		glPopMatrix();

		glPopMatrix();

	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TurnState == 0) {
			ChessMap[(z + 400) / 100][(x+400)/100] = 0;
			if (MoveZ == z - 100 && MoveX == x && TimerFunCount == 1)
				MoveObjZ = -10;
			else if (MoveZ == z - 200 && MoveX == x && MoveCount != 1)
				MoveObjZ = -20;
		}
		else {
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
			if (MoveZ == z + 100 && MoveX == x && TimerFunCount == 1)
				MoveObjZ = 10;
			else if (MoveZ == z + 200 && MoveX == x && MoveCount != 1)
				MoveObjZ = 20;
		}
		z += MoveObjZ;
		MoveCount = 1;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();
		glScalef(1, 1, 1);
		if (TurnState == 0)
			glTranslatef(x, 1, z - 100);
		else
			glTranslatef(x, 1, z + 100);
		drawobj();
		DrawRec();
		glPopMatrix();
		if (MoveCount == 0) {            // ó���� 2ĭ �����̱�
			glPushMatrix();
			glScalef(1, 1, 1);
			if (TurnState == 0)
				glTranslatef(x, 1, z - 200);
			else
				glTranslatef(x, 1, z + 200);
			drawobj();
			DrawRec();
			glPopMatrix();
		}
		glDisable(GL_BLEND);
	}
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
class Bishop : public Object {
public:
	Bishop(int X, int Z) {
		x = X;
		z = Z;
		y = 10;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		glPushMatrix();
		glRotatef(CollideAngle, 1, 0, 0);


		GLUquadric* ObjQuad = gluNewQuadric();

		glPushMatrix();
		glScaled(0.8, 0.8, 0.8);
		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 40.0, 10, 30, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 20.0, 20, 60, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 60, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 65, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 80, 0);
		glScaled(0.7, 1.0, 0.7);
		glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 108, 0);
		glutSolidSphere(5, 20, 20);
		glPopMatrix();


		glPopMatrix();


		glPopMatrix();

	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TimerFunCount == 1) {
			MoveObjX = ((InputX - x) / 10);
			MoveObjZ = ((InputZ - z) / 10);
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
		}
		x += MoveObjX;
		z += MoveObjZ;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		// -400, -400
		if ((400 + z) * (400 + z) <= (400 + x) * (400 + x)) {      // -400(x), -400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i <= (400 + z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 + z) * (400 + z) > (400 + x) * (400 + x)) {      // -400(x), -400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i <= (400 + x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// 400, -400
		if ((-400 - z) * (-400 - z) <= (400 - x) * (400 - x)) {      // 400(x), -400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (-400 - z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((-400 - z) * (-400 - z) >(400 - x) * (400 - x)) {      // 400(x), -400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 - x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// -400, 400
		if ((400 - z) * (400 - z) < (400 + x) * (400 + x)) {      // -400(x), 400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (400 - z) / 100 + TurnState; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 - z) * (400 - z) >(-400 - x) * (-400 - x)) {      // -400(x), 400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 + x) / 100 + TurnState; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// 400, 400
		if ((400 - z) * (400 - z) < (400 - x) * (400 - x)) {      // 400(x), 400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (400 - z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 - z) * (400 - z) >(400 - x) * (400 - x)) {      // 400(x), 400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 - x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		glDisable(GL_BLEND);
	};
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
class Rook	 : public Object {
public:
	Rook(int X, int Z) {
		x = X;
		y = 10;
		z = Z;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		GLUquadric* ObjQuad2 = gluNewQuadric();
		glPushMatrix();

		glRotatef(CollideAngle, 1, 0, 0);
		glScaled(0.8, 0.8, 0.8);


		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad2, 35.0, 30, 20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad2, 30.0, 25, 90, 30, 30);
		glPopMatrix();



		glPushMatrix();
		glTranslated(0, 90, 0);
		glScaled(1.0, 0.6, 1.0);
		glRotated(90, 1, 0, 0);

		glutSolidTorus(15, 20, 30, 30);
		glPopMatrix();


		glPopMatrix();


	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TimerFunCount == 1) {
			MoveObjX = ((InputX - x) / 10);
			MoveObjZ = ((InputZ - z) / 10);
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
		}
		x += MoveObjX;
		z += MoveObjZ;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		for (int i = 1; i < (400 + x) / 100 + 1; ++i) {      // -400���� (x��)
			/*if (ChessMap[(400 + z) / 100][(400 + x - (100 * i)) / 100] == 1 || ChessMap[(400 + z) / 100][(400 + x - (100 * i)) / 100] == 2)
				break;*/
			glPushMatrix();
			glTranslatef(x - (100 * i), 1, z);
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 + z) / 100 + 1; ++i) {      // -400���� (z��)
			/*if (ChessMap[(400 + z - (100 * i)) / 100][(400 + x) / 100] == 1 || ChessMap[(400 + z - (100 * i)) / 100][(400 + x) / 100] == 2)
				break;*/
			glPushMatrix();
			glTranslatef(x, 1, z - (100 * i));
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 - x) / 100; ++i) {      // 400���� (x��)
			/*if (ChessMap[(400 + z) / 100][(400 - x + (100 * i)) / 100] == 1 || ChessMap[(400 + z) / 100][(400 - x + (100 * i)) / 100] == 2)
				break;*/
			glPushMatrix();
			glTranslatef(x + (100 * i), 1, z);
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 - z) / 100; ++i) {      // 400���� (z��)
			/*if (ChessMap[(400 - z + (100 * i)) / 100][(400 + x) / 100] == 1 || ChessMap[(400 - z + (100 * i)) / 100][(400 + x) / 100] == 2)
				break;*/
			glPushMatrix();
			glTranslatef(x, 1, z + (100 * i));
			drawobj();
			DrawRec();
			glPopMatrix();
		};
		glDisable(GL_BLEND);
	}
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
class Knight : public Object {
public:
	Knight(int X, int Z) {
		x = X;
		z = Z;
		y = 10;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		glPushMatrix();
		glRotatef(CollideAngle, 1, 0, 0);

		GLUquadric* ObjQuad3 = gluNewQuadric();

		glScaled(0.8, 0.8, 0.8);


		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		glRotated(15, 0, 1, 0);
		gluCylinder(ObjQuad3, 35.0, 20, 60, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(15, 0, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad3, 20.0, 20, 90, 30, 30);
		glPopMatrix();

		glPushMatrix();

		glTranslated(30, 0, 0);
		glPushMatrix();
		glTranslated(0, 90, 0);
		glRotated(270, 0, 1, 0);
		glRotated(20, 1, 0, 0);
		glutSolidCone(25, 80, 30, 30);
		glPopMatrix();

		glPopMatrix();

		glPopMatrix();
	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TimerFunCount == 1) {
			MoveObjX = ((InputX - x) / 10);
			MoveObjZ = ((InputZ - z) / 10);
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
		}
		x += MoveObjX;
		z += MoveObjZ;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();                  // �տ�
		if (TurnState == 0 && x > -400 && z > -300) {
			glTranslatef(x - 100, 1, z - 200);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x < 300 && z < 200) {
			glTranslatef(x + 100, 1, z + 200);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //�տ�
		if (TurnState == 0 && x < 300 && z > -300) {
			glTranslatef(x + 100, 1, z - 200);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x > -400 && z < 200) {
			glTranslatef(x - 100, 1, z + 200);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //�޾�
		if (TurnState == 0 && x > -300 && z > -400) {
			glTranslatef(x - 200, 1, z - 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x < 200 && z < 300) {
			glTranslatef(x + 200, 1, z + 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //�޵�
		if (TurnState == 0 && x > -300 && z < 300) {
			glTranslatef(x - 200, 1, z + 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x < 200 && z > -400) {
			glTranslatef(x + 200, 1, z - 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //����
		if (TurnState == 0 && x < 200 && z > -400) {
			glTranslatef(x + 200, 1, z - 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x > -300 && z < 300) {
			glTranslatef(x - 200, 1, z + 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //����
		if (TurnState == 0 && x < 200 && z < 300) {
			glTranslatef(x + 200, 1, z + 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x > -300 && z > -400) {
			glTranslatef(x - 200, 1, z - 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // �ڿ�
		if (TurnState == 0 && x > -400 && z < 200) {
			glTranslatef(x - 100, 1, z + 200);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x < 300 && z > -300) {
			glTranslatef(x + 100, 1, z - 200);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  //�ڿ�
		if (TurnState == 0 && x < 300 && z < 200) {
			glTranslatef(x + 100, 1, z + 200);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x > -400 && z > -300) {
			glTranslatef(x - 100, 1, z - 200);
			drawobj();
			DrawRec();
		}
		glPopMatrix();
		glDisable(GL_BLEND);
	};
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
class King : public Object {
public:
	King(int X, int Z) {
		x = X;
		y = 10;
		z = Z;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		glPushMatrix();
		glRotatef(CollideAngle, 1, 0, 0);
		GLUquadric* ObjQuad = gluNewQuadric();

		glPushMatrix();

		glScaled(1, 1, 1);
		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 40.0, 10, 30, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 30.0, 15, 80, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 80, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 85, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 85, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 20.0, 25, 20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 105, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(30, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 118, 0);
		glRotated(45, 0, 0, 1);
		glutSolidCube(15);
		glPopMatrix();


		glPopMatrix();
		glPopMatrix();
	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TimerFunCount == 1) {
			MoveObjX = ((InputX - x) / 10);
			MoveObjZ = ((InputZ - z) / 10);
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
		}
		x += MoveObjX;
		z += MoveObjZ;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();                  // ��
		if (TurnState == 0 && z > -400) {
			glTranslatef(x, 1, z - 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z < 300) {
			glTranslatef(x, 1, z + 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // �տ�
		if (TurnState == 0 && z > -400 && x < 300) {
			glTranslatef(x + 100, 1, z - 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z < 300 && x > -400) {
			glTranslatef(x - 100, 1, z + 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // �տ�
		if (TurnState == 0 && z > -400 && x > -400) {
			glTranslatef(x - 100, 1, z - 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z < 300 && x < 300) {
			glTranslatef(x + 100, 1, z + 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // ��
		if (TurnState == 0 && x < 300) {
			glTranslatef(x + 100, 1, z);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x > -400) {
			glTranslatef(x - 100, 1, z);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // ��
		if (TurnState == 0 && x > -400) {
			glTranslatef(x - 100, 1, z);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && x < 300) {
			glTranslatef(x + 100, 1, z);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // �ڿ�
		if (TurnState == 0 && z < 300 && x < 300) {
			glTranslatef(x + 100, 1, z + 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z > -400 && x > -400) {
			glTranslatef(x - 100, 1, z - 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // �ڿ�
		if (TurnState == 0 && z < 300 && x > -400) {
			glTranslatef(x - 100, 1, z + 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z > -400 && x < 300) {
			glTranslatef(x + 100, 1, z - 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();

		glPushMatrix();                  // ��
		if (TurnState == 0 && z < 300) {
			glTranslatef(x, 1, z + 100);
			drawobj();
			DrawRec();
		}
		else if (TurnState == 1 && z > -400) {
			glTranslatef(x, 1, z - 100);
			drawobj();
			DrawRec();
		}
		glPopMatrix();
		glDisable(GL_BLEND);
	}
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
class Queen : public Object {
public:
	Queen(int X, int Z) {
		x = X;
		y = 10;
		z = Z;
	}
	virtual void setY(int n) { y = n; }
	virtual void setZ(int n) { z = n; }
	virtual void drawobj() {
		glPushMatrix();
		glRotatef(CollideAngle, 1, 0, 0);

		GLUquadric* ObjQuad = gluNewQuadric();

		glPushMatrix();
		glScaled(0.9, 0.9, 0.9);
		glPushMatrix();
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 8, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(40, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 5, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 40.0, 10, 30, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 30.0, 15, 80, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 80, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(25, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 85, 0);
		glScaled(1.0, 0.2, 1.0);
		glutSolidSphere(20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 85, 0);
		glRotated(270, 1, 0, 0);
		gluCylinder(ObjQuad, 20.0, 25, 20, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 90, 0);
		glutSolidSphere(22, 30, 30);
		glPopMatrix();

		glPushMatrix();
		glTranslated(0, 115, 0);
		glutSolidSphere(5, 20, 20);
		glPopMatrix();


		glPopMatrix();
		glPopMatrix();

	};            // �𵨸�
	virtual void moveobj() {            // ������
		if (TimerFunCount == 1) {
			MoveObjX = ((InputX - x) / 10);
			MoveObjZ = ((InputZ - z) / 10);
			ChessMap[(z + 400) / 100][(x + 400) / 100] = 0;
		}
		x += MoveObjX;
		z += MoveObjZ;
	};
	virtual void movedraw() {            // �����ִ°� �׷��ֱ�
		glEnable(GL_BLEND);
		glColor4f(1, 1, 0, 0.2);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		for (int i = 1; i < (400 + x) / 100 + 1; ++i) {      // -400���� (x��)
			glPushMatrix();
			glTranslatef(x - (100 * i), 1, z);
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 + z) / 100 + 1; ++i) {      // -400���� (z��)
			glPushMatrix();
			glTranslatef(x, 1, z - (100 * i));
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 - x) / 100; ++i) {      // 400���� (x��)
			glPushMatrix();
			glTranslatef(x + (100 * i), 1, z);
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		for (int i = 1; i < (400 - z) / 100; ++i) {      // 400���� (z��)
			glPushMatrix();
			glTranslatef(x, 1, z + (100 * i));
			drawobj();
			DrawRec();
			glPopMatrix();
		};

		// -400, -400
		if ((400 + z) * (400 + z) <= (400 + x) * (400 + x)) {      // -400(x), -400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i <= (400 + z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 + z) * (400 + z) > (400 + x) * (400 + x)) {      // -400(x), -400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i <= (400 + x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// 400, -400
		if ((-400 - z) * (-400 - z) <= (400 - x) * (400 - x)) {      // 400(x), -400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (-400 - z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((-400 - z) * (-400 - z) >(400 - x) * (400 - x)) {      // 400(x), -400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 - x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z - 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// -400, 400
		if ((400 - z) * (400 - z) < (400 + x) * (400 + x)) {      // -400(x), 400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (400 - z) / 100 + TurnState; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 - z) * (400 - z) >(-400 - x) * (-400 - x)) {      // -400(x), 400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 + x) / 100 + TurnState; ++i) {
				glPushMatrix();
				glTranslatef(x - 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		// 400, 400
		if ((400 - z) * (400 - z) < (400 - x) * (400 - x)) {      // 400(x), 400(z) ���� z������ �� ����ﶧ
			for (int i = 1; i < (400 - z) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		else if ((400 - z) * (400 - z) >(400 - x) * (400 - x)) {      // 400(x), 400(z) ���� x������ �� ����ﶧ
			for (int i = 1; i < (400 - x) / 100; ++i) {
				glPushMatrix();
				glTranslatef(x + 100 * i, 1, z + 100 * i);
				drawobj();
				DrawRec();
				glPopMatrix();
			}
		}
		glDisable(GL_BLEND);
	}
	virtual void collide() {
		if (TurnState == 0)
			z -= 3;
		else
			z += 3;
		y += 3;
		CollideAngle += 20;
	};            // �浹 �ִϸ��̼�
	virtual int getX() { return x; }      // X�� ��ȯ
	virtual int getY() { return y; }      // Y�� ��ȯ
	virtual int getZ() { return z; }      // Z�� ��ȯ
};
Object** ChessObj;
void main(int argc, char *argv[])
{
	InitObject();                                    // �� ��ġ �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);   // ���÷��� ��� ����
	glutInitWindowPosition(100, 100);                     // �������� ��ġ����
	glutInitWindowSize(1000, 1000);                        // �������� ũ�� ����
	glutCreateWindow("3D Chess");                        // ������ ���� (������ �̸�)
	glutTimerFunc(20, TimerFunction, 1);                  // �ð�����
	glutSpecialFunc(SpecialKeyboard);                     // Ű���� �Է�
	glutKeyboardFunc(Keyboard);                           // Ű���� �Է�
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(Motion);
	glutDisplayFunc(drawScene);                           // ��� �Լ��� ����
	glutReshapeFunc(Reshape);
	PlaySound(TEXT("classic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	glutMainLoop();

	for (int i = 0; i < 32; ++i) {                        // �Ҵ� ����
		ChessObj[i] = nullptr;
		delete ChessObj[i];
	}
	delete[] ChessObj;
}

GLvoid drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // �ʱ�ȭ
	glEnable(GL_DEPTH_TEST);  //����˻� ����
	glShadeModel(GL_SMOOTH);  //���̵�

	if (IntroState == 0)
	{
		gluLookAt(0.0, 100.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		glTranslated(-50, 0, 0);

		glPushMatrix();
		glBegin(GL_QUADS);

		glTexCoord2d(0, 1);
		glVertex3f(-290, 0, -290);

		glTexCoord2d(0, 0);
		glVertex3f(-290, 0, 290);

		glTexCoord2d(1, 0);
		glVertex3f(290, 0, 290);

		glTexCoord2d(1, 1);
		glVertex3f(290, 0, -290);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();


		glPopMatrix();


		glPushMatrix();
		glColor3d(1, 1, 1);
		glTranslated(-200, -300, 100);
		glRotated(90, 1, 0, 0);
		glRotated(180, 0, 0, 1);
		ChessObj[11]->drawobj();
		glPopMatrix();

		if (SelMenu == 1)
		{
			glPushMatrix();
			glColor3d(1, 1, 1);
			glLineWidth(10);
			glTranslated(115, 0, 0);
			glBegin(GL_LINE_LOOP);

			glVertex3f(-70, 10, 0);
			glVertex3f(-70, 10, 50);
			glVertex3f(70, 10, 50);
			glVertex3f(70, 10, 0);
			glEnd();
			glPopMatrix();

		}
		if (SelMenu == 2)
		{
			glPushMatrix();
			glColor3d(1, 1, 1);
			glLineWidth(10);
			glTranslated(115, 0, 83);
			glBegin(GL_LINE_LOOP);
			glVertex3f(-45, 10, 0);
			glVertex3f(-45, 10, 50);
			glVertex3f(65, 10, 50);
			glVertex3f(65, 10, 0);
			glEnd();
			glPopMatrix();
		}
	}

	else if (IntroState == 1) {
		gluLookAt(0.0, 1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

		if (Check == 1) {
			glRasterPos3f(0, 80, 0);
			for (int i = 0; i < strlen(string1); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string1[i]);   //check
		}
		if(CheckMate == 1){
			glRasterPos3f(0, 80, 0);
			for (int i = 0; i < strlen(string2); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string2[i]);   // check mate
		}


		glPushMatrix();
		glRotated(-45, 1, 0, 0);
		glTranslatef(-100, 0, -2500);

		glColor3f(0.8, 0.8, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[2]);

		glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f(-2000, -2000, 0);
		glTexCoord2f(1, 0);
		glVertex3f(-2000, 2000, 0);
		glTexCoord2f(0, 0);
		glVertex3f(2000, 2000, 0);
		glTexCoord2f(0, 1);
		glVertex3f(2000, -2000, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glPushMatrix();

		glTranslated(0, MoveIntroY, MoveIntroZ);
		glTranslatef(dTransX, -300 + dTransY, -500 + dTransZ);

		glRotatef(iAngleX, 1, 0, 0);
		glRotatef(iAngleY, 0, 1, 0);
		glRotatef(iAngleZ, 0, 0, 1);
		glRotatef(TurnY, 0, 1, 0);


		glPushMatrix();
		// �ٴ�
		glTranslatef(-50, 0, -50);

		glColor3f(0.8, 0.8, 1.0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f(-441, 0, -441);
		glTexCoord2f(1, 0);
		glVertex3f(-441, 0, 441);
		glTexCoord2f(0, 0);
		glVertex3f(441, 0, 441);
		glTexCoord2f(0, 1);
		glVertex3f(441, 0, -441);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();


		glPushMatrix();
		for (int i = 0; i < 32; ++i)               // �� �׸���
		{
			if (i < 16) {
				glColor3d(1.0f, 1.0f, 1.0f);
				glPushMatrix();
				glTranslatef(ChessObj[i]->getX(), ChessObj[i]->getY(), ChessObj[i]->getZ());
				ChessObj[i]->drawobj();
				glPopMatrix();
			}
			else {
				glColor3ub(0, 0, 0);
				glPushMatrix();
				glTranslatef(ChessObj[i]->getX(), ChessObj[i]->getY(), ChessObj[i]->getZ());
				ChessObj[i]->drawobj();
				glPopMatrix();
			}
		}
		glPopMatrix();

		DrawPos();                              // Ű���� ��ġ �׸���

		if (MoveDraw == 1 && ((TurnState == 1 && SelectObj < 16) || (TurnState == 0 && SelectObj > 15))) {                     // �����ϼ� �ִ°� �׷��ֱ�
			glPushMatrix();
			ChessObj[SelectObj]->movedraw();
			glPopMatrix();
		}

		if (MoveDraw == 2 && TurnState == 1 && SelectObj < 16) {                        // �� ������
			ChessObj[SelectObj]->moveobj();
			for (int i = 0; i < 16; ++i) {
				if (ChessObj[SelectObj]->getX() == ChessObj[i + 16]->getX() && ChessObj[SelectObj]->getZ() == ChessObj[i + 16]->getZ()) {
					ChessObj[i + 16]->collide();
					CollideNum = i + 16;
					CollideCount = 1;
				}
			}
		}
		if (MoveDraw == 2 && TurnState == 0 && SelectObj > 15) {                        // �� ������
			ChessObj[SelectObj]->moveobj();
			for (int i = 0; i < 16; ++i) {
				if (ChessObj[SelectObj]->getX() == ChessObj[i]->getX() && ChessObj[SelectObj]->getZ() == ChessObj[i]->getZ()) {
					ChessObj[i]->collide();
					CollideNum = i;
					CollideCount = 1;
				}
			}
		}
		glPopMatrix();
	}
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0, 1.0, 5000.0);
	glTranslatef(50, 0, -400);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_NORMALIZE);
	glLoadIdentity();
	glGenTextures(6, textures); // n���� �̹��� �ؽ�ó ������ �Ѵ�.
	LoadTexture("ChessBoard.bmp", 0);
	LoadTexture("bintro.bmp", 1);
	LoadTexture("space.bmp", 2);
	glEnable(GL_LIGHTING); // ���� ����Ѵ�.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, diffuseLight);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, specularLight);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glEnable(GL_LIGHT0);
}

void LoadTexture(const char* filename, int count)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // �ؽ�ó ��� ����

	glBindTexture(GL_TEXTURE_2D, textures[count]); //�ؽ�ó�� ��ü�� �����Ѵ�.
	pBytes = LoadDIBitmap(filename, &info); //�̹��� �ε��� �Ѵ�.
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 882, 882, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes); //�ؽ�ó ���� ���Ǹ� �Ѵ�.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //�ؽ�ó �Ķ���� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //�ؽ�ó �Ķ���� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     //�ؽ�ó �Ķ���� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     //�ؽ�ó �Ķ���� ����

																	  //   glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // �ؽ�ó ��� ����
}

GLubyte * LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;
	// ���̳ʸ� �б� ���� ������ ����
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;
	// ��Ʈ�� ���� ����� �д´�.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}
	// ������ BMP �������� Ȯ���Ѵ�.
	if (header.bfType != 'MB') {
		fclose(fp);
		return NULL;
	}
	// BITMAPINFOHEADER ��ġ�� ����.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	// ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		fclose(fp);
		exit(0);
		return NULL;
	}
	// ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize) {
		free(*info);
		fclose(fp);
		return NULL;
	}
	// ��Ʈ���� ũ�� ����
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth *
		(*info)->bmiHeader.biBitCount + 7) / 8.0 *
		abs((*info)->bmiHeader.biHeight);
	// ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL) {
		free(*info);
		fclose(fp);
		return NULL;
	}
	// ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�.
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize) {
		free(*info); free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}

GLvoid Keyboard(unsigned char key, int a, int b) {
	if (key == 'x')
		iAngleX += 10;
	else if (key == 'X')
		iAngleX -= 10;
	else if (key == 'y')
		iAngleY += 10;
	else if (key == 'Y')
		iAngleY -= 10;
	else if (key == 'z')
		iAngleZ += 10;
	else if (key == 'Z')
		iAngleZ -= 10;
	if (key == '1')
		dTransZ += 10;
	else if (key == '2')
		dTransZ -= 10;
	else if (key == '3')
		dTransX += 10;
	else if (key == '4')
		dTransX -= 10;
	else if (key == '5')
		dTransY += 10;
	else if (key == '6')
		dTransY -= 10;
	else if (key == 't') {
		if (TurnState == 0) {
			MoveZ = -400;
			Turn = 1;
		}
		else {
			MoveZ = 300;
			Turn = 1;
		}
		MoveDraw = 0;
	}
	else if (key == 'r')      // ���� ���
		MoveDraw = 0;
	else if (key == ' ') {      // �����ϼ� �ִ°� �����ֱ�
		if (MoveDraw == 0) {
			for (int i = 0; i < 32; ++i) {               // �����̽� �������� ���� �� ����
				if (ChessObj[i]->getX() == MoveX && ChessObj[i]->getZ() == MoveZ) {
					SelectObj = i;
					MoveDraw = 1;
				}
			}
		}
		else if (MoveDraw == 1) {
			MoveDraw = 2;
		}
		InputX = MoveX;
		InputZ = MoveZ;
		
	}
	else if (key == 'c') {
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 8; ++j) {
				if (ChessMap[i][j] == 1)
					ChessMap[i][j] = 0;
			}
		}
		for (int i = 0; i < 32; ++i) {
			if (i < 17 && i != 11 || i == 23)
				ChessObj[i]->setY(2000);
			if (i == 24) {
				ChessObj[i]->setZ(-400);
				ChessMap[0][0] = 1;
				ChessMap[7][0] = 0;
			}
			Check = 1;
		}
	}
	else if (key == 'C') {
		ChessObj[31]->setZ(-300);
		CheckMate = 1;
	}
	else if (key == 'm'){
		PlaySound(NULL, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT | SND_PURGE);
	}

	else if (key == 'i'){
		PlaySound(TEXT("classic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
		IntroState = 0;
	}
}

GLvoid SpecialKeyboard(int key, int x, int y) {
	if (CheckMate == 0) {
		if (key == GLUT_KEY_LEFT) {
			if (TurnState == 0 && MoveX > -400)
				MoveX -= 100;
			else if (TurnState == 1 && MoveX < 300)
				MoveX += 100;
		}
		else if (key == GLUT_KEY_RIGHT) {
			if (TurnState == 0 && MoveX < 300)
				MoveX += 100;
			else if (TurnState == 1 && MoveX > -400)
				MoveX -= 100;
		}
		else if (key == GLUT_KEY_UP) {
			if (TurnState == 0 && MoveZ > -400)
				MoveZ -= 100;
			else if (TurnState == 1 && MoveZ < 300)
				MoveZ += 100;
		}
		else if (key == GLUT_KEY_DOWN) {
			if (TurnState == 0 && MoveZ < 300)
				MoveZ += 100;
			else if (TurnState == 1 && MoveZ > -400)
				MoveZ -= 100;
		}
	}
}

GLvoid TimerFunction(int value) {
	// �� ��ü ȸ��
	if (TurnState == 1 && TurnY < 360 && Turn == 1) {
		TurnY += 5;
		if (TurnY < 280) {
			dTransX += 5;
			dTransZ += 5;
		}
	}
	else if (TurnY == 360) {
		Turn = 0;
		TurnY = 0;
		TurnState = 0;
	}
	else if (TurnState == 0 && TurnY < 180 && Turn == 1) {
		TurnY += 5;
		if (TurnY < 90)
			dTransY -= 10;
		else if (TurnY >= 90)
			dTransY += 10;
		if (TurnY < 100) {
			dTransX -= 5;
			dTransZ -= 5;
		}
	}
	else if (TurnY == 180) {
		TurnState = 1;
		Turn = 0;
	}
	if (MoveDraw == 2) {                    // ������
		if (TimerFunCount<5)
			ChessObj[SelectObj]->setY(20 * TimerFunCount);
		else
			ChessObj[SelectObj]->setY(100 - (TimerFunCount - 5) * 20);
		++TimerFunCount;
	}
	if (TimerFunCount == 11) {
		MoveDraw = 0;
		TimerFunCount = 0;
		Turn = 1;
		if(SelectObj == 11 || SelectObj == 27)
			ChessMap[(ChessObj[SelectObj]->getZ() + 400) / 100][(ChessObj[SelectObj]->getX() + 400) / 100] = 2;
		else
			ChessMap[(ChessObj[SelectObj]->getZ() + 400) / 100][(ChessObj[SelectObj]->getX() + 400) / 100] = 1;
		/*if (Check == 1) {
			Check = 0;
			CheckMate = 1;
		}*/
	}
	// ��


	if (IntroState == 1)
	{
		if (MoveIntroY != 0 && MoveIntroZ != 0)
		{
			iAngleY += 3;
			MoveIntroY += 10;
			MoveIntroZ += 10;

			if (MoveIntroY == 0)
				iAngleY = 0; 
		}
	}
	if(CollideCount == 1)
		ChessObj[CollideNum]->collide();				// �浹 ������

	system("cls");
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			cout << ChessMap[i][j] << " ";
		}
		cout << endl;
	}

	glutTimerFunc(20, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid InitObject() {               // ü�� �� �ʱ�ȭ
	ChessObj = new Object*[32];

	for (int i = 0; i < 32; ++i) {
		if (i < 8)
			ChessObj[i] = new Pawn(-400 + 100 * i, -300);
		else if (i == 8)
			ChessObj[i] = new Rook(-400 + 100 * (i - 8), -400);
		else if (i == 9)
			ChessObj[i] = new Knight(-400 + 100 * (i - 8), -400);
		else if (i == 10)
			ChessObj[i] = new Bishop(-400 + 100 * (i - 8), -400);
		else if (i == 11)
			ChessObj[i] = new King(-400 + 100 * (i - 8), -400);
		else if (i == 12)
			ChessObj[i] = new Queen(-400 + 100 * (i - 8), -400);
		else if (i == 13)
			ChessObj[i] = new Bishop(-400 + 100 * (i - 8), -400);
		else if (i == 14)
			ChessObj[i] = new Knight(-400 + 100 * (i - 8), -400);
		else if (i == 15)
			ChessObj[i] = new Rook(-400 + 100 * (i - 8), -400);
		else if (i > 15 && i < 24)
			ChessObj[i] = new Pawn(-400 + 100 * (i - 16), 200);
		else if (i == 24)
			ChessObj[i] = new Rook(-400 + 100 * (i - 24), 300);
		else if (i == 25)
			ChessObj[i] = new Knight(-400 + 100 * (i - 24), 300);
		else if (i == 26)
			ChessObj[i] = new Bishop(-400 + 100 * (i - 24), 300);
		else if (i == 27)
			ChessObj[i] = new King(-400 + 100 * (i - 24), 300);
		else if (i == 28)
			ChessObj[i] = new Queen(-400 + 100 * (i - 24), 300);
		else if (i == 29)
			ChessObj[i] = new Bishop(-400 + 100 * (i - 24), 300);
		else if (i == 30)
			ChessObj[i] = new Knight(-400 + 100 * (i - 24), 300);
		else if (i == 31)
			ChessObj[i] = new Rook(-400 + 100 * (i - 24), 300);
	}
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (i == 0 || i == 1 || i == 6 || i == 7)
				ChessMap[i][j] = 1;
		}
	}
	ChessMap[0][3] = 2;
	ChessMap[7][3] = 2;
}

GLvoid DrawPos() {               // Ű���忡 ���� �����̴� �簢�� �׸���
	glPushMatrix();
	glColor3d(1, 0, 0);
	glLineWidth(5);
	if (TurnState == 0)
		glTranslated(MoveX, 2, MoveZ);
	else
		glTranslated(MoveX, 2, MoveZ);
	glBegin(GL_LINE_STRIP);
	glVertex3d(50, 0, 50);
	glVertex3d(-50, 0, 50);
	glVertex3d(-50, 0, -50);
	glVertex3d(50, 0, -50);
	glVertex3d(50, 0, 50);
	glEnd();
	glPopMatrix();
	cout << MoveX << " " << MoveZ << endl;
}

GLvoid DrawRec() {               // �簢��
	glBegin(GL_LINE_STRIP);
	glVertex3d(50, 0, 50);
	glVertex3d(-50, 0, 50);
	glVertex3d(-50, 0, -50);
	glVertex3d(50, 0, -50);
	glVertex3d(50, 0, 50);
	glEnd();
}

void Mouse(int button, int state, int x, int y){
		if ((650 < x && 920 > x) && (480 < y && 600 > y))		{
			PlaySound(TEXT("goni.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);	// ��� ����
			IntroState = 1;
		}
		else if ((710 < x && 910> x) && (640 < y && 740 > y))
			exit(0);
	glutPostRedisplay();
}

GLvoid Motion(int x, int y){
	if ((650 < x && 920 > x) && (480 < y && 600 > y))	{
		SelMenu = 1;
	}
	else if ((710 < x && 910> x) && (640 < y && 740 > y))	{
		SelMenu = 2;
	}
}