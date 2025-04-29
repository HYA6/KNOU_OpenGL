#include <stdio.h>
#include <math.h>
#include <gl/freeglut.h>
#include <gl/glew.h>

// 해당 좌표 칠하는 함수
void setPixel(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

// 선분 알고리즘
// DDA(Digital Differential Analyzer) 알고리즘
// 한 축의 값을 1씩 변화시킬 때 다른 축의 값이 변화하는 값을 구하여 이어지는 선분상의 픽셀 좌표를 구하는 알고리즘
void DDA(int x0, int y0, int xEnd, int yEnd) {
	// x0 ≠ xEnd 또는 y0 ≠ yEnd라고 가정
	int dx = xEnd - x0, dy = yEnd - y0, steps; // dx: x
	float xIncrement, yIncrement, x = x0, y = y0;

	if (fabs(dx) > fabs(dy)) steps = fabs(dx); else steps = fabs(dy);

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);

	setPixel(round(x), round(y));

	for (int k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;
		setPixel(round(x), round(y));
	}
}