#include <stdio.h>
#include <math.h>
#include <gl/freeglut.h>
#include <gl/glew.h>

// �ش� ��ǥ ĥ�ϴ� �Լ�
void setPixel(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

// ���� �˰���
// DDA(Digital Differential Analyzer) �˰���
// �� ���� ���� 1�� ��ȭ��ų �� �ٸ� ���� ���� ��ȭ�ϴ� ���� ���Ͽ� �̾����� ���л��� �ȼ� ��ǥ�� ���ϴ� �˰���
void DDA(int x0, int y0, int xEnd, int yEnd) {
	// x0 �� xEnd �Ǵ� y0 �� yEnd��� ����
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