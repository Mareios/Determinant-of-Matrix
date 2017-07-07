# Determinant-of-Matrix
Command line application which gives you determinant of random square matrix.

## Calculation of the square matrix determinant

The determinant can be easily calculated by definition or Laplace expansion, but in practice these algorithms are not suitable due to their time complexity. Effective implementations are based on the Gaussian elimination method (LU decomposition). This program is able to work with 1000 x 1000 matrix in a reasonable time (usually a few seconds). Input of the application is square matrix (which will be random generated for you), output gives determinant of this matrix.

## How to run and get determinant of your matrix?

1. In Command line, run main.cpp with two arguments.
2. Choose first argument:
    * **normal** = use for small matrices
    * **threads** = use for large matrices (approx. 500 x 500 and more)
3. Give second argument:
    * Number of columns and rows (because it's square matrix)
4. RUN!
