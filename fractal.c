#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "bmp.h"

//Image Size
int WIDTH;
int HEIGHT;
//Enable percent done output on console
#define PERCENT_DONE_OUT
//Write sample images every 100 pixels
//#define WRITE_PARTIAL_IMAGES


#define RENDER_AREA 1.5

#define STEP_DIST 0.001
#define MAX_STEPS 3000

#define NORMAL_DIST 0.0001
//Offset for triangle to calculate normal
#define NORMAL_OFFSET 0.005
#define NORMAL_STEPS 500
//How many steps (STEP_DIST) to back up before shooting normal rays
#define NORMAL_BACK_UP_STEPS 10

unsigned char *image;

//A julia set generator
unsigned char julia_point(double zx, double zy, double cx, double cy, unsigned int iterations){
	unsigned int n;
  double tempX;
  for(n = 0; n < iterations; n++){
    tempX = zx;
    zx = (zx*zx)-(zy*zy) + cx;
    zy = (2.0*tempX*zy) + cy;
    if(sqrt((zx*zx) + (zy*zy)) > 4.0){
      return 0;
    }
  }
	return 1;
}

//c value for quaternions
// -1,0.2,0,0
double qcr = -1.0;
double qca = 0.2;
double qcb = 0.0;
double qcc = 0.0;

//A quaternions julia generator
//Quaternion (zr + za*i + zb*j + zc*k)
unsigned char quat_julia_point(double zr, double za, double zb, double zc, unsigned int iterations){
	unsigned int n;
	double r, a, b, c;
	for(n = 0; n < iterations; n++){
		r = zr; a = za; b = zb; c = zc;
		//Quaternion multiplication           and addition of c 
		zr = ( r * r - a * a - b * b - c * c ) + qcr;
		za = ( r * a + a * r + b * c - c * b ) + qca;
		zb = ( r * b + b * r + c * a - a * c ) + qcb;
		zc = ( r * c + c * r + a * b - b * a ) + qcc; 
		if(sqrt(zr*zr + za*za + zb*zb + zc*zc) > 4.0){
      return 0;
    }
	}
	return 1;
}

//March a ray until intersection
unsigned char ray_march(double *x, double *y, double *z, double dx, double dy, double dz, double slice, unsigned int maxSteps, unsigned int iters){
	//If the ray has hit the area to check for collisions or not
	unsigned char hitRenderArea = 0;
	//ray march
	for(unsigned int i = 0; i < maxSteps; i++){
		*x += dx;
		*y += dy;
		*z += dz;
		if(*x < -RENDER_AREA || *x > RENDER_AREA || *y < -RENDER_AREA || *y > RENDER_AREA || *z < -RENDER_AREA || *z > RENDER_AREA){
			//A ray that entered and exited the rendering area with no coliision will not reenter
			if(hitRenderArea){return 0;}																																																										 																																																												 
			//If we aren't in the rendering area, no need to check for collision
			continue;
		}
		hitRenderArea = 1;
		if(quat_julia_point(*x, *y, *z, slice, iters)){
			return 1;
		}
	}
	return 0;
}

//Normalize a vector
void normalize(double *x, double *y, double *z){
	double dist = sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));
	*x /= dist;
	*y /= dist;
	*z /= dist;
}

//Get the normal at an intersection point
void get_normal(double x, double y, double z, double dx, double dy, double dz, double slice, unsigned int iters, double *nx, double *ny, double *nz){
	//Points to shoot normals from
	//First point - just backed up
	double n1x, n1y, n1z;
	//Second point
	double n2x, n2y, n2z;
	//Third point
	double n3x, n3y, n3z;
	//n2 and n3 offsets from n1
	double on2x, on2y, on2z;
	double on3x, on3y, on3z;
	//vectors of the edges of the triangle on the surface
	double t1x, t1y, t1z;
	double t2x, t2y, t2z;
	
	//Back up from intersection to get n1
	n1x = x - (dx*NORMAL_BACK_UP_STEPS);
	n1y = y - (dy*NORMAL_BACK_UP_STEPS);
	n1z = z - (dz*NORMAL_BACK_UP_STEPS);
	
	//Find a on2 perpindicular to d
	on2x = 1.0;
	on2y = 1.0;
	on2z = (-dx-dy)/(dz);
	normalize(&on2x, &on2y, &on2z);
	
	//Find a on3 perpindiculat to d and on2 - normalized cross product
	on3x = (dy*on2z) - (dz*on2y);
	on3y = (dz*on2x) - (dx*on2z);
	on3z = (dx*on2y) - (dy*on2x);
	normalize(&on3x, &on3y, &on3z);
	
	//Multiply on2 and on3 by NORMAL_OFFSET
	on2x *= NORMAL_OFFSET;
	on2y *= NORMAL_OFFSET;
	on2z *= NORMAL_OFFSET;
	
	on3x *= NORMAL_OFFSET;
	on3y *= NORMAL_OFFSET;
	on3z *= NORMAL_OFFSET;
	
	//Create a n2 and n3 by adding offsets to n1
	n2x = n1x + on2x;
	n2y = n1y + on2y;
	n2z = n1z + on2z;
	
	n3x = n1x + on3x;
	n3y = n1y + on3y;
	n3z = n1z + on3z;
	
	//normalize d, then multiply by NORMAL_DIST
	normalize(&dx, &dy, &dz);
	dx*=NORMAL_DIST;
	dy*=NORMAL_DIST;
	dz*=NORMAL_DIST;
	
	//Shoot rays
	ray_march(&n1x, &n1y, &n1z, dx, dy, dz, slice, NORMAL_STEPS, iters);
	ray_march(&n2x, &n2y, &n2z, dx, dy, dz, slice, NORMAL_STEPS, iters);
	ray_march(&n3x, &n3y, &n3z, dx, dy, dz, slice, NORMAL_STEPS, iters);
	//n1, n2, and n3 now form a triangle on the surface of the volume - set t1 and t2 equal to the vectors from n1 to n2 and n1 to n3
	t1x = n2x - n1x;
	t1y = n2y - n1y;
	t1z = n2z - n1z;
	
	t2x = n3x - n1x;
	t2y = n3y - n1y;
	t2z = n3z - n1z;
	
	
	//set normal equal to dot product of t1 and t2
	*nx = (t1y*t2z) - (t1z*t2y);
	*ny = (t1z*t2x) - (t1x*t2z);
	*nz = (t1x*t2y) - (t1y*t2x);
	//normalize normal
	normalize(nx, ny, nz);
	
}

//Get color for a specific pixel
uint32_t get_color(unsigned int x, unsigned int y, unsigned int width, unsigned int height, double rotate){
	double zx, zy, zz;
	double slice;
	unsigned int iters;
	unsigned int maxSteps;
	double stepDist;
	double dx, dy, dz;
	//init c
	slice = 0.0;
	
	//starting point
	zx = 0.0;
	zy = 0.0;
	zz = -2.0;
	
	//rotate starting point
	double theta = atan2(zz, zx);
	double mag = sqrt(zx*zx+zz*zz);
	
	theta += rotate;
	
	zx = cos(theta) * mag;
	zz = sin(theta) * mag;
	
	//set dx, dy, and dz
	dz = 1;
	//90 degree fov, should be setable
	dx = ((double)x)/((double)width)*2.0-1.0;
	dy = ((double)y)/((double)height)*2.0-1.0;

	//rotate direction
	//rotate starting point
	
	theta = atan2(dz, dx);
	mag = sqrt(dx*dx+dz*dz);
	
	theta += rotate;
	
	dx = cos(theta) * mag;
	dz = sin(theta) * mag;
	
	//set parameters
	stepDist = STEP_DIST;
	maxSteps = MAX_STEPS;
	iters = 200;
	
	//normalize d, and divide by stepDist
	double dist = sqrt(dx*dx + dy*dy + dz*dz);
	dx/=(dist/stepDist);
	dy/=(dist/stepDist);
	dz/=(dist/stepDist);
	
	double nx = 0.0;
	double ny = 0.0;
	double nz = 0.0;
	
	if(ray_march(&zx, &zy, &zz, dx, dy, dz, slice, maxSteps, iters)){
		get_normal (zx, zy, zz, dx, dy, dz, slice, iters, &nx, &ny, &nz);
		unsigned char red = (unsigned char)((nx+1) * (128.0));
		unsigned char green = (unsigned char)((ny+1) * (128.0));
		unsigned char blue = (unsigned char)((nz+1) * (128.0));
		return (uint32_t)((red << 16) + (green << 8) + blue);
	}
	return 0x00ffffff;
}
void render_image(char * filename, double rotate){
	//X and Y pixel locations
	unsigned int pixelX;
	unsigned int pixelY;
	//Image width and height
	unsigned int width;
	unsigned int height;
	//returned packed pixel color
	uint32_t color;
	
	//set width and height
	width = WIDTH;
	height = HEIGHT;

	
	//iterate through each pixel
	for(pixelX = 0; pixelX < width; pixelX++){
#ifdef PERCENT_DONE_OUT
		printf("\e7%.1f\x25\e8", ((double)pixelX/(double)width)*100);
#endif
#ifdef WRITE_PARTIAL_IMAGES
		if(pixelX % 100 == 99){
			generateBitmapImage((unsigned char *)image, height, width, filename);
		}
#endif
		for(pixelY = 0; pixelY < height; pixelY++){
			color = get_color(pixelX, pixelY, width, height, rotate);
			image[pixelY*WIDTH*3 + pixelX*3 + 2] = (unsigned char)((color >> 16)&0xff);
			image[pixelY*WIDTH*3 + pixelX*3 + 1] = (unsigned char)((color >> 8)&0xff);
			image[pixelY*WIDTH*3 + pixelX*3 + 0] = (unsigned char)(color & 0xff);
		}
	}												 
	//write image
	generateBitmapImage((unsigned char *)image, height, width, filename);
}

int main(int argc, char **argv){
	setbuf(stdout, NULL);
	if(argc < 3 || argc > 3) {
		printf("Not enough params, correct usage: ./fractal WIDTH HEIGHT\n");
		return 1;
	}
	WIDTH = atoi(argv[1]);
	HEIGHT = atoi(argv[2]);

	int i,j;
	image = (unsigned char *) malloc(HEIGHT * WIDTH * 3 * sizeof(char));
	render_image("out.bmp", 0.1);
	return 0;
}
