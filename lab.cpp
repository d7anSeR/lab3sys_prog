#include <iostream>
#include <fstream>
#include <vector>
#include<stdio.h>
#include<math.h>
#include<cstdlib>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Структура для передачи данных в поток
struct ThreadData {
    vector<vector<double>>* matrix1;
    vector<vector<double>>* matrix2;
    vector<vector<double>>* result;
    int num_cols;
};
// Функция для вычисления произведения матриц в отдельном потоке
void* multiplyMatrix(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    vector<vector<double>>& matrix1 = *(data->matrix1);
    vector<vector<double>>& matrix2 = *(data->matrix2);
    vector<vector<double>>& result = *(data->result);
    int num_cols = data->num_cols;

    for (size_t i = 0; i < matrix1.size(); ++i) {
        for (size_t j = 0; j < matrix2.size(); ++j) {
            double sum = 0;
            for (size_t k = 0; k < num_cols; ++k) {
                sum += matrix1[i][k] * matrix2[k][j];
            }
            result[i][j] = sum;
        }
    }

    return nullptr;
}
void printMatrix(vector<vector<double>>matrix){
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix.size(); j++) {
            printf("%f ", matrix[i][j]);
        }
        std::cout << endl;
    }
}
vector<vector<double>> mulMatrix(vector<vector<double>> matrix1, vector<vector<double>> matrix2) {
    vector<vector<double>>matrix_new(matrix1.size(), vector<double>(matrix1.size()));
    for (int i = 0; i < matrix1.size(); i++) {
        int count = 0;
        while (count < matrix1.size()) {
            int sum = 0;
            for (int j = 0; j < matrix1.size(); j++) {
                sum += (matrix1[i][j] * matrix2[j][count]);
            }
            matrix_new[i][count] = sum;
            count++;
        }
        
    }
    return matrix_new;
}
vector<vector<double>> readFile(char* filename){
    ifstream fp(filename, ios::binary);
    if(!fp){
        perror("ERROR failed to open file");
    }
    fp.seekg(0, ios::end);
    int rows = fp.tellg();
    fp.seekg(0, ios::beg);
    int cols = rows;
    vector<vector<double>>matrix(10, vector<double>(10));
    vector<char>buffer(rows*cols);
    fp.read(buffer.data(),cols*rows);
    int k = 0;
    rows = cols = 10;
    int i = 0, j = 0;
    for(char c: buffer){
        if(c != ' ' && c != '\n' && atoi(&c) != 0){
            matrix[i][j++] = (double)c - '0';
            if(j == cols){
                i++;
                j = 0;
            }
        }
    }
    cout << endl << filename << ": " << endl;
    printMatrix(matrix);
    fp.close();
    return matrix;
}

int main() {
    vector<vector<double>> matrix1 = readFile("array1");
    vector<vector<double>> matrix2 = readFile("array2");
    auto startSeq = std::chrono::high_resolution_clock::now();
    vector<vector<double>>res1_matrix = mulMatrix(matrix1, matrix2);
    auto endSeq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationSeq = endSeq - startSeq;
    cout << "Sequential execution time: " << durationSeq.count() << " milliseconds" << endl;

    vector<vector<double>> res2_matrix(matrix1.size(), vector<double>(matrix2.size()));

    auto startPar = std::chrono::high_resolution_clock::now();
     // Создание потоков
    const int num_threads = 2; // Количество потоков
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].matrix1 = &matrix1;
        thread_data[i].matrix2 = &matrix2;
        thread_data[i].result = &res2_matrix;
        thread_data[i].num_cols = matrix1.size();
        pthread_create(&threads[i], nullptr, multiplyMatrix, &thread_data[i]);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    auto endPar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationPar = endPar - startPar;
    cout << "Parallel execution time: " << durationPar.count() << " milliseconds" << endl;
    bool equal = true;
    for(int i = 0; i < matrix1.size(); i++){
        for(int j = 0; j < matrix1.size(); j++){
            if(res1_matrix[i][j] != res2_matrix[i][j]){
                equal = false;
            }
        }
    }
    if(equal){
        cout << "Matrices are equal";
    }
    else{
        cout << "Matrices are not equal" << endl;
    }
    return 0;
}