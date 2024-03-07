#pragma once
#define X_SIZE      256
#define Y_SIZE      256
#define HIST_X_SIZE X_SIZE
#define HIST_Y_SIZE Y_SIZE
#define HIGH        255
#define LOW         0
#define LEVEL       256
#define X_EXP 8 /* X_EXP = log2(X_SIZE)  */
#define Y_EXP 8 /* Y_EXP = log2(Y_SIZE)  */
#define R 0
#define G 1
#define B 2

#define X_IN_POS	280
#define Y_IN_POS	50
#define X_OUT_POS	460
#define Y_OUT_POS	50
#define X_WORK_POS	370
#define Y_WORK_POS	210
#define STDOUT stdout
#define L_BASE 100           // 暫定値
#define PI 3.14159265
#define ROOT2 1.41421356
#define BUFF_MAX 2500
#define MAX_X_SIZE 640
#define BITMAP_HEAD_SIZE         0x036
#define BITMAP_COLORPALETTE_SIZE 0x400
#define    OPT        1     /*　OPT = 1　光学的ＤＦＴ（直流分が中央）    */
							/*　OPT = 0　通常のＤＦＴ（直流分が左端）    */

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned int   uint;


#pragma warning(disable:6031)
#pragma warning(disable:6064)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4715)
#pragma warning(disable:26451)
#pragma pack(1)
struct xyw {
	int x, y, w;
}buf[BUFF_MAX];

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

extern char bmpHeader[BITMAP_HEAD_SIZE];
extern char colorPalette[BITMAP_COLORPALETTE_SIZE];

extern PBITMAPFILEHEADER pBmpFileHeader;
extern PBITMAPINFOHEADER pBmpInfoHeader;

extern unsigned char image_in[Y_SIZE][X_SIZE];
extern unsigned char image_out[Y_SIZE][X_SIZE];
extern unsigned char image_work[Y_SIZE][X_SIZE];
extern unsigned char image_hist[HIST_Y_SIZE][HIST_X_SIZE];
extern long	hist[LEVEL];
extern float	ratio[Y_SIZE], size[X_SIZE];


void image_read(unsigned char* image, int xsize, int ysize, char* filename);
void histprint(long hist[]);
void image_copy(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);
void image_copy2(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);
void image_write(unsigned char* image, int xsize, int ysize, char* filename);
void histprint(long hist[]);
void histgram(unsigned char[Y_SIZE][X_SIZE], long hist[]);
void histimage(long hist[], unsigned char image_hist[Y_SIZE][X_SIZE]);
void histsmooth(long hist_in[], long hist_out[]);
void threshold(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int thresh, int mode);
void contraction(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);
void expansion(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);
void thinning(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);

int cconc(int inb[]);
int labeling(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_label[Y_SIZE][X_SIZE], int* cnt);
int labelset(unsigned char image[Y_SIZE][X_SIZE], int xs, int ys, int label);

void features(unsigned char image_work[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int cnt, float size[], float ratio[]);
void ratio_extract(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int cnt, float ratio[], float ratio_min, float ratio_max);
void size_extract(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int cnt, float size[], float size_min, float size_max);
void gradient(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp);
void laplacian(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp);
void template1(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float amp);
void smooth(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int which);
int mean(unsigned char c[]);
int median(unsigned char c[]);
void amplify(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int n);
void range(unsigned char image_in[Y_SIZE][X_SIZE], int* fmax, int* fmin);
void enpand(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int fmax, int  fmin);
void plane(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int hist[]);
void histsmooth2(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], int hist[]);
void sort(unsigned char image_in[Y_SIZE][X_SIZE], struct xyw data[], int level);
void weight(unsigned char image_in[Y_SIZE][X_SIZE], int i, int j, int* wt);
void scale_near(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float zx, float zy);
void scale(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float zx, float zy);
void rotation(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float deg);
void shift(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float px, float py);
void affine(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float deg, float zx, float zy, float px, float py);
void perspect(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], float ax, float xy, float px, float py, float pz, float rz, float rx, float ry, float v, float s);
void masking(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE], unsigned char image_mask[Y_SIZE][X_SIZE]);
//void gray_display(unsigned char image_in[Y_SIZE][X_SIZE], int xsize, int ysize, int xpos, int ypos);
//void graph_clear();
//void palette8();
//void end_set();
int calc_size(unsigned char image_label[Y_SIZE][X_SIZE], int label, int* cx, int* cy);
float calc_length(unsigned char image_label[Y_SIZE][X_SIZE], int label);
float trace(unsigned char image_label[Y_SIZE][X_SIZE], int  xs, int ys);
void matrix(float l[4][4], float m[4][4], float n[4][4]);
void param_pers(float k[], float a, float b, float x0, float y0, float z0, float z, float x, float y, float t, float s);
void fft1core(float a_rl[], float a_im[], int length,int ex, float sin_tbl[], float cos_tbl[], float buf[]);
void cstb(int length, int inv, float sin_tbl[], float cos_tbl[]);
void birv(float a[], int length, int ex, float b[]);
void fft1core(float a_rl[], float a_im[], int length,int ex, float sin_tbl[], float cos_tbl[], float buf[]);
void cstb(int length, int inv, float sin_tbl[], float cos_tbl[]);
void rvmtx1(float a[Y_SIZE][X_SIZE], float b[X_SIZE][Y_SIZE],int xsize, int ysize);
void rvmtx2(float a[X_SIZE][Y_SIZE], float b[Y_SIZE][X_SIZE],int xsize, int ysize);
int fft2(float a_rl[Y_SIZE][X_SIZE], float a_im[Y_SIZE][X_SIZE], int inv);
int fftimage(unsigned char image_in[Y_SIZE][X_SIZE], unsigned char image_out[Y_SIZE][X_SIZE]);
int fft1(float a_rl[], float a_im[], int ex, int inv);

//void piel_write(int x, int y, int pale_no);
//void graph_clear1();
//void outp(int a, int b);