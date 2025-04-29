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
	int dx = xEnd - x0, dy = yEnd - y0, steps; // dx: x 변화값, dy: y 변화값, steps: 총 칠해야 할 픽셀의 길이
	float xIncrement, yIncrement, x = x0, y = y0;

	// x 변화값이 크면 xIncrement = 1 또는 -1, y 변화값이 크면 yIncrement = 1 또는 -1
	if (fabs(dx) > fabs(dy)) steps = fabs(dx); else steps = fabs(dy); // fabs(): 절댓값

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);

	// 시작점 그리기
	setPixel(round(x), round(y)); // round(): 반올림

	// 변화값에 따라 점 찍기(선분 그리기)
	for (int k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;
		setPixel(round(x), round(y));
	}
}

// Bresenham 알고리즘
// 중간점 알고리즘(선분 알고리즘)
// 두 후보점 사이의 중간점이 선분 위로 지나가면 y값이 1증가하고, 선분 아래로 지나가면 그대로 유지되는 알고리즘
// 여기서는 기울기가 0과 1 사이라고 가정하므로 x는 무조건 y보다 큼 (x는 무조건 1증가)
void bresenham_line(int xl, int yl, int xr, int yr) {
	int x, y = yl, W = xr - xl, H = yr - yl; // W: 수평 거리, H: 수직 거리
	// F: 판별식, dF1: F가 음수일 때 사용하는 정수, dF2: F가 양수일 때 사용하는 정수
	int F = 2 * H - W, dF1 = 2 * H, dF2 = 2 * (H - W);

	// 점 찍은(선분 그리기) 후 판별식에 따라 좌표 변화
	for (x = xl; x <= xr; x++) {
		setPixel(x, y);
		if (F < 0) F += dF1;
		else {
			y++;
			F += dF2;
		}
	}
}