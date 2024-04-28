import numpy as np

# Генерация случайной матрицы размером 10x10
matrix1 = np.random.randint(1, 10, (10, 10))
matrix2 = np.random.randint(1, 10, (10, 10))
print("Matrix1:\n")
print(matrix1)
print("\nMatrix2:\n")
print(matrix2)
# Сохранение матриц в бинарные файлы
np.savetxt('array1', matrix1, fmt='%.2f')
np.savetxt('array2', matrix2, fmt='%.2f')