#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <conio.h>
#include <dos.h>
#include "params.h"

#ifdef HYPER
	#define display(image_in, xsize, ysize, xpos, ypos)\
			hyper_display(image_in, image_in, image_in, image_in, xsize, ysize, xpos, ypos)
	#define INIT_SET	HFOn();
	#define END_SET		HFOFF();
#else
	#ifdef GG
		#define display(image_in, xsize, ysize, xpos, ypos)\
				gg_display(image_in, image_in, image_in, xsize, ysize,xpos,ypos)
		#define INIT_SET
		#define END_SET
	#else
		#ifdef SHAZO
			#define display(image_in, xsize, ysize, xpos, ypos)\
					shazo_display(image_in, image_in, image_in, xsize, ysize,xpos,ypos)
			#define INIT_SET
			#define END_SET
		#else
			#define display(image_in, xsize, ysize, xpos, ypos)\
					gray_display(image_in, xsize, ysize, xpos, ypos)
			#define INIT_SET	graph_init();	gray_palette();
			#define END_SET		graph_clear();	palette8();
		#endif
	#endif
#endif

#define X_IN_POS	280
#define Y_IN_POS	50
#define X_OUT_POS	460
#define Y_OUT_POS	50
#define X_WORK_POS	370
#define Y_WORK_POS	210

unsigned char image_in[Y_SIZE][X_SIZE];
unsigned char image_out[Y_SIZE][X_SIZE];
unsigned char image_work[Y_SIZE][X_SIZE];
unsigned char image_hist[HIST_Y_SIZE][HIST_X_SIZE];
long	hist[LEVEL];
float	ratio[Y_SIZE], size[X_SIZE];
#define STDOUT stdout
#define L_BASE 100           // 暫定値
#define PI 3.14159265
#define ROOT2 1.41421356
#define BUFF_MAX 2500
#define MAX_X_SIZE 640
#define BITMAP_HEAD_SIZE         0x036
#define BITMAP_COLORPALETTE_SIZE 0x400

struct xyw {
	int x, y, w;
}buf[BUFF_MAX];

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned int   uint;

#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
	ushort  bfType;
	uint bfSize;
	ushort  bfReserved1;
	ushort  bfReserved2;
	uint bfOffBits;
} BITMAPFILEHEADER, * LPBITMAPFILEHEADER, * PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	uint    biSize;
	uint    biWidth;
	uint    biHeight;
	ushort  biPlanes;
	ushort  biBitCount;
	uint    biCompression;
	uint    biSizeImage;
	uint    biXPelsPerMeter;
	uint    biYPelsPerMeter;
	uint    biClrUsed;
	uint    biClrImportant;
} BITMAPINFOHEADER, * LPBITMAPINFOHEADER, * PBITMAPINFOHEADER;

char bmpHeader[BITMAP_HEAD_SIZE];
char colorPalette[BITMAP_COLORPALETTE_SIZE];

PBITMAPFILEHEADER pBmpFileHeader;
PBITMAPINFOHEADER pBmpInfoHeader;

void image_read(unsigned char* image, int xsize, int ysize, char* filename)
{
	FILE* fp;
	int DataOffset;
	int DataSize;
	int res;

	if ((fp = fopen(filename, "rb")) == NULL) {
		printf("%s open errer [rb] \n", filename);
		exit(-1);
	}
	/* bmp画像のヘッダー情報 */
	fread(bmpHeader, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 1, fp);
	pBmpFileHeader = (PBITMAPFILEHEADER)bmpHeader;
	pBmpInfoHeader = (PBITMAPFILEHEADER)(bmpHeader + sizeof(BITMAPFILEHEADER));

	/* 画像ビット深度確認 */
	if (pBmpInfoHeader->biBitCount > 8) {
		fclose(fp);
		printf("%s format error", filename);
	}

	/* 画像データの幅と高さ確認 */
	if (pBmpInfoHeader->biWidth > X_SIZE || pBmpInfoHeader->biHeight > Y_SIZE) {
		printf("%s  size error [%d]x[%d]\n", filename, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight);
	}
	DataSize = pBmpInfoHeader->biWidth * pBmpInfoHeader->biHeight;
	DataOffset = pBmpFileHeader->bfOffBits;

	/* bmp画像データOffset確認 */
	if (DataOffset != BITMAP_HEAD_SIZE + BITMAP_COLORPALETTE_SIZE) {
		fclose(fp);
		printf("%s format error [offset]", filename);
	}

	/* カラーパレットデータ取得 */
	fread(colorPalette, BITMAP_COLORPALETTE_SIZE, 1, fp);
	/* 画素データ取得 */
	fread(image, xsize, ysize, fp);
	fclose(fp);
}
void image_write(unsigned char* image, int xsize, int ysize, char* filename)
{
	FILE* fp;

	if ((fp = fopen(filename, "wb")) == NULL) {
		printf("%s open errer [wb] \n", filename);
		exit(-1);
	}

	fwrite(pBmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(pBmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(colorPalette, BITMAP_COLORPALETTE_SIZE, 1, fp);

	fwrite(image, xsize, ysize, fp);
	fclose(fp);
}

void main()
{
	int i, j, k;
	int thres, mode;
	int eflg = 1;
	char source[80];
	char destin[80];
	float a, b, x0, y0, z0, deg, xr, yr, zr, v, scr;
	float an, amp;
	float ratio_min, ratio_max;
	float size_min, size_max;
	int fmax, fmin;
	int cnt;

	init_set();

	while (eflg) {
		printf("＃＃＃画像処理プログラム　メニュー　＃＃＃\n");
		printf("１：入力画像データの読み込み\n");
		printf("２：出力画像の書き込み\n");
		printf("３：ヒストグラムの表示\n");
		printf("４：二値化\n");
		printf("５：二値画像の収縮、 膨張\n");
		printf("６：二値画像の細線化\n");
		printf("７：二値画像の特徴パラメータ\n");
		printf("８：フィルタ（３ｘ３）\n");
		printf("９：濃度変換\n");
		printf("１０：機何学変換\n");
		printf("９１：出力画像　＝＞入力画像のコピー\n");
		printf("９２：出力画像　＝＞ワーク画像のコピー\n");
		printf("９３：ワーク画像　＝＞入力画像のコピー\n");
		printf("９４：ワーク画像でマスク\n");
		printf("９９：終了　　\n");
		printf("処理番号　＊＊＊　");
		scanf("%d", &k);
		switch (k) {
		case 1:
			printf("入力ファイル名　＊＊＊");
			scanf("%s", source);
			image_read(*image_in, X_SIZE, Y_SIZE, source);
			//display(*image_in, X_SIZE, Y_SIZE, X_IN_POS, Y_IN_POS);
			printf("===============================================================\n\n");
			break;
		case 2:
			printf("出力ファイル名　＊＊＊");
			scanf("%s", destin);
			image_write(*image_out, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, destin);
			//image_write(*image_out, X_SIZE, Y_SIZE, destin);
			printf("===============================================================\n\n");
			break;
		case 3:
			histgram(image_in, hist);
			histimage(hist, image_hist);
			image_copy2(image_out, image_hist);
			image_write(*image_out, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, "hist.bmp");
			printf("===============================================================\n\n");
			break;
		case 4:
			printf("閾値   ** ");
			scanf("%d", &thres);
			printf("そのまま　= 1, 反転 = 2  ***  ");
			scanf("%d", &mode);
			mode = 1;
				threshold(image_in, image_work, thres, mode);
				image_copy(image_out, image_work);
				{
					char thrFileName[32];
					sprintf(thrFileName, "threshold_%03d.bmp", thres);
					image_write(*image_out, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, thrFileName);
				}
			printf("===============================================================\n\n");
			break;
		case 5:
			printf("1 : 収縮    2 :  膨張  *** ");
			scanf("%d", &i);

			if (i == 1) contraction(image_in, image_out);
			if (i == 2) expansion(image_in, image_out);
			display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			printf("===============================================================\n\n");
			break;
		case 6:
			thinning(image_in, image_out);
			printf("\n\n");
			break;
		case 7:
			labeling(image_in, image_work, &cnt);
			features(image_work, image_out, cnt, size, ratio);
			display(*image_out, X_SIZE, Y_SIZE, X_WORK_POS, Y_WORK_POS);
			image_copy(*image_work, *image_in);
			for (;;) {
				printf("1 : 円形度により物体を抽出　　\n");
				printf("2 : 面積により物体を抽出	　\n");
				printf("9 : メニューへ				　\n");
				printf("			*** ");
				scanf("%d", &i);
				if (i == 1) {
					printf("円形度の最小、最大  ***");
					scanf("%f %f", &ratio_min, &ratio_max);
					ratio_extract(image_in, image_out, cnt, ratio, ratio_min, ratio_max);
				}
				if (i == 2) {
					printf("面積の最小、最大  ***");
					scanf("%f %f", &size_min, &size_max);
					size_extract(image_in, image_out, cnt, size, size_min, size_max);
				}
				threshold(image_out, image_out, 1, 1);
				display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
				if (i == 9) break;
			}
			break;
		case 8:
			printf("1 : 1次微分(gradient)     \n");
			printf("2 : 2次微分(laplacian)    \n");
			printf("3 : テンプレート		  \n");
			printf("4 : 移動平均			  \n");
			printf("5 : メディアンフィルタ    \n");
			printf("			***");
			scanf("%d", &i);
			if (i == 1 || i == 2 || i == 3) {
				printf("出力画像の利得　＊＊＊");
				scanf("%f", &amp);
			}
			if (i == 1) gradient(image_in, image_out, amp);
			if (i == 2) laplacian(image_in, image_out, amp);
			if (i == 3) template1(image_in, image_out, amp);
			if (i == 4) smooth(image_in, image_out, 1);
			if (i == 5) smooth(image_in, image_out, 2);
			display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 9:
			printf("1 :  明るさをｎ倍              \n");
			printf("2 :  0～255への引き延ばし　    \n");
			printf("3 :  ヒストグラムの平坦化      \n");
			printf(" 　　　　　＊＊＊");
			scanf("%d", &i);
			if (i == 1) {
				printf("倍率  ***"); scanf("%d", &j);
				amplify(image_in, image_out, j);
			}
			if (i == 2) {
				range(image_in, &fmax, &fmin);
				enpand(image_in, image_out, fmax, fmin);
			}
			if (i == 3) {
				histgram(image_in, hist);
				plane(image_in, image_out, hist);
			}
			display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 10:
			printf("1 : 拡大　　縮小（最近傍法）");
			printf("2：拉大•縮小（線形補間法）");
			printf("3: 移動（線形補間法）");
			printf("4：回転（線形補間法）");
			printf("5：アフィン変換（線形補間法");
			printf("6：透視変換");
			scanf("%d", &i);
			if (i == 1) {
				printf("拡大率  横，縦 ***");
				scanf("%f %f", &a, &b);
				scale_near(image_in, image_out, a, b);
			}
			if (1 == 2) {
				printf("披大率  横，縦 ***");
				scanf("%f%f", &a, &b);
				scale(image_in, image_out, a, b);
			}
			if (1 == 3) {
				printf(" 移動量  横，縱 ***");
				scanf(" %f %f", &x0, &y0);
				shift(image_in, image_out, x0, y0);
			}
			if (i == 4) {
				printf("回転角(度) ***");
				scanf("%f", &deg);
				rotation(image_in, image_out, deg);
			}
			if (i == 5) {
				printf("披大率  横，縦 ***");
				scanf("%f %f", &a, &b);
				printf(" 移動量  横，縱 ***");
				scanf(" %f %f", &x0, &y0);
				printf("回転角(度) ***");
				scanf("%f", &deg);
				affine(image_in, image_out, deg, a, b, x0, y0);
			}
			if (1 == 6) {
				printf("披大率  横，縦 X,   Y   ***");
				scanf("%f%f", &a, &b);
				printf(" 移動量  横，縱 X,   Y,  Z   ***");
				scanf(" %f %f %f", &x0, &y0, &z0);
				printf("回転角(度) Z,   X,   Y   ***");
				scanf("%f%f%f", &zr, &xr, &yr);
				printf("視点の位置（Z方向)   ***");
				scanf("%f", &v);
				printf("スクリーンの位置（Z方向   ***");
				scanf("%f", &scr);
				perspect(image_in, image_out, a, b, x0, y0, z0, zr, xr, yr, v, scr);
			}
			display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 91: image_copy(*image_out, *image_in);
			display(*image_in, X_SIZE, Y_SIZE, X_IN_POS, Y_IN_POS);
			break;
		case 92: image_copy(*image_out, *image_work);
			display(*image_work, X_SIZE, Y_SIZE, X_WORK_POS, Y_WORK_POS);
			break;
		case 93: image_copy(*image_work, *image_in);
			display(*image_in, X_SIZE, Y_SIZE, X_IN_POS, Y_IN_POS);
			break;
		case 94: masking(image_in, image_out, image_work);
			display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 99: eflg = 0;
			break;
		default: printf("入力ェラー   ");
			break;
		}

	}
	end_set();
}

void init_set()
{
	END_SET
}

void image_copy(unsigned char* image_dest, unsigned char* image_src)
{
	/*copy*/
	memcpy(image_dest, image_src, Y_SIZE * X_SIZE);
}

void image_copy2(unsigned char* image_out, unsigned char* image_in)
{
	/*copy*/
	for (int i = 0; i < Y_SIZE; i++) {
		memcpy(image_out + (X_SIZE * (Y_SIZE - 1 - i)), image_in + (i * X_SIZE), Y_SIZE);
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
/*--- histgram --- ヒストグラムを求める処理 -----------------------------------
	image_in:    入力画像配列
	hist:        ヒストグラム
-----------------------------------------------------------------------------*/
void histgram(unsigned char* image, long hist[]) {
	int n = 0;
	int i = 0;
	int j = 0;

	for (n = 0; n < LEVEL; n++) hist[n] = 0;
	for (i = 0; i < Y_SIZE * X_SIZE; i++) {
		n = image[i];
		hist[n]++;
	}
}

/*　ヒストグラムの画像化　
LEVEL = 256
HIST_X_SIZE = 5
HIST_y_SIZE = 10
*/
float histimage(long hist[], unsigned char* image_hist) {
	int i = 0;
	int j = 0;
	int k = 0;

	int  max, ratio, range;
	long n;
	float d;
	ratio = LEVEL / HIST_X_SIZE; //256/128
	range = HIST_Y_SIZE - 5;     //128-5
	for (i = 0; i < HIST_Y_SIZE * HIST_X_SIZE; i++) {
		image_hist[i] = LOW;
		
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
			for (j = 0; j <= n; j++) image_hist[((range - j) * HIST_Y_SIZE) + i] = HIGH;
		}
		for (i = 0; i <= 4; i++) {
			k = (HIST_X_SIZE / 4) * i;
			if (k >= HIST_X_SIZE) k = HIST_X_SIZE - 1;
			for (j = range; j < HIST_Y_SIZE; j++) image_hist[j * HIST_Y_SIZE + k] = HIGH;
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
			for (j = 0; j <= n; j++) image_hist[((range - j) * HIST_Y_SIZE) + i] = HIGH;

		}
		for (i = 0; i <= 4; i++) {
			k = (HIST_X_SIZE / 4) * i;
			if (k >= HIST_X_SIZE) k = HIST_X_SIZE - 1;
			for (j = range; j < HIST_Y_SIZE; j++) image_hist[j * HIST_Y_SIZE + k] = HIGH;

		}
		break;
	default:
		return(-1);

		break;

	}
}


void histsmooth(long hist_in[], long hist_out[]) {
	int m, n, i;
	long sum;

	for (n = 0; n < LEVEL; n++) {
		sum = 0;
		for (n = 0; n <= 2; m++) {
			i = n + m;
			if (i < 0) i = 0;
			if (i > LEVEL - 1) i = LEVEL - 1;
			sum = sum + hist_in[i];
		}
		hist_out[n] = (float)sum / 5.0 + 5.0;
	}
}


void threshold(unsigned char* image_in, unsigned char* image_out, int thresh, int mode) 
{
	int i, j;
	int temp;

	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			temp = i * Y_SIZE + j;
			switch (mode) {
			case 1:
				if ((int)image_in[temp] >= thresh) {
					image_out[temp] = HIGH;
				}
				else {
					image_out[temp] = LOW;
				}
				break;
			case 2:
				if ((int)image_in[temp] >= thresh) {
					image_out[temp] = LOW;
				}
				else {
					image_out[temp] = HIGH;
				}
				break;
			default:
				break;
			}
		}
	}
}


void contraction(unsigned char* image_in, unsigned char* image_out) {
	int i, j;
	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			image_out[i * Y_SIZE + j] = image_in[i * Y_SIZE + j];
			if (image_in[(i - 1) * Y_SIZE + (j - 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[(i - 1) * Y_SIZE + j] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[(i - 1) * Y_SIZE + (j + 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[i * Y_SIZE + (j - 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[i * Y_SIZE + (j + 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[(i + 1) * Y_SIZE + (j - 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[(i + 1) * Y_SIZE + j] == LOW) image_out[i * Y_SIZE + j] = LOW;
			if (image_in[(i + 1) * Y_SIZE + (j + 1)] == LOW) image_out[i * Y_SIZE + j] = LOW;
		}
	}
}

void expansion(unsigned char* image_in, unsigned char* image_out) {
	int i, j;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			image_out[i * Y_SIZE + j] = image_in[i * Y_SIZE + j];
			if (image_in[(i - 1) * Y_SIZE + (j - 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[(i - 1) * Y_SIZE + j] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[(i - 1) * Y_SIZE + (j + 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[i * Y_SIZE + (j - 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[i * Y_SIZE + (j + 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[(i + 1) * Y_SIZE + (j - 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[(i + 1) * Y_SIZE + j] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
			if (image_in[(i + 1) * Y_SIZE + (j + 1)] == HIGH) image_out[i * Y_SIZE + j] = HIGH;
		}
	}
}
//二値画像の細線化
void thinning(unsigned char* image_in, unsigned char* image_out) {
	int ia[9] = { 0 };
	int ic[9] = { 0 };
	int i, ix, iy, m, ir, iv, iw;

	for (iy = 0; iy < Y_SIZE; iy++)
		for (ix = 0; ix < X_SIZE; ix++)
			image_out[iy * Y_SIZE + ix] = image_in[iy * Y_SIZE + ix];
	m = 100; ir = 1;
	while (ir != 0) {
		ir = 0;
		for (iy = 1; iy < Y_SIZE; iy++)
			for (ix = 1; ix < Y_SIZE; ix++) {
				if (image_out[iy * Y_SIZE + ix] != HIGH) {
					continue;
				}
				ia[0] = image_out[iy * Y_SIZE + ix + 1];
				ia[1] = image_out[(iy - 1) * Y_SIZE + ix + 1];
				ia[2] = image_out[(iy - 1) * Y_SIZE + ix];
				ia[3] = image_out[(iy - 1) * Y_SIZE + ix - 1];
				ia[4] = image_out[iy * Y_SIZE + ix - 1];
				ia[5] = image_out[(iy + 1) * Y_SIZE + ix - 1];
				ia[6] = image_out[(iy + 1) * Y_SIZE + ix];
				ia[7] = image_out[(iy + 1)* Y_SIZE + ix + 1];
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
				if (image_out[(iy - 1) * Y_SIZE + ix] == m) {
					ia[2] = 0;
					if (cconc(ia) != 1) continue;
					ia[2] = HIGH;
				}
				if (image_out[iy * Y_SIZE + ix - 1] == m) {
					ia[4] = 0;
					if (cconc(ia) != 1) continue;
					ia[4] = HIGH;
				}
				image_out[iy * Y_SIZE + ix] = m;
				ir++;
			}
		m++;
	}

	for (iy = 0; iy < Y_SIZE; iy++) {

		for (ix = 0; ix < X_SIZE; ix++) {
			if (image_out[iy * Y_SIZE + ix] < HIGH) {
				image_out[iy * Y_SIZE + ix] = 0;
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

void labeling(unsigned char* image_in, unsigned char* image_label, int* cnt) {
	int i, j, label;
	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++)
			image_label[i * Y_SIZE + j] = image_in[i * Y_SIZE + j];
	label = L_BASE;
	for (i = 0; i < Y_SIZE; i++)
		for (j = 0; j < X_SIZE; j++) {
			if (image_label[i * Y_SIZE + j] == HIGH) {
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

int labelset(unsigned char* image, int xs, int ys, int label) {
	int i, j, cnt;

	image[ys * Y_SIZE + xs] = label;
	for (;;) {
		cnt = 0;
		for (i = 0; i < Y_SIZE; i++) {
			for (j = 0; j < X_SIZE; j++) {
				if (image[i * Y_SIZE + j] == label) {
					if (image[i * Y_SIZE + j + 1] == HIGH) {
						image[i * Y_SIZE + j + 1] = label;
						cnt++;
					}
					if (image[i * Y_SIZE + j + 1] == HIGH) {
						image[i * Y_SIZE + j + 1] = label;
						cnt++;
					}
					if (image[(i-1) * Y_SIZE + j] == HIGH) {
						image[(i - 1) * Y_SIZE + j] = label;
						cnt++;
					}
					if (image[(i - 1) * Y_SIZE + j - 1] == HIGH) {
						image[(i - 1) * Y_SIZE + j - 1] = label;
						cnt++;
					}
					if (image[i * Y_SIZE + j - 1] == HIGH) {
						image[i * Y_SIZE + j - 1] = label;
						cnt++;
					}
					if (image[(i + 1) * Y_SIZE + j - 1] == HIGH) {
						image[(i + 1) * Y_SIZE + j - 1] = label;
						cnt++;
					}
					if (image[(i + 1) * Y_SIZE + j] == HIGH) {
						image[(i + 1) * Y_SIZE + j] = label;
						cnt++;
					}
					if (image[(i + 1) * Y_SIZE + j + 1] == HIGH) {
						image[(i + 1) * Y_SIZE + j + 1] = label;
						cnt++;
					}
				}
				if (cnt == 0) {
					return(0);
				}
			}
		}
	}
}

void features(unsigned char* image_label_in, unsigned char* image_label_out, int cnt, float size[], float ratio[]) {
	int i, j, center_x, center_y;
	float l;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			image_label_out[i * Y_SIZE + j] = image_label_in[i * Y_SIZE + j];
		}
	}
	printf("no      面積　　周囲長　　円形度　　重心(x,y)\n");
	for (i = 0; i < cnt; i++) {
		size[i] = calc_size(image_label_out, (i + L_BASE), &center_x, &center_y);
		l = calc_length(image_label_out, i + L_BASE);
		ratio[i] = 4 * PI * size[i] / (1 * 1);
		image_label_out[center_y * Y_SIZE + center_x] = HIGH;
		printf("%d   %f   %f   %f   (%d,%d\n", i, size[i], 1, ratio, center_x, center_y);
	}
}
int calc_size(unsigned char* image_label[], int label, int* cx, int* cy) {
	int i, j;
	float tx, ty, total;
	tx = 0; ty = 0; total = 0;
	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			if (image_label[i][j] == label) {
				tx += j; ty += i; total++;
			}
			if (total == 0.0)return(0.0);
			*cx = tx / total; *cy = ty / total;
			return(total);
		}
	}
}
int calc_length(unsigned char* image_label[], int label) {
	int i, j;

	for (i = 0; i < Y_SIZE; i++) {
		for (j = 0; j < X_SIZE; j++) {
			if (image_label[i][j] == label) {
				return(trace(image_label, j - 1, i));
			}
		}
	}
}

int trace(unsigned char* image_label[], int  xs, int ys) {
	int x, y, no, vec;
	float l;
	l = 0; x = xs; y = ys; no = image_label[y][x + 1]; vec = 5;

	for (;;) {
		if (x == xs && y == ys && l != 0) {
			return(1);
		}
		image_label[y][x] = HIGH;
		switch (vec)
		{
		case 3:
			if (image_label[y][x + 1] != no && image_label[y - 1][x + 1] == no)
			{
				x = x + 1; y = y; l++;
				vec = 0; continue;
			}
		case 4:
			if (image_label[y - 1][x + 1] != no && image_label[y - 1][x] == no)
			{
				x = x + 1; y = y - 1; l++;
				vec = 1; continue;
			}
		case 5:
			if (image_label[y - 1][x] != no && image_label[y - 1][x - 1] == no)
			{
				x = x; y = y - 1; l++;
				vec = 2; continue;
			}
		case 6:
			if (image_label[y - 1][x - 1] != no && image_label[y][x - 1] == no)
			{
				x = x - 1; y = y - 1; l++;
				vec = 3; continue;
			}
		case 7:
			if (image_label[y][x - 1] != no && image_label[y + 1][x - 1] == no)
			{
				x = x - 1; y = y; l++;
				vec = 4; continue;
			}
		case 0:
			if (image_label[y + 1][x] != no && image_label[y + 1][x] == no)
			{
				x = x - 1; y = y + 1; l++;
				vec = 5; continue;
			}
		case 1:
			if (image_label[y + 1][x + 1] != no && image_label[y + 1][x] == no)
			{
				x = x; y = y + 1; l++;
				vec = 6; continue;
			}
		case 2:
			if (image_label[y + 1][x + 1] != no && image_label[y][x + 1] == no)
			{
				x = x + 1; y = y + 1; l++;
				vec = 7; continue;
			}
			vec = 3;
		}
	}
}
void ratio_extract(unsigned char* image_label_in[], unsigned char* image_label_out[], int cnt, float ratio[], float ratio_min, float ratio_max) {
	int i, j, x, y;
	int lno[LEVEL];

	for (i = 0, j = 0; i < cnt; i++)
		if (ratio[i] >= ratio_min && ratio[i] <= ratio_max) {
			lno[j++] = L_BASE + i;
		}
	for (y = 0; y < Y_SIZE; y++)
		for (x = 0; x < X_SIZE; x++) {
			image_label_out[y][x] = 0;
			for (i = 0; i < j; i++) {
				if (image_label_in[y][x] == lno[i])
					image_label_out[y][x] = image_label_in[y][x];
			}
		}
}
void size_extract(unsigned char* image_label_in[], unsigned char* image_label_out[], int cnt, float size[], float size_min, float size_max) {
	int i, j, x, y;
	int lno[LEVEL];

	for (i = 0, j = 0; i < cnt; i++) {
		if (size[i] >= size_min && size[i] <= size_max) {
			lno[j++] = L_BASE + i;
		}
		for (y = 0; y < Y_SIZE; y++) {
			for (x = 0; x < X_SIZE; x++) {
				image_label_out[y][x] = 0;
				for (i = 0; i < j; i++) {
					if (image_label_in[y][x] == lno[i])
						image_label_out[y][x] = image_label_in[y][x];
				}
			}
		}
	}
}
void gradient(unsigned char* image_in[], unsigned char* image_out[], float amp1) {
	static int cx[9] = { 0,0,0,0,1,0,0,0,-1 };
	static int cy[9] = { 0,0,0,0,0,1,0,-1,0 };
	int d[9];
	int i, j, dat;
	float xx, yy, zz;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d[0] = image_in[i - 1][j - 1];
			d[1] = image_in[i - 1][j];
			d[2] = image_in[i - 1][j + 1];
			d[3] = image_in[i][j - 1];
			d[4] = image_in[i][j];
			d[5] = image_in[i][j + 1];
			d[6] = image_in[i + 1][j - 1];
			d[7] = image_in[i + 1][j];
			d[8] = image_in[i + 1][j + 1];
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
void laplacian(unsigned char* image_in[], unsigned char* image_out[], float amp1) {
	static int c[9] = { -1,-1,-1,-1,8,-1,-1,-1,-1 };
	int d[9];
	int i, j, dat;
	float z, zz;

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d[0] = image_in[i - 1][j - 1];
			d[1] = image_in[i - 1][j];
			d[2] = image_in[i - 1][j + 1];
			d[3] = image_in[i][j - 1];
			d[4] = image_in[i][j];
			d[5] = image_in[i][j + 1];
			d[6] = image_in[i + 1][j - 1];
			d[7] = image_in[i + 1][j];
			d[8] = image_in[i + 1][j + 1];
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
void template1(unsigned char* image_in[], unsigned char* image_out[], float amp1) {
	int d0, d1, d2, d3, d4, d5, d6, d7, d8;
	int i, j, k, dat, max;
	int m[8];
	float zz;
	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			d0 = image_in[i - 1][j - 1];
			d1 = image_in[i - 1][j];
			d2 = image_in[i - 1][j + 1];
			d3 = image_in[i][j - 1];
			d4 = image_in[i][j];
			d5 = image_in[i][j + 1];
			d6 = image_in[i + 1][j - 1];
			d7 = image_in[i + 1][j];
			d8 = image_in[i + 1][j + 1];

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

void smooth(unsigned char* image_in[], unsigned char* image_out[], int which) {
	int i, j;
	unsigned char c[9];

	for (i = 1; i < Y_SIZE - 1; i++) {
		for (j = 1; j < X_SIZE - 1; j++) {
			c[0] = image_in[i - 1][j - 1];
			c[1] = image_in[i - 1][j];
			c[2] = image_in[i - 1][j + 1];
			c[3] = image_in[i][j - 1];
			c[4] = image_in[i][j];
			c[5] = image_in[i][j + 1];
			c[6] = image_in[i + 1][j - 1];
			c[7] = image_in[i + 1][j];
			c[8] = image_in[i + 1][j + 1];
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

void amplify(unsigned char* image_in[], unsigned char* image_out[], int n) {
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
void range(unsigned char* image_in[], int* fmax, int* fmin) {
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

void enpand(unsigned char* image_in[], unsigned char* image_out[], int fmax, int  fmin) {
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
void plane(unsigned char* image_in[], unsigned char* image_out[], int hist[]) {
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
			for (j = 0; j < delt; j++) {
				image_out[buf[j].y][buf[j].x] = (unsigned char)i;
				image_in[buf[j].y][buf[j].x] = (unsigned char)(low + 1);
			}


			hist[low] = hist[low] - delt;
			high = low;
		}
	}
}
void sort(unsigned char* image_in[], struct xyw data[], int level) {
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
void weight(unsigned char* image_in[], int i, int j, int* wt) {
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
void scale_near(unsigned char* image_in[], unsigned char* image_out[], float zx, float zy) {
	int i, j, m, n;
	float x, y, p, q;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;

	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			if (i > 0)m = i / zy + 0.5; else m - i / zy - 0.5;
			if (j > 0)n = j / zy + 0.5; else n - i / zy - 0.5;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs))
				image_out[i + ys][j + xs] = image_in[m + ys][n + xs];
			else
				image_out[i + ys][j + xs] = 0;
		}
	}
}
void scale(unsigned char* image_in[], unsigned char* image_out[], float zx, float zy) {
	int i, j, m, n;
	float x, y, p, q;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;
	int d;
	for (i = -ys; i < ys; i++) {
		for (j = -xs; j < xs; j++) {
			y = i / zy;
			x = j / zx;
			if (y > 0)m = y; else m = y - 1;
			if (x > 0)n = x; else n = x - 1;
			q - y - m;
			p = x - n;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs))
				d = (1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs] + p * image_in[m + ys][n + 1 + xs]) + q * ((1.0 - p) * image_in[m + 1 + ys][n + xs] + p * image_in[m + 1 + ys][n + 1 + xs]);
			else
				d = 0;
			if (d < 0)d = 0;
			if (d > 255)d = 255;
			image_out[i + ys][j + xs] = d;
		}
	}
}
void rotation(unsigned char* image_in[], unsigned char* image_out[], float deg) {
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
			if (y > 0)m = y; else m = y - 1;
			if (x > 0)n = x; else n = x - 1;
			q = y - m;
			p = x - n;
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs))
				d = (1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs] + p * image_in[m + ys][n + xs]) + q * ((1.0 - p) * image_in[m + 1 + ys][n + xs] + p * image_in[m + 1 + ys][n + 1 + xs]);
			else
				d = 0;
			if (d < 0) {
				d = 0;
			}
			if (d > 255) {
				d = 255;
				image_out[i + ys][j + xs] = d;
			}
		}
	}
}
void shift(unsigned char* image_in[], unsigned char* image_out[], float px, float py) {
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
				d = (1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs] + p * image_in[m + ys][n + 1 + xs]) + q * ((1.0 - p) * image_in[m + 1 + ys][n + xs] + p * image_in[m + 1 + ys][n + 1 + xs]);
			}
			else {

				if (d < 0)
				{
					d = 0;
				}
				if (d > 255) {
					d = 255;
				}
				image_out[i + ys][j + xs] = d;
			}
		}
	}
}
void affine(unsigned char* image_in[], unsigned char* image_out[], float deg, float zx, float zy, float px, float py) {
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
			x + (u * c - v * s) / zx;
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

				q = y - m;
				p = x - n;
			}
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {

				d = (1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs] + p * image_in[m + ys][n + 1 + xs]) + q * ((1.0 - p) * image_in[m + 1 + ys][n + xs] + p * image_in[m + 1 + ys][n + 1 + xs]);
			}
			else {
				d = 0;
				if (d < 0)d = 0;
				if (d > 255)d = 255;
				image_out[i + ys][j + xs] = d;


			}

		}
	}
}
void perspect(unsigned char* image_in[], unsigned char* image_out[], float ax, float ay, float px, float py, float pz, float rz, float rx, float ry, float v, float s) {
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

				q = y - m;
				p = x - n;
			}
			if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {

				d = (1.0 - q) * ((1.0 - p) * image_in[m + ys][n + xs] + p * image_in[m + ys][n + 1 + xs]) + q * ((1.0 - p) * image_in[m + 1 + ys][n + xs] + p * image_in[m + 1 + ys][n + 1 + xs]);
			}
			else {
				d = 0;
				if (d < 0)d = 0;
				if (d > 255)d = 255;
				image_out[i + ys][j + xs] = d;
			}


		}
	}
}
void param_pers(float k[], float a, float b, float x0, float y0, float z0, float z, float x, float y, float t, float s) {
	float l[4][4], m[4][4], n[4][4], k1, k2, k3, k4, k5, k6, k7, k8, k9;
	double u, v, w;
	int i;
	int xs = X_SIZE / 2;
	int ys = Y_SIZE / 2;

	u = x * 3.141592 / 180.0;
	v = y * 3.141592 / 180.0;
	w = z * 3.141592 / 180.0;

	l[0][0] = 1.0 / xs; l[0][1] = 0; l[0][2] = 0; l[0][3] = 0; l[1][0] = 0; l[1][1] = -1.0 / xs; l[1][2] = 0; l[1][3] = 0; l[2][0] = 0; l[2][1] = 0; l[2][2] = 1; l[2][3] = 0; l[3][0] = 0; l[3][1] = 0; l[3][2] = 0; l[3][3] = 1;
	m[0][0] = a; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0; m[1][0] = 0; m[1][1] = b; m[1][2] = 0; m[1][3] = 0; m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0; m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	matrix(l, m, n);
	l[0][0] = 1; l[0][1] = 0; l[0][2] = 0; l[0][3] = 0; l[1][0] = 0; l[1][1] = -1; l[1][2] = 0; l[1][3] = 0; l[2][0] = 0; l[2][1] = 0; l[2][2] = 1; l[2][3] = 0; l[3][0] = 0; l[3][1] = 0; l[3][2] = 0; l[3][3] = 1;
	matrix(n, l, m);
	n[0][0] = cos(w); n[0][1] = sin(w); n[0][2] = 0; n[0][3] = 0; n[1][0] = -sin(w); n[1][1] = cos(w); n[1][2] = 0; n[1][3] = 0; n[2][0] = 0; n[2][1] = 0; n[2][2] = 1; n[2][3] = 0; n[3][0] = 0; n[3][1] = 0; n[3][2] = 0; n[2][3] = 1;
	matrix(m, n, l);
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0; m[1][0] = 0; m[1][1] = cos(u); m[1][2] = sin(u); m[1][3] = 0; m[2][0] = 0; m[2][1] = -sin(u); m[2][2] = cos(u); m[2][3] = 0; m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	matrix(l, m, n);
	l[0][0] = cos(v); l[0][1] = 0; l[0][2] = sin(v); l[0][3] = 0; l[1][0] = 0; l[1][1] = 1; l[1][2] = 0; l[1][3] = 0; l[2][0] = -sin(v); l[2][1] = 0; l[2][2] = cos(v); l[2][3] = 0; l[3][0] = 0; l[3][1] = 0; l[3][2] = 0; l[3][3] = 1;
	matrix(n, l, m);
	n[0][0] = 1; n[0][1] = 0; n[0][2] = 0; n[0][3] = 0; n[1][0] = 0; n[1][1] = 1; n[1][2] = 0; n[1][3] = 0; n[2][0] = 0; n[2][1] = 0; n[2][2] = -1; n[2][3] = 0; n[3][0] = 0; n[3][1] = 0; n[3][2] = t; n[3][3] = 1;
	matrix(m, n, l);
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0; m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0; m[2][0] = 0; m[2][1] = -sin(u); m[2][2] = 1 / s; m[2][3] = 1 / s;; m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	matrix(l, m, n);
	l[0][0] = xs; l[0][1] = 0; l[0][2] = 0; l[0][3] = 0; l[1][0] = 0; l[1][1] = 1; l[1][2] = 0; l[1][3] = 0; l[2][0] = 0; l[2][1] = 0; l[2][2] = 1 / s; l[2][3] = 1 / s; l[3][0] = 0; l[3][1] = 0; l[3][2] = 0; l[3][3] = 1;
	matrix(n, l, m);
	k1 = m[0][3]; k2 = m[1][3]; k3 = m[3][3];
	k4 = m[0][0]; k5 = m[1][0]; k6 = m[3][0];
	k7 = m[0][1]; k8 = m[1][1]; k9 = m[3][1];
	k[0] = k7 * k2 - k8 * k1;	k[1] = k5 * k1 - k4 * k2;	k[2] = k4 * k8 - k7 * k5;
	k[3] = k8 * k3 - k9 * k2;	k[6] = k9 * k1 - k7 * k3;	k[4] = k6 * k2 - k5 * k3;
	k[7] = k4 * k3 - k6 * k1;	k[5] = k5 * k9 - k8 * k6;	k[8] = k7 * k6 - k4 * k9;
	/*
	printf("r= %f\n", k[2]);
	printf("a= %f\n", k[3]);
	printf("b= %f\n", k[4]);
	printf("c= %f\n", k[5]);
	printf("d= %f\n", k[6]);
	printf("e= %f\n", k[7]);
	printf("f= %f\n", k[8]);

	*/

}
void matrix(float* l[4], float* m[4], float* n[4]) {
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
void  masking(unsigned char* image_in[], unsigned char* image_out[], unsigned char image_mask[]) {
	int i;
	for (i = 0; i < Y_SIZE * X_SIZE; i++) {
		if (image_mask[i] == HIGH) {
			image_out[i] = image_in[i];
		}
		else {
			image_out[i] = 0;
		}
	}
}
void text_clear() {
	printf("\0333[2J)");
}
void gray_display(unsigned char image_in[], int xsize, int ysize, int xpos, int ypos) {
	int x, y;
	for (y = 0; y < ysize; y++)
		for (x = 0; x < xsize; x++)
			piel_write(xpos + x, ypos + y, (int)image_in[xsize * y + x] / 16);
}
void piel_write(int x, int y, int pale_no)
{
#if 0
//	char far* p[4];
	char *p[4];
	int i, posi;
	static char bit[4] = { 1,2,4,8 };
	static char bit_mask[8] = { 0x80,0x40,0x20,0x10,8,4,2,1 };
	if (pale_no > 15) {
		pale_no = 15;
	}
	posi = x % 8;
//	p[0] = (char far*)(0xa8000000 + y * 80 + x / 8);
//	p[1] = (char far*)(0xb0000000 + y * 80 + x / 8);
//	p[2] = (char far*)(0xb8000000 + y * 80 + x / 8);
//	p[3] = (char far*)(0xxe0000000 + y * 80 + x / 8);
	p[0] = (char*)(0xa8000000 + y * 80 + x / 8);
	p[1] = (char*)(0xb0000000 + y * 80 + x / 8);
	p[2] = (char*)(0xb8000000 + y * 80 + x / 8);
	p[3] = (char*)(0xe0000000 + y * 80 + x / 8);
	for (i = 0; i < 4; i++) {
		if ((pale_no & bit[i] == 0)) {
			*p[i] &= ~bit_mask[posi];
		}
		else {
			*p[i] |= bit_mask[posi];
		}
	}
#endif
}

/* グラフィック画面をクリアする */
void graph_clear() {
	/*	long i;
	//	char far* r, far* g, far* b, far* k;
		char* r;
		char* g;
		char* b;
		char* k;

		r = (char*)0xb000000001;
		g = (char*)0xb000000001;
		b = (char*)0xb000000001;
		k = (char*)0xb000000001;
		r = (char*)0xb000000001;
		g = (char*)0xb000000001;
		b = (char*)0xb000000001;
		k = (char*)0xb000000001;

		for (i = 0; i < 32000; i++) {
			*r++ = 0;
			*g++ = 0;
			*b++ = 0;
			*k++ = 0;
		}
		*/
}
void palette8() {
	//outp(0x6a, 0);
}
void end_set() {

}
//void outp(int a, int b) {

//}

