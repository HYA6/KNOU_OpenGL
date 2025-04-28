#include  <stdio.h>

#include  <gl/glew.h>
#include  <gl/freeglut.h>

typedef struct {	// 3차원 좌표를 표현하기 위한 구조체
    float x, y, z;
} Vec3f;

enum { TRIANGLE, N_VBOs }; // 열거형(enum)을 이용하여 VBO 번호에 이름 부여
// (TRIANGLE에는 0번 부여, N_VBOs에는 VBO 개수에 해당하는 1번 부여)
GLuint VBO[N_VBOs]; // 꼭짓점 버퍼 객체 (VBO 핸들 저장하는 배열)

static const char* pVS = // 꼭짓점 셰이더 소스 (각 꼭짓점의 위치 결정)
"#version 330                                               \n" // GLSL 버전 3.3 사용
"layout (location = 0) in vec3 Position;                    \n" // 꼭짓점 정보가 vec3 값으로 표현되는 전역변수 Position에 입력됨
"                                                           \n" // layout (location = 0): 꼭짓점 속성이 전달될 인덱스가 0번
"void main()                                                \n"
"{                                                          \n"
"    gl_Position = vec4(Position*0.1, 1.0);                 \n" // Position에 0.1을 곱함으로써 삼각형의 크기가 1/10으로 줄어듬
"}";

static const char* pFS = // 프래그먼트 셰이더 소스 (픽셀의 색 결정)
"#version 330                                               \n"
"out vec4 FragColor;                                        \n" // 색을 출력하기 위한 전역변수 FragColor(RGB-A)
"                                                           \n"
"void main()                                                \n"
"{                                                          \n"
"    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                  \n" // 빨강 삼각형이 나타남
"}";

// 셰이더 프로그램을 컴파일하여 셰이더 프로그램 객체에 첨부
void AddShader(GLuint shaderProg, const char* pShaderSrc, GLint ShaderType)
{   // 셰이더 생성
    GLuint shader = glCreateShader(ShaderType);
    if (!shader) {
        fprintf(stderr, "오류 - Shader 생성(%d)\n", ShaderType);
        exit(0);
    }
    // 셰이더 컴파일
    const GLchar* src[1] = { pShaderSrc };
    const GLint len[1] = { strlen(pShaderSrc) };
    glShaderSource(shader, 1, src, len); // 셰이더 소스코드를 셰이더 객체에 전달 (포인터 배열+문자열의 길이 배열 전달)
    glCompileShader(shader); // 셰이더 컴파일
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // 컴파일이 올바르게 되었는지 확인
    if (!success) { // 컴파일 오류 발생
        GLchar infoLog[256];
        glGetShaderInfoLog(shader, 256, NULL, infoLog); // 오류가 있으면 프로그램 중단
        fprintf(stderr, "오류 - Shader 컴파일(%d): %s\n", ShaderType, infoLog);
        exit(1);
    }
    glAttachShader(shaderProg, shader); // 셰이더 프로그램에 컴파일된 셰이더를 추가
}

// 준비된 셰이더 프로그램을 컴파일하고 링크하여 GPU에서 동작하도록 작업 (SetUpShaders 함수에 AddShader 함수 포함)
void SetUpShaders()
{
    GLuint shaderProg = glCreateProgram(); // 셰이더 프로그램 객체 생성
    if (!shaderProg) {
        fprintf(stderr, "오류 - Shader 프로그램 생성\n");
        exit(1);
    }

    // 꼭짓점 셰이더 및 프래그먼트 셰이더 적재
    AddShader(shaderProg, pVS, GL_VERTEX_SHADER); // 꼭짓점 셰이더
    AddShader(shaderProg, pFS, GL_FRAGMENT_SHADER); // 프래그먼트 셰이더

    GLint success = 0;
    GLchar errLog[256];

    glLinkProgram(shaderProg); // 셰이더 프로그램 링크
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &success); // 링크가 잘 되었는지 확인
    if (!success) {
        glGetProgramInfoLog(shaderProg, sizeof(errLog), NULL, errLog); // 오류가 있으면 프로그램 중단
        fprintf(stderr, "오류 - Shader 프로그램 링크: %s\n", errLog);
        exit(1);
    }

    glValidateProgram(shaderProg); // 프로그램 객체가 유효한지 검사
    glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &success); // 링크가 잘 되었는지 확인
    if (!success) {
        glGetProgramInfoLog(shaderProg, sizeof(errLog), NULL, errLog);
        fprintf(stderr, "Invalid shader program: %s\n", errLog);
        exit(1);
    }
    glUseProgram(shaderProg); // 현재 셰이더 프로그램 객체로 지정
}

// 그림 생성
void RenderCB()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // RGB+a 화면을 지울 색 지정
    glClear(GL_COLOR_BUFFER_BIT); // 지울 대상 지정 (GL_COLOR_BUFFER_BIT을 백색으로 화면 지움)

    glEnableVertexAttribArray(0); // 꼭짓점 속성 배열의 인덱스 활성화 (여기서 0은 16행에서 지정한 인덱스 값)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[TRIANGLE]); // VBO 다시 연결(VBO가 하나면 다시 연결할 필요 X, 여러 개일 경우 사용할 버퍼 연결)
    // glVertexAttribPointer(속성의 인덱스, 속성의 수, 속성의 자료형, 파이프라인에서 사용하기 전 정규화 여부, 데이터 시작 위치 사이의 간격, 속성이 위치하는 변위)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // 데이터를 꼭짓점 버퍼에서 어떻게 액세스할 것인지 지정
    // glDrawArrays(생성하려는 그래픽스 기본 요소 지정 상수, 첫 번째 꼭짓점 위치, 꼭짓점의 수)
    glDrawArrays(GL_TRIANGLES, 0, 3); // 그림 생성 (GL_TRIANGLES: 삼각형)
    glDisableVertexAttribArray(0); // 꼭짓점 속성 배열의 인덱스 비활성화 (glEnableVertexAttribArray 함수와 반대)
    glFinish(); // 실행 제어 (glFlush: 가능한 한 빨리 명령 실행, glFinish: 이전에 큐에 전달한 명령들 모두 실행 후 복귀)
}

// 그림 생성을 위한 준비
void InitVBOs() // VBO(Vertex Buffer Object): 꼭짓점 버퍼 객체
{
    Vec3f Vertices[3] = { //삼각형의 꼭짓점 좌표
        {-5.0f, -5.0f, 0.0f},
        {5.0f, -5.0f, 0.0f},
        {0.0f, 5.0f, 0.0f} };
    // 꼭짓점 버퍼를 생성하여 삼각형의 꼭짓점 좌표 전달
    glGenBuffers(N_VBOs, VBO); // VBO 핸들 1개 생성
    // glBindBuffer(목적지 이름, 핸들)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[TRIANGLE]); // VBO를 적절한 목적지 이름(포인터 같은?)에 연결
    // glBufferData(목적지 이름, 데이터 크기, 전달할 데이터, 데이터의 사용 형태)
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // 목적지 이름을 통해 VBO에 데이터 전달
    // GL_STATIC_DRAW: VBO에 한 번 데이터 지정 후 변경하지 않음
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv); // GLUT 초기화
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // 디스플레이 방식 옵션 설정 (defalt: 단일 버퍼링, RGB 색 모드)
    glutInitWindowPosition(50, 100); // 디스플레이 윈도의 좌상단 모서리 초기 위치
    glutInitWindowSize(640, 480); // 디스플레이 윈도의 초기 픽셀 폭과 높이 지정
    glutCreateWindow("OpenGL Sample"); // 디스플레이 윈도 생성

    GLenum s = glewInit();
    if (s != GLEW_OK) { // glewInit 함수는 정상적인 경우 GLEW_OK 반환
        fprintf(stderr, "오류 - %s\n", glewGetErrorString(s));
        return 1;
    }

    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    SetUpShaders(); // 준비된 셰이더 프로그램을 컴파일하고 링크하여 GPU에서 동작하도록 작업
    InitVBOs(); // 그림 생성을 위한 준비

    glutDisplayFunc(RenderCB); // 윈도를 디스플레이하기 위한 콜백 함수
    glutMainLoop(); // glutDisplayFunc로 지정된 콜백 함수에 따라 윈도를 화면에 디스플레이한 후 그림 출력

    return 0;
}