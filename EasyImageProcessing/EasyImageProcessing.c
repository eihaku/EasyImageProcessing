#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <conio.h>
#include <dos.h>
#include "params.h"


char bmpHeader[BITMAP_HEAD_SIZE];
char colorPalette[BITMAP_COLORPALETTE_SIZE];

PBITMAPFILEHEADER pBmpFileHeader;
PBITMAPINFOHEADER pBmpInfoHeader;

unsigned char image_in[Y_SIZE][X_SIZE];
unsigned char image_out[Y_SIZE][X_SIZE];
unsigned char image_work[Y_SIZE][X_SIZE];
unsigned char image_hist[HIST_Y_SIZE][HIST_X_SIZE];
long	hist[LEVEL];
float	ratio[Y_SIZE], size[X_SIZE];


void main()
{
	int i, j, k;
	int thres, mode;
	int eflg = 1;
	char source[80];
	char destin[80];
	float a, b, x0, y0, z0, deg, xr, yr, zr, v, scr;
	float amp1;
	float ratio_min, ratio_max;
	float size_min, size_max;
	int fmax, fmin;
	int cnt;

//	init_set();

	image_read(*image_in, X_SIZE, Y_SIZE, "LENNA.bmp");


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
			//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			printf("===============================================================\n\n");
			break;
		case 6:
			thinning(image_in, image_out);
			printf("\n\n");
			break;
		case 7:
			labeling(image_in, image_work, &cnt);
			features(image_work, image_out, cnt, size, ratio);
			//display(*image_out, X_SIZE, Y_SIZE, X_WORK_POS, Y_WORK_POS);
			image_write(*image_work, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, "710.bmp");
			image_write(*image_out, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, "711.bmp");

			image_copy(image_in, image_out);
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
					image_write(*image_out, pBmpInfoHeader->biWidth, pBmpInfoHeader->biHeight, "714.bmp");

				}
				threshold(image_out, image_out, 1, 1);
				//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
				if (i == 9) break;
			}
			break;
		case 8:
			printf("1 : 1次微分(gradient)     \n");
			printf("2 : 2次微分(laplacian)    \n");
			printf("3 : テンプレート		  \n");//模板
			printf("4 : 移動平均			  \n");
			printf("5 : メディアンフィルタ    \n");//中值滤波器
			printf("			***");
			scanf("%d", &i);
			if (i == 1 || i == 2 || i == 3) {
				printf("出力画像の利得　＊＊＊");
				scanf("%f", &amp1);
				if (i == 1) gradient(image_in, image_out, amp1);
				if (i == 2) laplacian(image_in, image_out, amp1);
				if (i == 3) template1(image_in, image_out, amp1);
			}

			if (i == 4) smooth(image_in, image_out, 1);
			if (i == 5) smooth(image_in, image_out, 2);
			//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
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
				histsmooth2(image_in, image_out, hist);

				//plane(image_in, image_out, hist);
			}
			//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 10:
			printf("1 : 拡大　　縮小（最近傍法）");
			printf("2：拡大 縮小（線形補間法）");
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
			if (i == 2) {
				printf("披大率  横，縦 ***");
				scanf("%f %f", &a, &b);
				scale(image_in, image_out, a, b);
			}
			if (i == 3) {
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
			if (i == 6) {
				printf("披大率  横，縦 X,   Y   ***");
				scanf("%f %f", &a, &b);
				printf(" 移動量  横，縱 X,   Y,  Z   ***");
				scanf("%f %f %f", &x0, &y0, &z0);
				printf("回転角(度) Z,   X,   Y   ***");
				scanf("%f %f %f", &zr, &xr, &yr);
				printf("視点の位置（Z方向)   ***");
				scanf("%f", &v);
				printf("スクリーンの位置（Z方向   ***");
				scanf("%f", &scr);
				perspect(image_in, image_out, a, b, x0, y0, z0, zr, xr, yr, v, scr);
			}
			//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 91: image_copy(image_out, image_in);
			//display(*image_in, X_SIZE, Y_SIZE, X_IN_POS, Y_IN_POS);
			break;
		case 92: image_copy(image_out, image_work);
			//display(*image_work, X_SIZE, Y_SIZE, X_WORK_POS, Y_WORK_POS);
			break;
		case 93: image_copy(image_work, image_in);
			//display(*image_in, X_SIZE, Y_SIZE, X_IN_POS, Y_IN_POS);
			break;
		case 94: masking(image_in, image_out, image_work);
			//display(*image_out, X_SIZE, Y_SIZE, X_OUT_POS, Y_OUT_POS);
			break;
		case 99: eflg = 0;
			break;
		default: printf("入力ェラー   ");
			break;
		}
	}
}