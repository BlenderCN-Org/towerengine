#include "towerengine.h"

char *ftoa(float f, int len)
{
	char *buf = new char[len + 1];
	snprintf(buf, len + 1, "%f", f);
	int i;
	for(i=0; i<(int)strlen(buf); i++)
		buf[i] = buf[i] == ',' ? '.' : buf[i];
	return buf;
}

char *itoa(int v, int len)
{
	char *buf = new char[len + 1];
	snprintf(buf, len + 1, "%d", v);
	return buf;
}

char *cstr(const char *str)
{
	char *r;
	r = new char[strlen(str) + 1];
	snprintf(r, strlen(str)+1, "%s", str);
	return r;
}

void SetColor(GLenum which, float r, float g, float b)
{
	float c[4];

	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = 0.0;
	glColor3f(r, g, b);
	glMaterialfv(GL_FRONT_AND_BACK, which, c);
}

void PaintAxes(void)
{
	glPointSize(8.0);

	UseNoShader();

	glDisable(GL_LIGHTING);

	glLineWidth(3);

	SetColor(GL_DIFFUSE, 1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();

	SetColor(GL_DIFFUSE, 1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();

	SetColor(GL_DIFFUSE, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	SetColor(GL_DIFFUSE, 1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	glVertex3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
}


char *PathOfFile(const char *file)
{
    int i;
    char *path = 0;

    if(!file)
    {
        path = new char[0];
        return path;
    }

    for(i = strlen(file); i > 0; i--)
    {
        path = new char [i];
        snprintf(path, i, "%s", file);
        if(path[strlen(path) - 1] == '/' || path[strlen(path) - 1] == '\\')
            break;
    }

    return path;
}



// Matrix Functions

void CombineMatrix4(float MatrixA[16],float MatrixB[16], float *retM)
{
	float NewMatrix[16];
	int i;

	for(i = 0; i < 4; i++)
	{ //Cycle through each vector of first matrix.
		NewMatrix[i*4] = MatrixA[i*4] * MatrixB[0] + MatrixA[i*4+1] * MatrixB[4] + MatrixA[i*4+2] * MatrixB[8] + MatrixA[i*4+3] * MatrixB[12];
		NewMatrix[i*4+1] = MatrixA[i*4] * MatrixB[1] + MatrixA[i*4+1] * MatrixB[5] + MatrixA[i*4+2] * MatrixB[9] + MatrixA[i*4+3] * MatrixB[13];
		NewMatrix[i*4+2] = MatrixA[i*4] * MatrixB[2] + MatrixA[i*4+1] * MatrixB[6] + MatrixA[i*4+2] * MatrixB[10] + MatrixA[i*4+3] * MatrixB[14];
		NewMatrix[i*4+3] = MatrixA[i*4] * MatrixB[3] + MatrixA[i*4+1] * MatrixB[7] + MatrixA[i*4+2] * MatrixB[11] + MatrixA[i*4+3] * MatrixB[15];
	}
	memcpy(retM,NewMatrix,64);
}

void InverseMatrix4(float m[16], float *ret)
{
	float inv[16]; // The inverse will go here

	inv[0] = m[0];
	inv[1] = m[4];
	inv[2] = m[8];
	inv[4] = m[1];
	inv[5] = m[5];
	inv[6] = m[9];
	inv[8] = m[2];
	inv[9] = m[6];
	inv[10] = m[10];

	inv[12] = inv[0]*-m[12]+inv[4]*-m[13]+inv[8]*-m[14];
	inv[13] = inv[1]*-m[12]+inv[5]*-m[13]+inv[9]*-m[14];
	inv[14] = inv[2]*-m[12]+inv[6]*-m[13]+inv[10]*-m[14];

	inv[3] = 0.0f;
	inv[7] = 0.0f;
	inv[11] = 0.0f;
	inv[15] = 1.0f;

	memcpy(ret,inv,64);
}

CVector ApplyMatrix4(float m[16], CVector v)
{
	CVector r;
	r.x = Dot(Vec(m[0], m[1], m[2]), v) + m[3];
	r.y = Dot(Vec(m[4], m[5], m[6]), v) + m[7];
	r.z = Dot(Vec(m[8], m[9], m[10]), v) + m[11];

	return r;
}

void PrintMatrix4(float m[16])
{
	printf("%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f\n",
				m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15]);
}

