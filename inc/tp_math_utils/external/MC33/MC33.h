/*
  File: MC33.h
  Programmed by: David Vega - dvega@uc.edu.ve
  version: 5.2
  March 2022
  This library is the C++ version of the library described in the paper:
  Vega, D., Abache, J., Coll, D., A Fast and Memory Saving Marching Cubes 33
  implementation with the correct interior test, Journal of Computer Graphics
  Techniques (JCGT), vol. 8, no. 3, 1_17, 2019.
*/

#ifndef MC33_h_
#define MC33_h_

#include "tp_math_utils/Geometry3D.h"

#include <vector>
#include <functional>

namespace tp_math_utils
{
class Grid3DBase;

/********************************** USAGE ************************************/
/*
//1. Header
#include <MC33.h>

//2. Read a grid file.
  grid3D G;
  G.read_dat_file("filename.dat");

//3. create a MC33 object and assign it the grid3D.
  MC33 MC;
  MC.set_grid3d(G); // or MC.set_grid3d(Z);

//4. calculate an isosurface.
  surface S;
  MC.calculate_isosurface(S, isovalue);

*/


/*
The class grid3d contains a 3D matrix (F[][][]) that stores the values of a function
evaluated at points of a 3D regularly spaced grid. N[] is the number of intervals in
each dimension. The grid contains (N[2] + 1)*(N[1] + 1)*(N[0] + 1) points. L[] is the
grid size in each dimension. r0[] are the coordinates of the first grid point. d[]
is the distance between adjacent points in each dimension (can be different for each
dimension), nonortho has a value of 1 when the grid is inclined else 0. _A and A_
are the matrices that transform from inclined to orthogonal coordinates and vice
versa, respectively. If the grid is periodic (is infinitely repeated along each
dimension) the flag periodic must be different from 0.
*/
class grid3d_ {
private:
  float ***F;
  int x_data;
  // if x_data is negative, the data of grid are external an they cannot be modified using
  // the member functions of this class. If it is positive, the grid data is managed by the
  // functions of this class. In the subgrids x_data is equal to the inner index step.

  int alloc_F(); // allocates memory for the grid data
  void free_F(); // release the allocated memory
public:
//Get pointers to some class members:

  const size_t* get_N();  // Grid dimensions Nx,Ny,Nz
  const float* get_L();
  const double* get_r0();
  const double* get_d();
  const float* get_Ang();
  const double (*get__A())[3];
  const double (*get_A_())[3];
  int isnotorthogonal(); // returns nonortho value

//Generates an orthogonal grid from a function fn(x,y,z). xi and xf are the
//limits of the interval along the x axis, yi and yf along the y axis and zi
//and zf along the z axis. dx, dy and dz are the respective step sizes.
  int generate_grid_from_fn(double xi, double yi, double zi, double xf, double yf, double zf, double dx, double dy, double dz, double (*fn)(double x, double y, double z));


//Modifying the grid parameters:
  /******************************************************************
  set_grid_dimensions sets the new dimensions of a grid data. It overrides the effect of
  the other functions that modify the grid parameters. Nx, Ny and Nz are the number of
  grid points in each dimension. It returns 0 if memory allocation was successful.*/
  int set_grid_dimensions(size_t Nx, size_t Ny, size_t Nz);
  void set_grid_value(size_t i, size_t j, size_t k, float value); // set a grid point value
  void set_ratio_aspect(double rx, double ry, double rz); // modifies d and L
  void set_r0(double x, double y, double z); // modifies r0
  void set_Ang(float angle_bc, float angle_ca, float angle_ab); // modifies Ang

  /******************************************************************
  set_data_pointer creates internal pointers that point to the external data array. data
  must be stored with the nested inner loop running from i = 0 to Nx - 1 and the outer
  loop from k = 0 to Nz - 1. The data content cannot be modified using class functions.
  It returns 0 if memory allocation of internal pointers was successful.*/
  int set_data_pointer(size_t Nx, size_t Ny, size_t Nz, float* data);

// Managing subgrids:
  /******************************************************************
  The grid can contain several subgrids that use the same data. The add_subgrid function
  make a subgrid and stores it in an internal array (the subgrids do not contain data,
  only pointers to the main grid data). The input parameters of the function are the three
  indices of the origin grid point, the number of points in each dimension and the index
  step in each dimension. The function returns 0 if the subgrid was successfully added,
  -1 for wrong input parameters and -2 for error in memory allocation. */
  int add_subgrid(size_t Oi, size_t Oj, size_t Ok,
          size_t Ni, size_t Nj, size_t Nk,
          size_t Si, size_t Sj, size_t Sk);
  void clear_subgrid(); // erase all subgrids
//  grid3d *get_subgrid(size_t i); // returns a pointer to the subgrid i
  void del_subgrid(size_t i); // delete the subgrid i
  size_t subgrid_size(); // returns the number of subgrids

//Reading grid data from files:
  /******************************************************************
  The functions returns zero when succeeds. If the file could not be opened or the data
  does not match the format, the return value is -1. If a memory error occurred, the
  return value is -2. A return value of -4 means that the data read may be incomplete. */
  /******************************************************************
  read_grd reads a *.grd file from the DMol3 program.*/
  int read_grd(const char *filename);

  /** read_grd_binary reads a file with an internal binary format */
  int read_grd_binary(const char *filename);
  /******************************************************************
  read_scanfiles reads a set of files that contain a slab of res*res scan data points,
  the data points are read as unsigned short int (if order is different from 0, the
  bytes of the unsigned short are exchanged). The filename must end with a number, and
  the function reads all files with end number greater or equal to filename.
  (Some datasets: http://www.graphics.stanford.edu/data/voldata/voldata.html)*/
  int read_scanfiles(const char *filename, size_t res, int order);

  /******************************************************************
  read_raw_file reads a file that contains integer (8, 16 or 32 bits) or float (or double)
  data points. byte is the number of bytes of the integer (1, 2 or 4), or of the float (4
  or 8). If the data is big endian, byte must be negative (only for integers). The vector
  n[3] contains the number of points in each dimension. The size of file must be
  abs(byte)*n[0]*n[1]*n[2].*/
  int read_raw_file(const char *filename, size_t *n, int byte, int isfloat = 0);

  /******************************************************************
  read_dat_file reads a dat file. The function returns 0 when succeeds.
  http://www.cg.tuwien.ac.at/research/vis/datasets/*/
  int read_dat_file(const char *filename);

  /******************************************************************
  save the grid points values in a raw data file of float (float or double). The
  function returns 0 when succeeds.*/
  int save_raw_file(const char *filename);

  grid3d_();
  // Copy constructor
  ~grid3d_();
};

//template <class T>
//struct MC33_v3 {
//  T v[3];
//};

///* The class surface contains the data of a isosurface. The isovalue is iso, the
//number of surface points is nV, and the number of triangles is nT. The vector V
//contains the vertex coordinates, the vector T contains the triangle indices, The
//vector N contains the normal coordinates (one normal for each vertex), and the
//color vector contains the color index of each point.*/
//class surface {
//private:
//  size_t nV, nT;
//  std::vector<MC33_v3<size_t>> T;
//  std::vector<MC33_v3<float>> V;
//  std::vector<MC33_v3<float>> N;
//  std::vector<int> color;
//  float iso;
//public:
//  union {
//    long long size_t ul;
//    int i[3];
//    char c[12];
//    float f[3];
//    double df;
//  } user; // user data
//  float get_isovalue(); // returns the isovalue
//  size_t get_num_vertices(); // gets the number of vertices
//  size_t get_num_triangles(); // gets the number of triangles
//  const size_t *getTriangle(size_t n); // gets a pointer to indices of triangle n
//  const float *getVertex(size_t n); // gets a pointer to coordinates of vertex n
//  const float *getNormal(size_t n); // gets a pointer to the normal vector n
//  void flipNormals(); // reverses the direction of all normals
//  void flipTriangles(); // toggle CW / CCW vertex order in triangles
//  const unsigned char *getColor(size_t n); // gets a pointer to the color of vertex n
//  void setColor(size_t n, unsigned char *pcolor);

//  /******************************************************************
//  Saves all the surface *S data (in binary format) to a "filename" file. The
//  return value is 0 if the call succeeds, else -1.*/
//  int save_bin(const char *filename);

//  /******************************************************************
//  Saves all the surface *S data (in plain text format) to a "filename" file.
//  The return value is 0 if the call succeeds, else -1.*/
//  int save_txt(const char *filename);

//  /******************************************************************
//  Reads (from a "filename" file) the surface data stored in binary format.
//  The return value is 0 if the call succeeds, else -1.*/
//  int read_bin(const char *filename);

//  /* Draw the surface */
//  void draw();

//  /* Draw some points of the surface */
//  void drawdraft();

//  /* Clear all vector data */
//  void clear();

//  /* Correct the data vector sizes */
//  void adjustvectorlenght();

//  surface();
//};

//##################################################################################################
//! Marching cubes 33 class.
/*!
The function member set_grid3d must be called once before calculate an isosurface.
If the grid3d object is modified or you want to use another grid3d object with the
same MC33 object, this function must be called again.
The function calculate_isosurface fill the surface object with the isosurface data.
*/
class MC33
{
  TP_DQ;
public:
  //################################################################################################
  MC33();

  //################################################################################################
  ~MC33();

  //################################################################################################
  int setGrid3D(Grid3DBase* grid);

  //################################################################################################
  //! Calculate the isosurface with isovalue iso and store the data in the surface Sf.
  bool calculateIsosurface(Geometry3D& geometry, float iso);
};

}

#endif
