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
	int dx = xEnd - x0, dy = yEnd - y0, steps; // dx: x ��ȭ��, dy: y ��ȭ��, steps: �� ĥ�ؾ� �� �ȼ��� ����
	float xIncrement, yIncrement, x = x0, y = y0;

	// x ��ȭ���� ũ�� xIncrement = 1 �Ǵ� -1, y ��ȭ���� ũ�� yIncrement = 1 �Ǵ� -1
	if (fabs(dx) > fabs(dy)) steps = fabs(dx); else steps = fabs(dy); // fabs(): ����

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);

	// ������ �׸���
	setPixel(round(x), round(y)); // round(): �ݿø�

	// ��ȭ���� ���� �� ���(���� �׸���)
	for (int k = 0; k < steps; k++) {
		x += xIncrement;
		y += yIncrement;
		setPixel(round(x), round(y));
	}
}

// Bresenham �˰���
// �߰��� �˰���(���� �˰���)
// �� �ĺ��� ������ �߰����� ���� ���� �������� y���� 1�����ϰ�, ���� �Ʒ��� �������� �״�� �����Ǵ� �˰���
// ���⼭�� ���Ⱑ 0�� 1 ���̶�� �����ϹǷ� x�� ������ y���� ŭ (x�� ������ 1����)
void bresenham_line(int xl, int yl, int xr, int yr) {
	int x, y = yl, W = xr - xl, H = yr - yl; // W: ���� �Ÿ�, H: ���� �Ÿ�
	// F: �Ǻ���, dF1: F�� ������ �� ����ϴ� ����, dF2: F�� ����� �� ����ϴ� ����
	int F = 2 * H - W, dF1 = 2 * H, dF2 = 2 * (H - W);

	// �� ����(���� �׸���) �� �Ǻ��Ŀ� ���� ��ǥ ��ȭ
	for (x = xl; x <= xr; x++) {
		setPixel(x, y);
		if (F < 0) F += dF1;
		else {
			y++;
			F += dF2;
		}
	}
}