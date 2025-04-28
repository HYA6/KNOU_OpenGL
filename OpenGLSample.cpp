#include  <stdio.h>

#include  <gl/glew.h>
#include  <gl/freeglut.h>

typedef struct {	// 3���� ��ǥ�� ǥ���ϱ� ���� ����ü
    float x, y, z;
} Vec3f;

enum { TRIANGLE, N_VBOs }; // ������(enum)�� �̿��Ͽ� VBO ��ȣ�� �̸� �ο�
// (TRIANGLE���� 0�� �ο�, N_VBOs���� VBO ������ �ش��ϴ� 1�� �ο�)
GLuint VBO[N_VBOs]; // ������ ���� ��ü (VBO �ڵ� �����ϴ� �迭)

static const char* pVS = // ������ ���̴� �ҽ� (�� �������� ��ġ ����)
"#version 330                                               \n" // GLSL ���� 3.3 ���
"layout (location = 0) in vec3 Position;                    \n" // ������ ������ vec3 ������ ǥ���Ǵ� �������� Position�� �Էµ�
"                                                           \n" // layout (location = 0): ������ �Ӽ��� ���޵� �ε����� 0��
"void main()                                                \n"
"{                                                          \n"
"    gl_Position = vec4(Position*0.1, 1.0);                 \n" // Position�� 0.1�� �������ν� �ﰢ���� ũ�Ⱑ 1/10���� �پ��
"}";

static const char* pFS = // �����׸�Ʈ ���̴� �ҽ� (�ȼ��� �� ����)
"#version 330                                               \n"
"out vec4 FragColor;                                        \n" // ���� ����ϱ� ���� �������� FragColor(RGB-A)
"                                                           \n"
"void main()                                                \n"
"{                                                          \n"
"    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                  \n" // ���� �ﰢ���� ��Ÿ��
"}";

// ���̴� ���α׷��� �������Ͽ� ���̴� ���α׷� ��ü�� ÷��
void AddShader(GLuint shaderProg, const char* pShaderSrc, GLint ShaderType)
{   // ���̴� ����
    GLuint shader = glCreateShader(ShaderType);
    if (!shader) {
        fprintf(stderr, "���� - Shader ����(%d)\n", ShaderType);
        exit(0);
    }
    // ���̴� ������
    const GLchar* src[1] = { pShaderSrc };
    const GLint len[1] = { strlen(pShaderSrc) };
    glShaderSource(shader, 1, src, len); // ���̴� �ҽ��ڵ带 ���̴� ��ü�� ���� (������ �迭+���ڿ��� ���� �迭 ����)
    glCompileShader(shader); // ���̴� ������
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // �������� �ùٸ��� �Ǿ����� Ȯ��
    if (!success) { // ������ ���� �߻�
        GLchar infoLog[256];
        glGetShaderInfoLog(shader, 256, NULL, infoLog); // ������ ������ ���α׷� �ߴ�
        fprintf(stderr, "���� - Shader ������(%d): %s\n", ShaderType, infoLog);
        exit(1);
    }
    glAttachShader(shaderProg, shader); // ���̴� ���α׷��� �����ϵ� ���̴��� �߰�
}

// �غ�� ���̴� ���α׷��� �������ϰ� ��ũ�Ͽ� GPU���� �����ϵ��� �۾� (SetUpShaders �Լ��� AddShader �Լ� ����)
void SetUpShaders()
{
    GLuint shaderProg = glCreateProgram(); // ���̴� ���α׷� ��ü ����
    if (!shaderProg) {
        fprintf(stderr, "���� - Shader ���α׷� ����\n");
        exit(1);
    }

    // ������ ���̴� �� �����׸�Ʈ ���̴� ����
    AddShader(shaderProg, pVS, GL_VERTEX_SHADER); // ������ ���̴�
    AddShader(shaderProg, pFS, GL_FRAGMENT_SHADER); // �����׸�Ʈ ���̴�

    GLint success = 0;
    GLchar errLog[256];

    glLinkProgram(shaderProg); // ���̴� ���α׷� ��ũ
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &success); // ��ũ�� �� �Ǿ����� Ȯ��
    if (!success) {
        glGetProgramInfoLog(shaderProg, sizeof(errLog), NULL, errLog); // ������ ������ ���α׷� �ߴ�
        fprintf(stderr, "���� - Shader ���α׷� ��ũ: %s\n", errLog);
        exit(1);
    }

    glValidateProgram(shaderProg); // ���α׷� ��ü�� ��ȿ���� �˻�
    glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &success); // ��ũ�� �� �Ǿ����� Ȯ��
    if (!success) {
        glGetProgramInfoLog(shaderProg, sizeof(errLog), NULL, errLog);
        fprintf(stderr, "Invalid shader program: %s\n", errLog);
        exit(1);
    }
    glUseProgram(shaderProg); // ���� ���̴� ���α׷� ��ü�� ����
}

// �׸� ����
void RenderCB()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // RGB+a ȭ���� ���� �� ����
    glClear(GL_COLOR_BUFFER_BIT); // ���� ��� ���� (GL_COLOR_BUFFER_BIT�� ������� ȭ�� ����)

    glEnableVertexAttribArray(0); // ������ �Ӽ� �迭�� �ε��� Ȱ��ȭ (���⼭ 0�� 16�࿡�� ������ �ε��� ��)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[TRIANGLE]); // VBO �ٽ� ����(VBO�� �ϳ��� �ٽ� ������ �ʿ� X, ���� ���� ��� ����� ���� ����)
    // glVertexAttribPointer(�Ӽ��� �ε���, �Ӽ��� ��, �Ӽ��� �ڷ���, ���������ο��� ����ϱ� �� ����ȭ ����, ������ ���� ��ġ ������ ����, �Ӽ��� ��ġ�ϴ� ����)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // �����͸� ������ ���ۿ��� ��� �׼����� ������ ����
    // glDrawArrays(�����Ϸ��� �׷��Ƚ� �⺻ ��� ���� ���, ù ��° ������ ��ġ, �������� ��)
    glDrawArrays(GL_TRIANGLES, 0, 3); // �׸� ���� (GL_TRIANGLES: �ﰢ��)
    glDisableVertexAttribArray(0); // ������ �Ӽ� �迭�� �ε��� ��Ȱ��ȭ (glEnableVertexAttribArray �Լ��� �ݴ�)
    glFinish(); // ���� ���� (glFlush: ������ �� ���� ��� ����, glFinish: ������ ť�� ������ ��ɵ� ��� ���� �� ����)
}

// �׸� ������ ���� �غ�
void InitVBOs() // VBO(Vertex Buffer Object): ������ ���� ��ü
{
    Vec3f Vertices[3] = { //�ﰢ���� ������ ��ǥ
        {-5.0f, -5.0f, 0.0f},
        {5.0f, -5.0f, 0.0f},
        {0.0f, 5.0f, 0.0f} };
    // ������ ���۸� �����Ͽ� �ﰢ���� ������ ��ǥ ����
    glGenBuffers(N_VBOs, VBO); // VBO �ڵ� 1�� ����
    // glBindBuffer(������ �̸�, �ڵ�)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[TRIANGLE]); // VBO�� ������ ������ �̸�(������ ����?)�� ����
    // glBufferData(������ �̸�, ������ ũ��, ������ ������, �������� ��� ����)
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // ������ �̸��� ���� VBO�� ������ ����
    // GL_STATIC_DRAW: VBO�� �� �� ������ ���� �� �������� ����
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // GLUT �ʱ�ȭ
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // ���÷��� ��� �ɼ� ���� (defalt: ���� ���۸�, RGB �� ���)
    glutInitWindowPosition(50, 100); // ���÷��� ������ �»�� �𼭸� �ʱ� ��ġ
    glutInitWindowSize(640, 480); // ���÷��� ������ �ʱ� �ȼ� ���� ���� ����
    glutCreateWindow("OpenGL Sample"); // ���÷��� ���� ����

    GLenum s = glewInit();
    if (s != GLEW_OK) { // glewInit �Լ��� �������� ��� GLEW_OK ��ȯ
        fprintf(stderr, "���� - %s\n", glewGetErrorString(s));
        return 1;
    }

    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    SetUpShaders(); // �غ�� ���̴� ���α׷��� �������ϰ� ��ũ�Ͽ� GPU���� �����ϵ��� �۾�
    InitVBOs(); // �׸� ������ ���� �غ�

    glutDisplayFunc(RenderCB); // ������ ���÷����ϱ� ���� �ݹ� �Լ�
    glutMainLoop(); // glutDisplayFunc�� ������ �ݹ� �Լ��� ���� ������ ȭ�鿡 ���÷����� �� �׸� ���

    return 0;
}