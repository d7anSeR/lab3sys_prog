#include <iostream>
#include <fstream>
#include <vector>
#include<stdio.h>
#include<math.h>
#include<cstdlib>


using namespace std;
void printMatrix(vector<vector<int>>matrix, int rows, int cols){
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j] << " ";
        }
        std::cout << endl;
    }
}
int** mulMatrix(int** matrix1, int ** matrix2, int rows) {
    int **matrix_new = (int**)malloc(rows*sizeof(int*));
    for (int i = 0; i < rows; i++) {
        int count = 0;
        matrix_new[i] = (int*)malloc(rows*sizeof(int));
        while (count < rows) {
            int sum = 0;
            for (int j = 0; j < rows; j++) {
                sum += (matrix1[i][j] * matrix2[j][count]);
            }
            matrix_new[i][count] = sum;
            count++;
        }
        
    }
    //printMatrix(matrix_new);
    cout << "A * B:" << endl;
    return matrix_new;
}
void readFile(char* filename){
    ifstream fp(filename, ios::binary);
    if(!fp){
        perror("ERROR failed to open file");
    }
    fp.seekg(0, ios::end);
    int rows = fp.tellg();
    fp.seekg(0, ios::beg);
    int cols = rows;
    vector<vector<int>>matrix(10, vector<int>(10));
    vector<char>buffer(rows*cols);
    fp.read(buffer.data(),cols*rows);
    int k = 0;
    rows = cols = 10;
    int i = 0, j = 0;
    for(char c: buffer){
        if(c != ' ' && c != '\n' && atoi(&c) != 0){
            matrix[i][j++] = c - '0';
            if(j == cols){
                i++;
                j = 0;
            }
        }
    }
    printMatrix(matrix, cols, rows);
    fp.close();
}
void clearMemory(int** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}
int main() {
    readFile("array1");
    //char **matrix2 = readFile("array2");
    //int **mulmatrix = mulMatrix(matrix1, matrix2, rows);
    return 0;
}