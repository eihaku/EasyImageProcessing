#pragma once
#define X_SIZE      256
#define Y_SIZE      256
#define HIST_X_SIZE X_SIZE
#define HIST_Y_SIZE Y_SIZE
#define HIGH        255
#define LOW         0
#define LEVEL       256


void image_read(unsigned char* image, int xsize, int ysize, char* filename);
void histprint(long hist[]);
void init_set();
void image_copy(unsigned char* image_in, unsigned char* image_out);
void image_copy2(unsigned char* image_in, unsigned char* image_out);

void histprint(long hist[]);
void histgram(unsigned char *image_in, long hist[]);
float histimage(long hist[], unsigned char *image_hist);
void histsmooth(long hist_in[], long hist_out[]);
void threshold(unsigned char *image_in, unsigned char *image_out, int thresh, int mode);
void contraction(unsigned char* image_in, unsigned char* image_out);
void expansion(unsigned char* image_in, unsigned char* image_out);
void thinning(unsigned char* image_in, unsigned char* image_out);

int cconc(int inb[]);
void labeling(unsigned char* image_in, unsigned char* image_label, int* cnt);
int labelset(unsigned char* image, int xs, int ys, int label);

void features(unsigned char* image_work[], unsigned char* image_out[], int cnt, float size[], float ratio[]);
void ratio_extract(unsigned char* image_in[], unsigned char* image_out[], int cnt, float ratio[], float ratio_min, float ratio_max);
void size_extract(unsigned char* image_in[], unsigned char* image_out[], int cnt, float size[], float size_min, float size_max);
void gradient(unsigned char* image_in[], unsigned char* image_out[], float amp);
void laplacian(unsigned char* image_in[], unsigned char* image_out[], float amp);
void template1(unsigned char* image_in[], unsigned char* image_out[], float amp);
void smooth(unsigned char* image_in[], unsigned char* image_out[], int which);
int mean(unsigned char* c[]);
int median(unsigned char* c[]);
void amplify(unsigned char* image_in[], unsigned char* image_out[], int n);
void range(unsigned char* image_in[], int* fmax, int* fmin);
void enpand(unsigned char* image_in[], unsigned char* image_out[], int fmax, int  fmin);
void plane(unsigned char* image_in[], unsigned char* image_out[], int hist[]);
void sort(unsigned char* image_in[], struct xyw data[], int level);
void weight(unsigned char* image_in[], int i, int j, int* wt);
void scale_near(unsigned char* image_in[], unsigned char* image_out[], float zx, float zy);
void scale(unsigned char* image_in[], unsigned char* image_out[], float zx, float zy);
void rotation(unsigned char* image_in[], unsigned char* image_out[], float deg);
void shift(unsigned char* image_in[], unsigned char* image_out[], float px, float py);
void affine(unsigned char* image_in[], unsigned char* image_out[], float deg, float zx, float zy, float px, float py);
void perspect(unsigned char* image_in[], unsigned char* image_out[], float ax, float xy, float px, float py, float pz, float rz, float rx, float ry, float v, float s);
void  masking(unsigned char* image_in[], unsigned char* image_out[], unsigned char image_mask[]);
void text_clear();
void gray_display(unsigned char* image_in[], int xsize, int ysize, int xpos, int ypos);
void graph_clear();
void palette8();
void end_set();
int calc_size(unsigned char* image_label[], int label, int* cx, int* cy);
int calc_length(unsigned char* image_label[], int label);
int trace(unsigned char* image_label[], int  xs, int ys);
void matrix(float* l[], float* m[], float* n[]);
void param_pers(float k[], float a, float b, float x0, float y0, float z0, float z, float x, float y, float t, float s);
void piel_write(int x, int y, int pale_no);
void graph_clear1();
void outp(int a, int b);