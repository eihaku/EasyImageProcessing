#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <conio.h>
#include <dos.h>
#include "params.h"


void image_read(unsigned char* image, int xsize, int ysize, char* filename)
{
	FILE* fp;
	int DataOffset;
	int DataSize;
	//	int res;

	if ((fp = fopen(filename, "rb")) == NULL) {
		printf("%s open errer [rb] \n", filename);
		exit(-1);
	}
	/* bmp����Υإå��`��� */
	fread(bmpHeader, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 1, fp);
	pBmpFileHeader = (PBITMAPFILEHEADER)bmpHeader;
	pBmpInfoHeader = (PBITMAPINFOHEADER)(bmpHeader + sizeof(BITMAPFILEHEADER));

	/* ����ӥå���ȴ_�J */
	if (pBmpInfoHeader->biBitCount > 8) {
		fclose(fp);
		printf("%s format error", filename);
	}

	/* ����ǩ`���η��ȸߤ��_�J */
	if (pBmpInfoHeader->biWidth > X_SIZE || pBmpInfoHeader->biHeight > Y_SIZE) {
		printf("%s  size error [%d]x[%d]\n", filename, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight);
	}
	DataSize = pBmpInfoHeader->biWidth * pBmpInfoHeader->biHeight;
	DataOffset = pBmpFileHeader->bfOffBits;

	/* bmp����ǩ`��Offset�_�J */
	if (DataOffset != BITMAP_HEAD_SIZE + BITMAP_COLORPALETTE_SIZE) {
		fclose(fp);
		printf("%s format error [offset]", filename);
	}

	/* ����`�ѥ�åȥǩ`��ȡ�� */
	fread(colorPalette, BITMAP_COLORPALETTE_SIZE, 1, fp);
	/* ���إǩ`��ȡ�� */
	fread(image, xsize, ysize, fp);
	fclose(fp);
}
void image_write(unsigned char* image, int xsize, int ysize, char* filename)
{
	FILE* fp;

	if ((fp = fopen(filename, "wb")) == NULL) {
		printf("%s open errer [wb] \n", filename);
		return;
	}

	fwrite(pBmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(pBmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(colorPalette, BITMAP_COLORPALETTE_SIZE, 1, fp);

	fwrite(image, xsize, ysize, fp);
	fclose(fp);
}

void image_copy(unsigned char image_dest[Y_SIZE][X_SIZE], unsigned char image_src[Y_SIZE][X_SIZE])
{
	/*copy*/
	memcpy((unsigned char*)image_dest, (unsigned char*)image_src, Y_SIZE * X_SIZE);
}

void image_copy2(unsigned char image_out[Y_SIZE][X_SIZE], unsigned char image_in[Y_SIZE][X_SIZE])
{
	/*copy*/
	for (int i = 0; i < Y_SIZE; i++) {
		memcpy((unsigned char*)image_out[i], (unsigned char*)image_in[Y_SIZE-i-1], X_SIZE);
	}
}

void histprint(long hist[]) {
	int i = 0;
	int j = 0;
	int k = 0;
	float p, q, max;
	p = X_SIZE * Y_SIZE;
	max = 0;
	for (i = 0; i < LEVEL; i++)if (hist[i] > max) max = hist[i];
	for (i = 0; i < LEVEL; i++) {
		q = (float)hist[i] / p * 100.0;
		fprintf(STDOUT, "%3d;%5.1f%%|", i, q);
		k = (float)hist[i] / max * 60.0;
		for (j = 0; j < k; j++) fprintf(STDOUT, "*");
		fprintf(STDOUT, "\r\n");
	}
}
/*--- histgram --- �ҥ��ȥ���������I�� -----------------------------------
	image_in:    ������������
	hist:        �ҥ��ȥ����
-----------------------------------------------------------------------------*/
void histgram(unsigned char image[Y_SIZE][X_SIZE], long hist[]) {
	int n = 0;
	int i = 0;
	int j = 0;

	for (n = 0; n < LEVEL; n++) hist[n] = 0;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			n = image[i][j];
			hist[n]++;
		}
	}
}

/*���ҥ��ȥ����λ��񻯡�
LEVEL = 256
HIST_X_SIZE = 5
HIST_y_SIZE = 10
*/
void histimage(long hist[], unsigned char image_hist[Y_SIZE][X_SIZE]) {
	int i = 0;
	int j = 0;
	int k = 0;
	unsigned char* image = (unsigned char*)image_hist;

	int  max, ratio, range;
	long n;
	float d;
	ratio = LEVEL / HIST_X_SIZE; //256/128
	range = HIST_Y_SIZE - 5;     //128-5
	for (i = 0; i < HIST_Y_SIZE * HIST_X_SIZE; i++) {
		image[i] = LOW;

	}
	switch (ratio) {
	case 1:
		max = 0;
		for (i = 0; i < LEVEL; i++) {
			n = hist[i];
			if (n > max) max = n;
		}
		for (i = 0; i < LEVEL; i++) {
			d = (float)hist[i];
			n = d / (float)max * (float)range;
			for (j = 0; j <= n; j++) image_hist[(range - j) ][ i] = HIGH;
		}
		for (i = 0; i <= 4; i++) {
			k = (HIST_X_SIZE / 4) * i;
			if (k >= HIST_X_SIZE) k = HIST_X_SIZE - 1;
			for (j = range; j < HIST_Y_SIZE; j++) image_hist[j][k] = HIGH;
		}
		break;
	case 2:
		max = 0;
		for (i = 0; i < LEVEL / 2; i++) {
			n = hist[2 * i] + hist[2 * i + 1];
			if (n > max) max = n;
		}
		for (i = 0; i < LEVEL / 2; i++) {
			d = (float)(hist[2 * i] + hist[2 * i + 1]);
			n = d / (float)max * (float)range;
			for (j = 0; j <= n; j++) image_hist[(range - j) ][ i] = HIGH;

		}
		for (i = 0; i <= 4; i++) {
			k = (HIST_X_SIZE / 4) * i;
			if (k >= HIST_X_SIZE) k = HIST_X_SIZE - 1;
			for (j = range; j < HIST_Y_SIZE; j++) image_hist[j][k] = HIGH;

		}
		break;
	default:
		break;

	}
}


void histsmooth(long hist_in[], long hist_out[]) {
	int m, n, i;
	long sum;

	for (n = 0; n < LEVEL; n++) {
		sum = 0;
		for (m = 0; m <= 2; m++) {
			i = n + m;
			if (i < 0) i = 0;
			if (i > LEVEL - 1) i = LEVEL - 1;
			sum = sum + hist_in[i];
		}
		hist_out[n] = (float)sum / 5.0 + 5.0;
	}
}


void threshold(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int thresh, int mode)
{
	int i, j;

	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			switch (mode) {
			case 1:
				if ((int)image_in[i][j] >= thresh) {
					image_out[i][j] = HIGH;
				}
				else {
					image_out[i][j] = LOW;
				}
				break;
			case 2:
				if ((int)image_in[i][j] >= thresh) {
					image_out[i][j] = LOW;
				}
				else {
					image_out[i][j] = HIGH;
				}
				break;
			default:
				break;
			}
		}
	}
}


void contraction(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]) {
	int i, j;
	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			image_out[i][j] = image_in[i][j];
			if (image_in[(i - 1)][(j - 1)] == LOW) image_out[i][j] = LOW;
			if (image_in[(i - 1)][j] == LOW) image_out[i][j] = LOW;
			if (image_in[(i - 1)][(j + 1)] == LOW) image_out[i][j] = LOW;
			if (image_in[i][(j - 1)] == LOW) image_out[i][j] = LOW;
			if (image_in[i][(j + 1)] == LOW) image_out[i][j] = LOW;
			if (image_in[(i + 1)][(j - 1)] == LOW) image_out[i][j] = LOW;
			if (image_in[(i + 1)][j] == LOW) image_out[i][j] = LOW;
			if (image_in[(i + 1)][(j + 1)] == LOW) image_out[i][j] = LOW;
		}
	}
}

void expansion(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]) {
	int i, j;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			image_out[i][j] = image_in[i][j];
			if (image_in[(i - 1)][(j - 1)] == HIGH) image_out[i][j] = HIGH;
			if (image_in[(i - 1)][j] == HIGH) image_out[i][j] = HIGH;
			if (image_in[(i - 1)][(j + 1)] == HIGH) image_out[i][j] = HIGH;
			if (image_in[i][(j - 1)] == HIGH) image_out[i][j] = HIGH;
			if (image_in[i][(j + 1)] == HIGH) image_out[i][j] = HIGH;
			if (image_in[(i + 1)][(j - 1)] == HIGH) image_out[i][j] = HIGH;
			if (image_in[(i + 1)][j] == HIGH) image_out[i][j] = HIGH;
			if (image_in[(i + 1)][(j + 1)] == HIGH) image_out[i][j] = HIGH;
		}
	}
}
//��������μ�����
void thinning(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]) {
	int ia[9] = { 0 };
	int ic[9] = { 0 };
	int i, ix, iy, m, ir, iv, iw;

	for (iy = 0; iy < Y_SIZE; iy++)
		for (ix = 0; ix < X_SIZE; ix++)
			image_out[iy][ix] = image_in[iy][ix];
	m = 100; ir = 1;
	while (ir != 0) {
		ir = 0;
		for (iy = 1; iy < Y_SIZE; iy++)
			for (ix = 1; ix < Y_SIZE; ix++) {
				if (image_out[iy][ix] != HIGH) {
					continue;
				}
				ia[0] = image_out[iy][ix + 1];
				ia[1] = image_out[(iy - 1)][ix + 1];
				ia[2] = image_out[(iy - 1)][ix];
				ia[3] = image_out[(iy - 1)][ix - 1];
				ia[4] = image_out[iy][ix - 1];
				ia[5] = image_out[(iy + 1)][ix - 1];
				ia[6] = image_out[(iy + 1)][ix];
				ia[7] = image_out[(iy + 1)][ix + 1];
				for (i = 0; i < 8; i++) {
					if (ia[i] == m) {
						ia[i] = HIGH; ic[i] = 0;
					}
					else {
						if (ia[i] < HIGH) ia[i] = 0;
						ic[i] = ia[i];
					}
				}
				ia[8] = ia[0];
				ic[8] = ic[0];
				if (ia[0] + ia[2] + ia[4] + ia[6] == HIGH * 4) {
					continue;
				}
				for (i = 0, iv = 0, iw = 0; i < 8; i++) {
					if (ia[i] == HIGH) iv++;
					if (ic[i] == HIGH) iw++;
				}
				if (iv <= 1) {
					continue;
				}
				if (iw <= 1) {
					continue;
				}
				if (cconc(ia) != 1) {
					continue;
				}
				if (image_out[(iy - 1)][ix] == m) {
					ia[2] = 0;
					if (cconc(ia) != 1) continue;
					ia[2] = HIGH;
				}
				if (image_out[iy][ix - 1] == m) {
					ia[4] = 0;
					if (cconc(ia) != 1) continue;
					ia[4] = HIGH;
				}
				image_out[iy][ix] = m;
				ir++;
			}
		m++;
	}

	for (iy = 0; iy < Y_SIZE; iy++) {

		for (ix = 0; ix < X_SIZE; ix++) {
			if (image_out[iy][ix] < HIGH) {
				image_out[iy][ix] = 0;
			}
		}
	}
}


int cconc(int inb[]) {
	int icn = 0;

	for (int i = 0; i < 8; i += 2)
		if (inb[i] == 0)
			if (inb[i + 1] == HIGH || inb[i + 2] == HIGH)
				icn++;
	return(icn);
}
int labeling(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_label[Y_SIZE][X_SIZE], int* cnt) {
	int i, j, label;
	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++)
			image_label[i][j] = image_in[i][j];
	label = L_BASE;
	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++) {
			if (image_label[i][j] == HIGH) {
				if (label >= HIGH) {
					printf("ERROR! to many labels. \n");
					return(-1);
				}
				labelset(image_label, j, i, label); label++;
			}
		}
	*cnt = label - L_BASE;
	printf(" no. of labels : %d \n", *cnt);
}
/*--- labelset --- �B�Y���Ƥ��뻭�ؤ��٤Ƥ˥�٥븶������ ---------------------
	image:    ��������
	xs, ys:    �����`��λ��
	label:    ��٥뷬��
-----------------------------------------------------------------------------*/
int labelset(unsigned char image[Y_SIZE][X_SIZE], int xs, int ys, int label) {
	int i, j, cnt;

	image[ys][xs] = label;
	for (;;) {
		cnt = 0;
		for (i = 1; i < Y_SIZE - 1; i++) {
			for (j = 1; j < X_SIZE - 1; j++) {
				if (image[i][j] == label) {
					if (image[i][j + 1] == HIGH) {
						image[i][j + 1] = label;
						cnt++;
					}
					if (image[i][j + 1] == HIGH) {
						image[i][j + 1] = label;
						cnt++;
					}
					if (image[(i - 1)][j] == HIGH) {
						image[(i - 1)][j] = label;
						cnt++;
					}
					if (image[(i - 1)][j - 1] == HIGH) {
						image[(i - 1)][j - 1] = label;
						cnt++;
					}
					if (image[i][j - 1] == HIGH) {
						image[i][j - 1] = label;
						cnt++;
					}
					if (image[(i + 1)][j - 1] == HIGH) {
						image[(i + 1)][j - 1] = label;
						cnt++;
					}
					if (image[(i + 1)][j] == HIGH) {
						image[(i + 1)][j] = label;
						cnt++;
					}
					if (image[(i + 1)][j + 1] == HIGH) {
						image[(i + 1)][j + 1] = label;
						cnt++;
					}
				}
			}
		}
		if (cnt == 0) {
			return(0);
		}
	}
}

void features(unsigned char image_label_in[Y_SIZE][X_SIZE], unsigned char image_label_out[Y_SIZE][X_SIZE], int cnt, float size[], float ratio[]) {
	int i, j, center_x, center_y;
	float l;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			image_label_out[i][j] = image_label_in[i][j];
		}
	}
	printf("no      ��e�����܇��L�������ζȡ�������(x,y)\n");
	for (i = 0; i < cnt; i++) {
		size[i] = calc_size(image_label_out, i + L_BASE, &center_x, &center_y);
		l = calc_length(image_label_out, i + L_BASE);
		ratio[i] = 4 * PI * size[i] / (l * l);
		image_label_out[center_y][center_x] = HIGH;
		printf("%d   %f   %f   %f   (%d,%d)\n", i, size[i], l, ratio[i], center_x, center_y);
	}
}
int calc_size(unsigned char image_label[Y_SIZE][X_SIZE], int label, int* cx, int* cy) {
	int i, j;
	float tx, ty, total;
	tx = 0; ty = 0; total = 0;
	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++)
			if (image_label[i][j] == label) {
				tx += j; ty += i; total++;
			}
	if (total == 0.0)return(0.0);
	*cx = tx / total; *cy = ty / total;

	return(total);
}
float calc_length(unsigned char image_label[Y_SIZE][X_SIZE], int label) {
	int i, j;

	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			if (image_label[i][j] == label) {
				return(trace(image_label, j - 1, i));
			}
		}
	}

}

float trace(unsigned char image_label[Y_SIZE][X_SIZE], int  xs, int ys) {
	int x, y, no, vec;
	float l;
	l = 0; x = xs; y = ys; no = image_label[y][x + 1]; vec = 5;

	for (;;) {
		if (x == xs && y == ys && l != 0) {
			return(l);
		}
		image_label[y][x] = HIGH;
		switch (vec) {
		case 3:
			if (image_label[y][x + 1] != no && image_label[(y - 1)][x + 1] == no)
			{
				x = x + 1; y = y; l++;
				vec = 0; continue;
			}
		case 4:
			if (image_label[(y - 1)][x + 1] != no && image_label[(y - 1)][x] == no)
			{
				x = x + 1; y = y - 1; l += ROOT2;
				vec = 1; continue;
			}
		case 5:
			if (image_label[(y - 1)][x] != no && image_label[(y - 1)][x - 1] == no)
			{
				x = x; y = y - 1; l++;
				vec = 2; continue;
			}
		case 6:
			if (image_label[(y - 1)][x - 1] != no && image_label[y][x - 1] == no)
			{
				x = x - 1; y = y - 1; l += ROOT2;
				vec = 3; continue;
			}
		case 7:
			if (image_label[y][x - 1] != no && image_label[(y + 1)][x - 1] == no)
			{
				x = x - 1; y = y; l++;
				vec = 4; continue;
			}
		case 0:
			if (image_label[(y + 1)][x - 1] != no && image_label[(y + 1)][x] == no)
			{
				x = x - 1; y = y + 1; l += ROOT2;
				vec = 5; continue;
			}
		case 1:
			if (image_label[(y + 1)][x] != no && image_label[(y + 1)][x + 1] == no)
			{
				x = x; y = y + 1; l++;
				vec = 6; continue;
			}
		case 2:
			if (image_label[(y + 1)][x + 1] != no && image_label[y][x + 1] == no)
			{
				x = x + 1; y = y + 1; l += ROOT2;
				vec = 7; continue;
			}
			vec = 3;
		}
	}
	return l;
}
void ratio_extract(unsigned char image_label_in[Y_SIZE][X_SIZE], unsigned char image_label_out[Y_SIZE][X_SIZE], int cnt, float ratio[], float ratio_min, float ratio_max) {
	int i, j, x, y;
	int lno[LEVEL];

	for (i = 0, j = 0; i < cnt; i++)
		if (ratio[i] >= ratio_min && ratio[i] <= ratio_max) {
			lno[j++] = L_BASE + i;
		}
	for (y = 0; y < Y_SIZE; y++) {
		for (x = 0; x < X_SIZE; x++) {
			image_label_out[y][x] = 0;
			for (i = 0; i < j; i++) {
				if (image_label_in[y][x] == lno[i]) {
					image_label_out[y][x] = image_label_in[y][x];
				}
			}
		}
	}
}
void size_extract(unsigned char image_label_in[Y_SIZE][X_SIZE], unsigned char image_label_out[Y_SIZE][X_SIZE], int cnt, float size[], float size_min, float size_max) {
	int i, j, x, y;
	int lno[LEVEL];

	for (i = 0, j = 0; i < cnt; i++) {
		if (size[i] >= size_min && size[i] <= size_max) {
			lno[j++] = L_BASE + i;
		}
	}
	for (y = 0; y < Y_SIZE; y++) {
		for (x = 0; x < X_SIZE; x++) {
			image_label_out[y][x] = 0;
			for (i = 0; i < j; i++) {
				if (image_label_in[y][x] == lno[i]) {
					image_label_out[y][x] = image_label_in[y][x];
				}
			}
		}
	}
}
void gradient(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp1) {
	static int cx[9] = { 0, 0, 0,
						 0, 1, 0,
						 0, 0, -1 };
	static int cy[9] = { 0, 0, 0,
						 0, 0, 1,
						 0,-1, 0 };
	int d[9];
	int i, j, dat;
	float xx, yy, zz;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d[0] = image_in[(i - 1)][j - 1];
			d[1] = image_in[(i - 1)][j];
			d[2] = image_in[(i - 1)][j + 1];
			d[3] = image_in[i][j - 1];
			d[4] = image_in[i][j];
			d[5] = image_in[i][j + 1];
			d[6] = image_in[(i + 1)][j - 1];
			d[7] = image_in[(i + 1)][j];
			d[8] = image_in[(i + 1)][j + 1];
			xx = cx[0] * d[0] + cx[1] * d[1] + cx[2] * d[2] + cx[3] * d[3] + cx[4] * d[4] + cx[5] * d[5] + cx[6] * d[6] + cx[7] * d[7] + cx[8] * d[8];
			yy = cy[0] * d[0] + cy[1] * d[1] + cy[2] * d[2] + cy[3] * d[3] + cy[4] * d[4] + cy[5] * d[5] + cy[6] * d[6] + cy[7] * d[7] + cy[8] * d[8];
			zz = amp1 * (float)sqrt((float)(xx * xx + yy * yy));
			dat = (int)(zz);

			if (dat > 255) {
				dat = 255;
			}
			image_out[i][j] = (char)(dat);
		}
	}
}
void laplacian(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp1) {
	static int c[9] = { -1,-1,-1,-1,8,-1,-1,-1,-1 };
	int d[9];
	int i, j, dat;
	float z, zz;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d[0] = image_in[(i - 1)][j - 1];
			d[1] = image_in[(i - 1)][j];
			d[2] = image_in[(i - 1)][j + 1];
			d[3] = image_in[i][j - 1];
			d[4] = image_in[i][j];
			d[5] = image_in[i][j + 1];
			d[6] = image_in[(i + 1)][j - 1];
			d[7] = image_in[(i + 1)][j];
			d[8] = image_in[(i + 1)][j + 1];
			z = c[0] * d[0] + c[1] * d[1] + c[2] * d[2] + c[3] * d[3] + c[4] * d[4] + c[5] * d[5] + c[6] * d[6] + c[7] * d[7] + c[8] * d[8];
			zz = amp1 * z;
			dat = (int)(zz);
			if (dat < 0) {
				dat = -dat;
			}
			if (dat > 255) {
				dat = 255;
			}
			image_out[i][j] = (char)(dat);
		}
	}
}
void template1(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp1) {
	int d0, d1, d2, d3, d4, d5, d6, d7, d8;
	int i, j, k, dat, max;
	int m[8];
	float zz;
	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d0 = image_in[(i - 1)][j - 1];
			d1 = image_in[(i - 1)][j];
			d2 = image_in[(i - 1)][j + 1];
			d3 = image_in[i][j - 1];
			d4 = image_in[i][j];
			d5 = image_in[i][j + 1];
			d6 = image_in[(i + 1)][j - 1];
			d7 = image_in[(i + 1)][j];
			d8 = image_in[(i + 1)][j + 1];

			m[0] = d0 + d1 + d2 + d3 - 2 * d4 + d5 - d6 - d7 - d8;
			m[1] = d0 + d1 + d2 + d3 - 2 * d4 - d5 + d6 - d7 - d8;
			m[2] = d0 + d1 - d2 + d3 - 2 * d4 - d5 + d6 + d7 - d8;
			m[3] = d0 - d1 - d2 + d3 - 2 * d4 - d5 + d6 + d7 + d8;
			m[4] = -d0 - d1 - d2 + d3 - 2 * d4 + d5 + d6 + d7 + d8;
			m[5] = -d0 - d1 + d2 - d3 - 2 * d4 + d5 + d6 + d7 + d8;
			m[6] = -d0 + d1 + d2 - d3 - 2 * d4 + d5 - d6 + d7 + d8;
			m[7] = d0 + d1 + d2 - d3 - 2 * d4 + d5 - d6 - d7 + d8;
			max = 0;
			for (k = 0; k < 8; k++) {
				if (max < m[k]) {
					max = m[k];
				}
			}
			zz = amp1 * (float)(max);
			dat = (int)(zz);
			if (dat > 255) {
				dat = 255;
			}
			image_out[i][j] = (char)(dat);
		}
	}
}

void smooth(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int which) {
	int i, j;
	unsigned char c[9];

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			c[0] = image_in[(i - 1)][j - 1];
			c[1] = image_in[(i - 1)][j];
			c[2] = image_in[(i - 1)][j + 1];
			c[3] = image_in[i][j - 1];
			c[4] = image_in[i][j];
			c[5] = image_in[i][j + 1];
			c[6] = image_in[(i + 1)][j - 1];
			c[7] = image_in[(i + 1)][j];
			c[8] = image_in[(i + 1)][j + 1];
			switch (which) {
			case 1:
				image_out[i][j] = mean(c);
				break;
			case 2:
				image_out[i][j] = median(c);
				break;
			}
		}
	}
}
int mean(unsigned char c[]) {
	int i, buf;
	buf = 0;
	for (i = 0; i < 9; i++)
		buf += (int)c[i];
	return(buf / 9);
}
int median(unsigned char c[]) {
	int i, j, buf;

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			if (c[i + 1] < c[i]) {
				buf = c[i + 1];
				c[i + 1] = c[i];
				c[i] = buf;
			}
		}
	}
	return(c[4]);
}

void amplify(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int n) {
	int i, j, nf;
	for (i = 0; i < Y_SIZE; i++) {
		{
			for (j = 0; j < X_SIZE; j++) {
				nf = (int)image_in[i][j] * n;
				if (nf > HIGH)nf = HIGH;
				image_out[i][j] = (unsigned char)nf;
			}
		}
	}
}
void range(unsigned char image_in[Y_SIZE][X_SIZE], int* fmax, int* fmin) {
	int i, j, nf;
	*fmax = LOW;
	*fmin = HIGH;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			nf = (int)image_in[i][j];
			if (nf > *fmax) *fmax = nf;
			if (nf < *fmin)*fmin = nf;
		}
	}
}

void enpand(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int fmax, int  fmin) {
	int i, j;
	float d;

	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			d = (float)(HIGH - LOW) / (float)(fmax - fmin)
				* ((int)image_in[i][j] - fmin) + LOW;
			if (d > HIGH) image_out[i][j] = HIGH;
			else if (d < LOW) image_out[i][j] = LOW;
			else image_out[i][j] = d;
		}
	}
}
void plane(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int hist[]) {
	int i, j, iy, jx, sum;
	int delt;
	int low;
	int high;
	int av;

	av = (int)((Y_SIZE) * (X_SIZE) / LEVEL);
	high = HIGH;
	low = HIGH;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			image_out[i][j] = 0;
		}
	}

	for (i = HIGH; i > 0; i--) {
		for (sum = 0; sum < av; low--) sum = sum + hist[low];
		low++;
		delt = hist[low] - (sum - av);
		sort(image_in, buf, low);
		if (low < high) {
			for (iy = 0; iy < Y_SIZE; iy++) {
				for (jx = 0; jx < X_SIZE; jx++) {
					if (((int)image_in[iy][jx] >= low + 1) & ((int)image_in[iy][jx] <= high)) {
						image_out[iy][jx] = (unsigned char)i;
					}
				}
			}
		}
		for (j = 0; j < delt; j++) {
			image_out[buf[j].y][buf[j].x] = (unsigned char)i;
			image_in[buf[j].y][buf[j].x] = (unsigned char)(low + 1);
		}


		hist[low] = hist[low] - delt;
		high = low;
		
	}
}

void histsmooth2(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int hist[])
{
	unsigned char min = 255;
	unsigned char max = 0;
	unsigned char image_buff;
	float temp;

	for (int i = 0; i < LEVEL; i++) {
		if (hist[i] < min) {
			min = hist[i];
		}
		if (hist[i] > max) {
			max = hist[i];
		}
	}
	temp = 255.0 / (float)(max - min);
	for (int i = 0; i < Y_SIZE; i++) {
		for (int j = 0; j < X_SIZE; j++) {
			if (image_in[i][j] <= min) {
				image_out[i][j] = 0;
			}
			else if (image_in[i][j] >= max) {
				image_out[i][j] = 255;
			}
			else {
				image_buff = (float)((image_in[i][j] - min) * temp);
				if (image_buff > 255) {
					image_buff = 255;
				}
				image_out[i][j] = (unsigned char)(image_buff);
			}
		}
	}
}
void sort(unsigned char image_in[Y_SIZE][X_SIZE], struct xyw data[], int level) {
	int i, j, inum, wt;
	struct xyw temp;
	inum = 0;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			if ((int)image_in[i][j] == level) {
				weight(image_in, i, j, &wt);
				data[inum].y = i;
				data[inum].x = j;
				data[inum].w = wt;
			}
		}
	}
	for (i = 0; i < inum - 1; i++) {
		for (j = 1 + 1; j < inum; j++) {
			if (data[i].w <= data[j].w) {
				temp.y = data[i].y;
				temp.x = data[i].x;
				temp.w = data[i].w;
				data[i].y = data[j].y;
				data[i].x = data[j].x;
				data[i].w = data[j].w;
				data[j].y = temp.y;
				data[j].x = temp.x;
				data[j].w = temp.w;
			}
		}
	}
}
void weight(unsigned char image_in[Y_SIZE][X_SIZE], int i, int j, int* wt) {
	int dim, djm;
	int dip, djp;
	int k, d[8];

	dim = i - 1;
	djm = j - 1;
	dip = i + 1;
	djp = j + 1;
	if (dim < 0) {
		dim = i;
	}
	if (djm < 0) {
		djm = j;
	}
	if (dip > Y_SIZE - 1) {
		dip = i;
	}
	if (djp > X_SIZE - 1) {
		djp = j;
	}

	d[0] = (int)image_in[dim][djm];
	d[1] = (int)image_in[dim][j];
	d[2] = (int)image_in[dim][djp];
	d[3] = (int)image_in[i][djm];
	d[4] = (int)image_in[i][djp];
	d[5] = (int)image_in[dip][djm];
	d[6] = (int)image_in[dip][j];
	d[7] = (int)image_in[dip][djp];
	for (k = 0; k < 8; k++) {
		*wt = *wt + d[i];
	}
}
void scale_near(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float zx, float zy) {
	int i, j, m, n;
	//float x, y, p, q;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;

	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			if (i > 0) m = (int)(i / zy + 0.5);
			else m = (int)(i / zy - 0.5);
			if (j > 0) n = (int)(j / zx + 0.5);
			else n = (int)(j / zx - 0.5);
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs))
				image_out[i + ys][j + xs] = image_in[m + ys][n + xs];
			else
				image_out[i + ys][j + xs] = 0;
		}
	}
}
void scale(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float zx, float zy) {
	int i, j, m, n;
	float x, y, p, q;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;
	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			y = i / zy;
			x = j / zx;
			if (y > 0) {
				m = y;
			}
			else {
				m = y - 1;
			}
			if (x > 0) {
				n = x;
			}
			else {
				n = x - 1;
			}
			q = y - m;
			p = x - n;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {
				d = (1.0 - q) * ((1.0 - p) * image_in[(m + ys)][n + xs] + p * image_in[(m + ys)][n + 1 + xs]) + q * ((1.0 - p) * image_in[(m + 1 + ys)][n + xs] + p * image_in[(m + 1 + ys)][n + 1 + xs]);
			}
			else {
				d = 0;
			}
			if (d < 0) {
				d = 0;
			}
			if (d > 255) {
				d = 255;
			}
			image_out[(i + ys)][j + xs] = d;
		}
	}
}
void rotation(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float deg) {
	int i, j, m, n;
	float x, y, p, q;
	double r;
	float c, s;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;

	r = deg * 3.141592 / 180.0;
	c = cos(r);
	s = sin(r);
	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			y = j * s + i * c;
			x = j * c - i * s;
			if (y > 0) {
				m = y;
			}
			else {
				m = y - 1;
			}
			if (x > 0) {
				n = x;
			}
			else {
				n = x - 1;
			}
			q = y - m;
			p = x - n;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {
				d = (1.0 - q) * ((1.0 - p) * image_in[(m + ys)][n + xs] + p * image_in[(m + ys)][n + xs]) + q * ((1.0 - p) * image_in[(m + 1 + ys)][n + xs] + p * image_in[(m + 1 + ys)][n + 1 + xs]);
			}
			else {
				d = 0;
			}
			if (d < 0) {
				d = 0;
			}
			if (d > 255) {
				d = 255;
			}
			image_out[(i + ys)][j + xs] = d;
		}
	}
}
void shift(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float px, float py) {
	int i = 0;
	int	j = 0;
	int	m, n;
	float x, y, p, q;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d = 0;

	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			y = i - py;
			x = j - px;
			if (y > 0) m = y; else m = y - 1;
			if (x > 0)n = x; else n = x - 1;
			q = y - m;
			p = x - n;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {
				d = (1.0 - q) * ((1.0 - p) * image_in[(m + ys)][n + xs] + p * image_in[(m + ys)][n + 1 + xs]) + q * ((1.0 - p) * image_in[(m + 1 + ys)][n + xs] + p * image_in[(m + 1 + ys)][n + 1 + xs]);
			}
			else {
				d = 0;
			}
			if (d < 0)
			{
				d = 0;
			}
			if (d > 255) {
				d = 255;
			}
			image_out[(i + ys)][j + xs] = d;
		}
	}
}
//#define PI    3.141592

/*--- affine --- ����sС,��ܞ,�Ƅӣ������a�g����------------------------------
	image_in:    ������������
	image_out:    ������������
	deg:        ��ܞ�ǣ��ȣ�
	zx:            �����ʣ��ᣩ
	zy:            �����ʣ��k��
	px:            �Ƅ������ᣩ
	py:            �Ƅ������k��
-----------------------------------------------------------------------------*/
void affine(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float deg, float zx, float zy, float px, float py) {
	int i, j, m, n;
	float x, y, u, v, p, q;
	float r;
	float c, s;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;
	r = deg * 3.141592 / 180.0;
	c = cos(r);
	s = sin(r);
	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			v = i - py;
			u = j - px;
			y = (u * s + v * c) / zy;
			x = (u * c - v * s) / zx;
			if (y > 0) {
				m = y;
			}
			else {
				m = y - 1;
			}

			if (x > 0) {
				n = x;
			}
			else {
				n = x - 1;
			}
			q = y - m;
			p = x - n;

			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {

				d = (1.0 - q) * ((1.0 - p) * image_in[(m + ys)][n + xs] + p * image_in[(m + ys)][n + 1 + xs]) + q * ((1.0 - p) * image_in[(m + 1 + ys)][n + xs] + p * image_in[(m + 1 + ys)][n + 1 + xs]);
			}
			else {
				d = 0;
				if (d < 0)d = 0;
				if (d > 255)d = 255;
			}
			image_out[(i + ys)][j + xs] = d;
		}
	}
}
/*--- perspect --- ͸ҕ��Q�������a�g����--------------------------------------
	image_in:    ������������
	image_out:    ������������
	ax:            �����ʣ��ᣩ
	ay:            �����ʣ��k��
	px:            �Ƅ���������
	py:            �Ƅ���������
	pz:            �Ƅ���������
	rz:            ��ܞ�ǣ����ȣ�
	rx:            ��ܞ�ǣ����ȣ�
	ry:            ��ܞ�ǣ����ȣ�
	v:            ҕ���λ�ã�����
	s:            ������`���λ�ã�����
-----------------------------------------------------------------------------*/
void perspect(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float ax, float ay, float px, float py, float pz, float rz, float rx, float ry, float v, float s) {
	int i, j, m, n;
	float x, y, w, p, q;
	float k[9];
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;

	param_pers(k, ax, ay, px, py, pz, rz, rx, ry, v, s);
	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			w = k[0] * j + k[1] * i + k[2];
			x = k[3] * j + k[4] * i + k[5];
			y = k[6] * j + k[7] * i + k[8];
			x = x / w;
			y = y / w;
			if (y > 0) {
				m = y;
			}
			else {
				m = y - 1;
			}
			if (x > 0) {
				n = x;
			}
			else {
				n = x - 1;
			}

			q = y - m;
			p = x - n;

			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs))
				d = (int)((1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs]
					+ p * image_in[m + ys][n + 1 + xs])
					+ q * ((1.0 - p) * image_in[m + 1 + ys][n + xs]
						+ p * image_in[m + 1 + ys][n + 1 + xs]));
			else
				d = 0;
			if (d < 0) d = 0;
			if (d > 255) d = 255;
			image_out[i + ys][j + xs] = d;
		}
	}
}
void param_pers(float k[], float a, float b, float x0, float y0, float z0, float z, float x, float y, float t, float s) {
	float l[4][4], m[4][4], n[4][4], k1, k2, k3, k4, k5, k6, k7, k8, k9;
	double u, v, w;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;

	u = x * 3.141592 / 180.0;    v = y * 3.141592 / 180.0;    w = z * 3.141592 / 180.0;
	l[0][0] = 1.0 / xs;  l[0][1] = 0;       l[0][2] = 0;        l[0][3] = 0;
	l[1][0] = 0;       l[1][1] = -1.0 / xs; l[1][2] = 0;        l[1][3] = 0;
	l[2][0] = 0;       l[2][1] = 0;       l[2][2] = 1;        l[2][3] = 0;
	l[3][0] = 0;       l[3][1] = 0;       l[3][2] = 0;        l[3][3] = 1;
	m[0][0] = a;       m[0][1] = 0;       m[0][2] = 0;        m[0][3] = 0;
	m[1][0] = 0;       m[1][1] = b;       m[1][2] = 0;        m[1][3] = 0;
	m[2][0] = 0;       m[2][1] = 0;       m[2][2] = 1;        m[2][3] = 0;
	m[3][0] = 0;       m[3][1] = 0;       m[3][2] = 0;        m[3][3] = 1;
	matrix(l, m, n);    /* ��Ҏ���ޥȥ�å��� �� ����sС�ޥȥ�å��� */
	l[0][0] = 1;       l[0][1] = 0;       l[0][2] = 0;        l[0][3] = 0;
	l[1][0] = 0;       l[1][1] = 1;       l[1][2] = 0;        l[1][3] = 0;
	l[2][0] = 0;       l[2][1] = 0;       l[2][2] = 1;        l[2][3] = 0;
	l[3][0] = x0;      l[3][1] = y0;      l[3][2] = z0;       l[3][3] = 1;
	matrix(n, l, m);    /* �� �Ƅӥޥȥ�å��� */
	n[0][0] = cos(w); n[0][1] = sin(w);  n[0][2] = 0;        n[0][3] = 0;
	n[1][0] = -sin(w); n[1][1] = cos(w);  n[1][2] = 0;        n[1][3] = 0;
	n[2][0] = 0;       n[2][1] = 0;       n[2][2] = 1;        n[2][3] = 0;
	n[3][0] = 0;       n[3][1] = 0;       n[3][2] = 0;        n[3][3] = 1;
	matrix(m, n, l);    /* �� ���S�λ�ܞ�ޥȥ�å��� */
	m[0][0] = 1;       m[0][1] = 0;       m[0][2] = 0;       m[0][3] = 0;
	m[1][0] = 0;       m[1][1] = cos(u);  m[1][2] = sin(u);  m[1][3] = 0;
	m[2][0] = 0;       m[2][1] = -sin(u); m[2][2] = cos(u);  m[2][3] = 0;
	m[3][0] = 0;       m[3][1] = 0;       m[3][2] = 0;       m[3][3] = 1;
	matrix(l, m, n);    /* �� ���S�λ�ܞ�ޥȥ�å��� */
	l[0][0] = cos(v);  l[0][1] = 0;       l[0][2] = sin(v);  l[0][3] = 0;
	l[1][0] = 0;       l[1][1] = 1;       l[1][2] = 0;       l[1][3] = 0;
	l[2][0] = -sin(v); l[2][1] = 0;       l[2][2] = cos(v);  l[2][3] = 0;
	l[3][0] = 0;       l[3][1] = 0;       l[3][2] = 0;       l[3][3] = 1;
	matrix(n, l, m);    /* �� ���S�λ�ܞ�ޥȥ�å��� */
	n[0][0] = 1;       n[0][1] = 0;       n[0][2] = 0;       n[0][3] = 0;
	n[1][0] = 0;       n[1][1] = 1;       n[1][2] = 0;       n[1][3] = 0;
	n[2][0] = 0;       n[2][1] = 0;       n[2][2] = -1;      n[2][3] = 0;
	n[3][0] = 0;       n[3][1] = 0;       n[3][2] = t;       n[3][3] = 1;
	matrix(m, n, l);    /* �� ҕ�����ˉ�Q�ޥȥ�å��� */
	m[0][0] = 1;       m[0][1] = 0;       m[0][2] = 0;       m[0][3] = 0;
	m[1][0] = 0;       m[1][1] = 1;       m[1][2] = 0;       m[1][3] = 0;
	m[2][0] = 0;       m[2][1] = 0;       m[2][2] = 1 / s;     m[2][3] = 1 / s;
	m[3][0] = 0;       m[3][1] = 0;       m[3][2] = -1;      m[3][3] = 0;
	matrix(l, m, n);    /* �� ͸ҕ��Q�ޥȥ�å��� */
	l[0][0] = xs;      l[0][1] = 0;       l[0][2] = 0;       l[0][3] = 0;
	l[1][0] = 0;       l[1][1] = -xs;     l[1][2] = 0;       l[1][3] = 0;
	l[2][0] = 0;       l[2][1] = 0;       l[2][2] = 1;       l[2][3] = 0;
	l[3][0] = 0;       l[3][1] = 0;       l[3][2] = 0;       l[3][3] = 1;
	matrix(n, l, m);    /* �� ��Ҏ����ޥȥ�å��� */
	k1 = (float)(m[0][3]);    k2 = (float)(m[1][3]);    k3 = (float)(m[3][3]);
	k4 = (float)(m[0][0]);    k5 = (float)(m[1][0]);    k6 = (float)(m[3][0]);
	k7 = (float)(m[0][1]);    k8 = (float)(m[1][1]);    k9 = (float)(m[3][1]);
	k[0] = k7 * k2 - k8 * k1; k[1] = k5 * k1 - k4 * k2; k[2] = k4 * k8 - k7 * k5;
	k[3] = k8 * k3 - k9 * k2; k[6] = k9 * k1 - k7 * k3; k[4] = k6 * k2 - k5 * k3;
	k[7] = k4 * k3 - k6 * k1; k[5] = k5 * k9 - k8 * k6; k[8] = k7 * k6 - k4 * k9;	/*
	printf("r= %f\n", k[2]);
	printf("a= %f\n", k[3]);
	printf("b= %f\n", k[4]);
	printf("c= %f\n", k[5]);
	printf("d= %f\n", k[6]);
	printf("e= %f\n", k[7]);
	printf("f= %f\n", k[8]);
	*/
}
void matrix(float l[4][4], float m[4][4], float n[4][4])
{
	int i, j, k;
	float p;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			p = 0;
			for (k = 0; k < 4; k++) p = p + l[i][k] * m[k][j];
			n[i][j] = p;
		}
	}
}
void  masking(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], unsigned char image_mask[Y_SIZE][X_SIZE])
{
	int i, j;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			if (image_mask[i][j] == HIGH) {
				image_out[i][j] = image_in[i][j];
			}
			else {
				image_out[i][j] = 0;
			}
		}
	}
}
/*--- fft1 --- ����Ԫ�ƣƣԤΌg�� ---------------------------------------------
	a_rl:    �ǩ`���g��������������ã�
	a_im:    �ǩ`������������������ã�
	ex:        �ǩ`�������򣲤Τ٤��\���뤨��(�ǩ`������ = 2��ex�\����
	inv:    1: �ģƣԣ�-1: ��ģƣ�
-----------------------------------------------------------------------------*/
int fft1(float a_rl[], float a_im[], int ex, int inv)
{
	int        i, length = 1;
	float* sin_tbl;    /* SINӋ���åƩ`�֥롡*/
	float* cos_tbl;    /* COSӋ���åƩ`�֥롡*/
	float* buf;        /* ���I�åХåե���*/

	for (i = 0; i < ex; i++) length *= 2;        /* �ǩ`��������Ӌ�� */
	sin_tbl = (float*)malloc((size_t)length * sizeof(float));
	cos_tbl = (float*)malloc((size_t)length * sizeof(float));
	buf = (float*)malloc((size_t)length * sizeof(float));
	if ((sin_tbl == NULL) || (cos_tbl == NULL) || (buf == NULL)) {
		return -1;
	}
	cstb(length, inv, sin_tbl, cos_tbl);    /* SIN,COS�Ʃ`�֥����� */
	fft1core(a_rl, a_im, length, ex, sin_tbl, cos_tbl, buf);
	free(sin_tbl);
	free(cos_tbl);
	return 0;
}

/*--- fft1core --- ����Ԫ�ƣƣԤ�Ӌ��κˤˤʤ벿�� ---------------------------
	a_rl:    �ǩ`���g��������������ã�
	a_im:    �ǩ`������������������ã�
	ex:        �ǩ`�������򣲤Τ٤��\���뤨��(�ǩ`������ = 2��ex�\����
	sin_tbl:    SINӋ���åƩ`�֥�
	cos_tbl:    COSӋ���åƩ`�֥�
-----------------------------------------------------------------------------*/
void fft1core(float a_rl[], float a_im[], int length,
	int ex, float sin_tbl[], float cos_tbl[], float buf[])
{
	int        i, j, k, w, j1, j2;
	int        numb, lenb, timb;
	float   xr, xi, yr, yi, nrml;

	if (OPT == 1) {
		for (i = 1; i < length; i += 2) {
			a_rl[i] = -a_rl[i];
			a_im[i] = -a_im[i];
		}
	}
	numb = 1;
	lenb = length;
	for (i = 0; i < ex; i++) {
		lenb /= 2;
		timb = 0;
		for (j = 0; j < numb; j++) {
			w = 0;
			for (k = 0; k < lenb; k++) {
				j1 = timb + k;
				j2 = j1 + lenb;
				xr = a_rl[j1];
				xi = a_im[j1];
				yr = a_rl[j2];
				yi = a_im[j2];
				a_rl[j1] = xr + yr;
				a_im[j1] = xi + yi;
				xr = xr - yr;
				xi = xi - yi;
				a_rl[j2] = xr * cos_tbl[w] - xi * sin_tbl[w];
				a_im[j2] = xr * sin_tbl[w] + xi * cos_tbl[w];
				w += numb;
			}
			timb += (2 * lenb);
		}
		numb *= 2;
	}
	birv(a_rl, length, ex, buf);        /*���g���ǩ`���΁K�ٓQ����*/
	birv(a_im, length, ex, buf);        /*�������ǩ`���΁K�ٓQ����*/
	if (OPT == 1) {
		for (i = 1; i < length; i += 2) {
			a_rl[i] = -a_rl[i];
			a_im[i] = -a_im[i];
		}
	}
	nrml = (float)(1.0 / sqrt((float)length));
	for (i = 0; i < length; i++) {
		a_rl[i] *= nrml;
		a_im[i] *= nrml;
	}
}

/*--- cstb --- SIN,COS�Ʃ`�֥����� --------------------------------------------
	length:        �ǩ`������
	inv:        1: �ģƣ�, -1: ��ģƣ�
	sin_tbl:    SINӋ���åƩ`�֥�
	cos_tbl:    COSӋ���åƩ`�֥�
-----------------------------------------------------------------------------*/
void cstb(int length, int inv, float sin_tbl[], float cos_tbl[])
{
	int        i;
	float      xx, arg;

	xx = (float)(((-PI) * 2.0) / (float)length);
	if (inv < 0) xx = -xx;
	for (i = 0; i < length; i++) {
		arg = (float)i * xx;
		sin_tbl[i] = (float)sin(arg);
		cos_tbl[i] = (float)cos(arg);
	}
}

/*--- birv --- �ǩ`���΁K�ٓQ�� -----------------------------------------------
	a:        �ǩ`��������
	length:    �ǩ`������
	ex:        �ǩ`�������򣲤Τ٤��\���뤨��(length = 2��ex�\����
	b:        ���I�åХåե�
-----------------------------------------------------------------------------*/
void birv(float a[], int length, int ex, float b[])
{
	int    i, ii, k, bit;

	for (i = 0; i < length; i++) {
		for (k = 0, ii = i, bit = 0; ; bit <<= 1, ii >>= 1) {
			bit = (ii & 1) | bit;
			if (++k == ex) break;
		}
		b[i] = a[bit];
	}
	for (i = 0; i < length; i++)
		a[i] = b[i];
}
/*--- fft2 --- ����Ԫ�ƣƣԤΌg�� ---------------------------------------------
		��X_SIZE��Y_SIZE�����Τ٤��\�Έ��Ϥ��ޤ룩
	a_rl:    �ǩ`���g��������������ã�
	a_im:    �ǩ`������������������ã�
	inv:    1: �ģƣԣ�-1: ��ģƣ�
-----------------------------------------------------------------------------*/
int fft2(float a_rl[Y_SIZE][X_SIZE], float a_im[Y_SIZE][X_SIZE], int inv)
{
	float* b_rl;        /* �ǩ`��ܞ�����I�����У��g������*/
	float* b_im;        /* �ǩ`��ܞ�����I�����У���������*/
	float* hsin_tbl;    /* ˮƽ��SINӋ���åƩ`�֥�        */
	float* hcos_tbl;    /* ˮƽ��COSӋ���åƩ`�֥�        */
	float* vsin_tbl;    /* ��ֱ��SINӋ���åƩ`�֥�        */
	float* vcos_tbl;    /* ��ֱ��COSӋ���åƩ`�֥�        */
	float* buf_x;        /* ���I�åХåե���ˮƽ����    */
	float* buf_y;        /* ���I�åХåե�����ֱ����    */
	int        i;

	b_rl = (float*)calloc((size_t)X_SIZE * Y_SIZE, sizeof(float));
	b_im = (float*)calloc((size_t)X_SIZE * Y_SIZE, sizeof(float));
	hsin_tbl = (float*)calloc((size_t)X_SIZE, sizeof(float));
	hcos_tbl = (float*)calloc((size_t)X_SIZE, sizeof(float));
	vsin_tbl = (float*)calloc((size_t)Y_SIZE, sizeof(float));
	vcos_tbl = (float*)calloc((size_t)Y_SIZE, sizeof(float));
	buf_x = (float*)malloc((size_t)X_SIZE * sizeof(float));
	buf_y = (float*)malloc((size_t)Y_SIZE * sizeof(float));
	if ((b_rl == NULL) || (b_im == NULL)
		|| (hsin_tbl == NULL) || (hcos_tbl == NULL)
		|| (vsin_tbl == NULL) || (vcos_tbl == NULL)
		|| (buf_x == NULL) || (buf_y == NULL)) {
		return -1;
	}
	cstb(X_SIZE, inv, hsin_tbl, hcos_tbl);    /* ˮƽ��SIN,COS�Ʃ`�֥�����    */
	cstb(Y_SIZE, inv, vsin_tbl, vcos_tbl);    /* ��ֱ��SIN,COS�Ʃ`�֥�����    */
	/* ˮƽ����Σƣƣ� */
	for (i = 0; i < Y_SIZE; i++) {
		fft1core(&a_rl[(long)i][0], &a_im[(long)i][0],
			X_SIZE, X_EXP, hsin_tbl, hcos_tbl, buf_x);
	}
	/* ����Ԫ�ǩ`����ܞ�� */
	rvmtx1((float(*)[X_SIZE])a_rl, (float(*)[X_SIZE])b_rl, X_SIZE, Y_SIZE);
	rvmtx1((float(*)[X_SIZE])a_im, (float(*)[X_SIZE])b_im, X_SIZE, Y_SIZE);
	/* ��ֱ����Σƣƣ� */
	for (i = 0; i < X_SIZE; i++) {
		fft1core(&b_rl[(long)Y_SIZE * i], &b_im[(long)Y_SIZE * i],
			Y_SIZE, Y_EXP, vsin_tbl, vcos_tbl, buf_y);
	}
	/* ����Ԫ�ǩ`����ܞ�� */
	rvmtx2((float(*)[Y_SIZE])b_rl, (float(*)[Y_SIZE])a_rl, X_SIZE, Y_SIZE);
	rvmtx2((float(*)[Y_SIZE])b_im, (float(*)[Y_SIZE])a_im, X_SIZE, Y_SIZE);
	free(b_rl);
	free(b_im);
	free(hsin_tbl);
	free(hcos_tbl);
	free(vsin_tbl);
	free(vcos_tbl);
	return 0;
}

/*--- rvmtx1 --- ����Ԫ�ǩ`����ܞ�� -------------------------------------------
	a:        ����Ԫ�����ǩ`��
	b:        ����Ԫ�����ǩ`��
	xsize:    ˮƽ�ǩ`������
	ysize:    ��ֱ�ǩ`������
-----------------------------------------------------------------------------*/
void rvmtx1(float a[Y_SIZE][X_SIZE], float b[X_SIZE][Y_SIZE],
	int xsize, int ysize)
{
	int  i, j;

	for (i = 0; i < ysize; i++) {
		for (j = 0; j < xsize; j++)
			b[j][i] = a[i][j];
	}
}

/*--- rvmtx2 --- ����Ԫ�ǩ`����ܞ�� -------------------------------------------
	a:        ����Ԫ�����ǩ`��
	b:        ����Ԫ�����ǩ`��
	xsize:    ˮƽ�ǩ`������
	ysize:    ��ֱ�ǩ`������
-----------------------------------------------------------------------------*/
void rvmtx2(float a[X_SIZE][Y_SIZE], float b[Y_SIZE][X_SIZE],
	int xsize, int ysize)
{
	int i, j;

	for (i = 0; i < ysize; i++) {
		for (j = 0; j < xsize; j++)
			b[i][j] = a[j][i];
	}
}
/*--- fftfilter --- �ƣƣԤˤ�뻭��Υե��륿�I�� ----------------------------
		��X_SIZE��Y_SIZE�����Τ٤��\�Έ��Ϥ��ޤ룩
	image_in:    ������������
	image_out:    ������������
	deg:        ��ܞ�ǣ��ȣ�
	a, b:        ͨ�^����a���ϣ�b���¤��ܲ����ɷ֤�ͨ�^���룩
				a=0��b=X_SIZE=Y_SIZE�ΤȤ���ȫ�����ͨ�^
-----------------------------------------------------------------------------*/
int fftimage(unsigned char image_in[Y_SIZE][X_SIZE],
	unsigned char image_out[Y_SIZE][X_SIZE])
{
	float* ar;        /* �ǩ`���g��������������ã�*/
	float* ai;        /* �ǩ`������������������ã�*/
	double     norm, max;
	float    data;
	long    i, j;

	ar = (float*)malloc((size_t)Y_SIZE * X_SIZE * sizeof(float));
	ai = (float*)malloc((size_t)Y_SIZE * X_SIZE * sizeof(float));
	if ((ar == NULL) || (ai == NULL)) return -1;
	/* ԭ������i���z�ߣ�����Ԫ�ƣƣԤ������ǩ`���ˉ�Q���� */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			ar[X_SIZE * i + j] = (float)image_in[i][j];
			ai[X_SIZE * i + j] = 0.0;
		}
	}
	/* ����Ԫ�ƣƣԤ�g�Ф��� */
	if (fft2((float(*)[X_SIZE])ar, (float(*)[X_SIZE])ai, 1) == -1) return -1;
	/* �ƣƣԽY�����񻯤��� */
	max = 0;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			norm = ar[X_SIZE * i + j] * ar[X_SIZE * i + j]
				+ ai[X_SIZE * i + j] * ai[X_SIZE * i + j];    /* ����ɷ�Ӌ�� */
			if (norm != 0.0) norm = log(norm) / 2.0;
			else norm = 0.0;
			ar[X_SIZE * i + j] = (float)norm;
			if (norm > max) max = norm;
		}
	}
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			ar[X_SIZE * i + j] = (float)(ar[X_SIZE * i + j] * 255 / max);
		}
	}
	/* �ƣƣԽY������ǩ`���ˉ�Q���� */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			data = ar[X_SIZE * i + j];
			if (data > 255) data = 255;
			if (data < 0) data = 0;
			image_out[i][j] = (unsigned char)data;
		}
	}
	free(ar);
	free(ai);
	return 0;
}
int fftfilter(unsigned char image_in[Y_SIZE][X_SIZE],
	unsigned char image_out[Y_SIZE][X_SIZE], int a, int b)
{
	float* ar;   /*���ǩ`���g��������������ã���*/
	float* ai;   /*���ǩ`������������������ã���*/
	float* ff;   /*���ե��륿�ο��g�ܲ������ԡ���*/
	float    data;
	long    i, j, circ;

	ar = (float*)malloc((size_t)Y_SIZE * X_SIZE * sizeof(float));
	ai = (float*)malloc((size_t)Y_SIZE * X_SIZE * sizeof(float));
	ff = (float*)malloc((size_t)Y_SIZE * X_SIZE * sizeof(float));
	if ((ar == NULL) || (ai == NULL) || (ff == NULL)) {
		return -1;
	}
	/* ԭ������i���z�ߣ�����Ԫ�ƣƣԤ������ǩ`���ˉ�Q���� */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			ar[X_SIZE * i + j] = (float)image_in[i][j];
			ai[X_SIZE * i + j] = 0.0;
		}
	}
	/* �ƣƣԤ�g�Ф���ԭ������ܲ����ɷ֤ˉ�Q���� */
	if (fft2((float(*)[X_SIZE])ar, (float(*)[X_SIZE])ai, 1) == -1)
		return -1;
	/* �ܲ���a����b���¤γɷ֤�����ͨ�^����ե��륿������ */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			data = (float)((j - X_SIZE / 2) * (j - X_SIZE / 2)
				+ (i - Y_SIZE / 2) * (i - Y_SIZE / 2));
			circ = (long)sqrt(data);
			if ((circ >= a) && (circ <= b))
				ff[X_SIZE * i + j] = 1.0;
			else
				ff[X_SIZE * i + j] = 0.0;
		}
	}
	/* ԭ������ܲ����ɷ֤ˌ����ƥե��륿�I����Фʤ� */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			ar[X_SIZE * i + j] *= ff[X_SIZE * i + j];
			ai[X_SIZE * i + j] *= ff[X_SIZE * i + j];
		}
	}
	/* ��ƣƣԤ�g�Ф����ե��륿�I���줿�ܲ����ɷ֤���ˑ��� */
	if (fft2((float(*)[X_SIZE])ar, (float(*)[X_SIZE])ai, -1) == -1)
		return -1;
	/* �Y������ǩ`���ˉ�Q���� */
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			data = ar[X_SIZE * i + j];
			if (data > 255) data = 255;
			if (data < 0) data = 0;
			image_out[i][j] = (unsigned char)data;
		}
	}
	free(ar);
	free(ai);
	free(ff);
	return 0;
}