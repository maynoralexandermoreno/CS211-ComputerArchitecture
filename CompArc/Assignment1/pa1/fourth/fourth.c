#include <stdlib.h>
#include <stdio.h>

void freeMatrix(double** A, int row, int col); 
void printMatrix(double** matrix, int row, int col);

int main(int argc, char** argv){
  int i, j, k;
  k = 0;
  if (k == 0 ){

  }
  //-------------check number of arguments-----------------------
  if(argc != 3){
    return 0;
  }
  if (argv[1] == NULL){
    return 0;
  }
  else if (argv[2] == NULL){
    return 0;
  }
  //-----------read trainin file----------------------------
  FILE* fp1 = fopen(argv[1], "r");
  if (fp1 == NULL){
    return 0;
  }
  int row, col, attributes;
  fscanf(fp1,"%d\n", &attributes);
  col  = attributes + 1;
  fscanf(fp1,"%d\n", &row);

  //-------------- read test file; test part only has input for the rows, cols = attributes  ------------------
  FILE* fp2 = fopen(argv[2], "r");
  if (fp2 == NULL){
    return 0;
  }
  int row2;
  fscanf(fp2,"%d\n", &row2); 

 

  //--------------create training and test matrix from the file----------------
  double num;

  double ** X = (double**)malloc(sizeof(double*) * row);
  for (i = 0; i < row; i++) {
    X[i] = (double*)malloc(sizeof(double) * col);
  }
  double ** testX = (double**)malloc(sizeof(double*) * row2);
  for (i = 0; i < row2; i++){
    testX[i] = (double*)malloc(sizeof(double) * attributes);
  }
  double ** Y = (double**)malloc(sizeof(double*) * row);
  for (i = 0; i < row; i++){
    Y[i] = (double*)malloc(sizeof(double) * 1);
  }

  //-------The first column of the training matrix will get placed in Y, the first column of X will be 1; the other columns go into X----
  //------close the training file when done------------------
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      fscanf(fp1, "%lf%*[,]", &num);
      if(j == 0){
	Y[i][0] = num;
	X[i][0] = 1.0;
      }
      else{
	X[i][j] = num;
      }
    }
    fscanf(fp1, "\n");
  }
  fclose(fp1);

  //-------populate testX with the test file, then close test file-------------------
  for (i = 0; i < row2; i++) {
    for (j = 0; j < attributes; j++) {
      fscanf(fp2, "%lf%*[,]", &num);
      testX[i][j] = num;
    }
    fscanf(fp1, "\n");
  }
  fclose(fp2);

  //--------------solves for W----------------------------------
  
  double ** T = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++) {
    T[i] = (double*)malloc(sizeof(double) * row);
  }
  double ** TX = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    TX[i] = (double*)malloc(sizeof(double) * col);
  }
  double ** I = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    I[i] = (double*)malloc(sizeof(double) * col);
  }
  double ** Aug = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    Aug[i] = (double*)malloc(sizeof(double) * (col*2));
  }
  double ** TXI = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    TXI[i] = (double*)malloc(sizeof(double) * col);
  }
  double ** TXIT = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    TXIT[i] = (double*)malloc(sizeof(double) * row);
  }
  double ** W = (double**)malloc(sizeof(double*) * col);
  for (i = 0; i < col; i++){
    W[i] = (double*)malloc(sizeof(double) * 1);
  }
  double ** F = (double**)malloc(sizeof(double*) * row2);
  for (i = 0; i < row2; i++){
    F[i] = (double*)malloc(sizeof(double) * 1);
  }

  //----------transpose X, save in T------------------
  for(i = 0; i < row; i++){
    for(j = 0; j < col; j++){
      T[j][i] = X[i][j];
    }
  }

  //--------------- multiply T and X and place in corresponding element of TX-------------------
  for (i = 0; i < col; i++){
    for(j = 0; j < col; j++){
      for (k = 0; k < row; k++){
	TX[i][j] += T[i][k] * X[k][j];
      }
    }
  }

  //----------------inverse T----------------------
  // Identity matrixes are square
  // Identity matrixes have 1's where the row = column
  for(i = 0; i < col; i++){
    I[i][i] = 1.0;
  }

  // Augmented matrix is composed of the first half being A and second being I, it is going to have 2x rows than columns;
  for(i = 0; i < col; i++){
    for(j = 0; j < col; j++){
      Aug[i][j] = TX[i][j];
    } 
  }
  for(i = 0; i < col; i++){
    for(j=col; j < (col*2); j++){
      Aug[i][j] = I[i][j-col];
    }
  }
  
  //   Algorith idea; wherever Aaug[i][i] is, that is where we want our leading coefficients;
  //   So skip those positions. Any other row above or below (where col# != row#) can be zeroed
  //   to zero, the leading coefficient is Aaug[i][j]/Aaug[j][j]. Every element in that row can
  //   now be replaced with initial element of row i - coefficient * initial element of row j.
  //   this has the effect of zeroing out all columns above and below Aaug[j][j]. Repeat this for
  //   every column up to where col = row, and boom, done
  //
  // This would turn the matrix into RREF form (makes the leading coefficient 1); /

  for(j=0; j < col; j++){
    double coef;
    for(i = 0; i < col; i++){
      if (i != j){
	coef = Aug[i][j]/Aug[j][j];
      	for(k = 0; k < col*2; k++){
	  Aug[i][k] = Aug[i][k] - coef* Aug[j][k];
	}
      }
    }
    coef = Aug[j][j];
    for(k = 0; k < col*2; k++){
      Aug[j][k] = Aug[j][k]/ coef; 
    }
  }

  // Now the first half is an indentity matrix, last half has our inverted matrix; Place Inverted TX into TXI
  for(i = 0; i < col; i++){
    for(j = col; j < (col*2); j++){
      TXI[i][j-col] = Aug[i][j];
    }
  }
  //--------------- multiply TXI and T-------------------
  for (i = 0; i < col; i++){
    for(j = 0; j < row; j++){
      for (k = 0; k < col; k++){
	TXIT[i][j] += TXI[i][k] * T[k][j];
      }
    }
  }
 
  
  //----------------- multiply TXIT * Y to get W -----------------------
  for (i = 0; i < col; i++){
    for (k = 0; k < row; k++){
      W[i][0] += TXIT[i][k] * Y[k][0];
    }
  }

  //---------------------multiply testXW to get F(inally)  -----------------------
  for (i = 0; i < row2; i++){
    double temp = W[0][0];
    for(j = 0; j < attributes; j++){
      temp += testX[i][j]*W[j+1][0];
    }
    printf("%0.f\n", temp);
  }

  //--------------------free all the matrixes --------------------------
  freeMatrix(X, row, col);
  freeMatrix(Y, row, 1);
  freeMatrix(T, col, row);
  freeMatrix(TX, col, col);
  freeMatrix(I, col, col);
  freeMatrix(Aug, col, (col*2));
  freeMatrix(TXI, col, col);
  freeMatrix(TXIT, col, row);
  freeMatrix(W, col, 1);
  freeMatrix(testX, row2, attributes);
  freeMatrix(F, row2, 1);

  return 0;
}

//--------------------freeMatrix ------------------------
void freeMatrix(double** A, int row, int col){
  int i;
  for (i=0; i<row; i++){
    free(A[i]);
  }
  free(A);
}

//--------------------printMatrix----------------------------
void printMatrix(double** matrix, int row, int col) {
  int i, j;
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      printf("%lf ", matrix[i][j]);
     }
  printf("\n");
 }
}

