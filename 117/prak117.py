import os, sys

# HACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACK
cwd = os.getcwd()
cwd = cwd.rstrip('/1234567890') # remove characters from string
sys.path.insert(0, cwd)
from praklib import *

from math import sqrt

def a_i(L, d): return L/2 - d;

def a_i_sq(L, d): return (L/2 - d)**2

def task1():
    print('-' * 15, "УПРАЖНЕНИЕ 1", '-' * 15)
    I_D = 0; I_TSMALL = 1; I_A = 2; I_T = 3; I_X = 4; I_Y = 5; I_SY = 6;
    print("Введите L, погр. L (м), M, погр. M (кг)")
    L = (float(input()), float(input()))
    M = (float(input()), float(input()))
    print("Введите d0, погрещность d0 (м)")
    d0 = (float(input()), float(input()))
    n = 10
    print("Введите результаты эксперимента в формате 'd t' на одной строке (d: мм, t: с)")
    print("Чтобы посчитать среднее значение времён, введите все значения t после d ('d t1 t2 t3 ...')")
    print("Чтобы закончить ввод, введите букву q")
    data = []
    while (inp := input()) != 'q':
        inpl = inp.split()
        d = float(inpl[0]) / 1000 + d0[0]
        t = avg(list(map(float, inpl[1:])))
        a = a_i(L[0], d)
        T = t/n
        data.append([d, t, a, T, T*T*a, a*a, sigma(a_i_sq, [L[0], d], [L[1], 0.001])])
    print("%-16s" * 7 % ("d(mm)", "t(c)", "a(mm)", "T(c)", "x=T^2 (c^2*mm)", "y=a^2 (mm^2)", "S_y (mm^2)"))
    for i in data:
        l = i.copy()
        l[I_D] *= 1000 # d
        l[I_A] *= 1000 # a
        l[I_X] *= 1000 # T^2 * a
        l[I_Y] *= 1000000 # a^2
        l[I_SY] *= 1000000 # S_{a^2}
        print("%-16.4f"  * 7 % tuple(l))
    with open("output_117_T_a.data", "w") as file:
        for a, b in zip([i[I_T] for i in data], [i[I_A] for i in data]):
            file.write(f"{a} {b}\n")
    X = np.array([i[I_X] for i in data])
    Y = np.array([i[I_Y] for i in data])
    SY = np.array([i[I_SY] for i in data])
    with open("output_117_mnk.data", "w") as file:
        for a, b, c in zip(X, Y, SY):
            file.write(f"{a} {b} {c}\n")
    mnk_res = mnk(X, Y, SY)
    g = (4*PI*PI*mnk_res['a'], 4*PI*PI*mnk_res['sa'])
    a0_mnk = ( sqrt(abs(mnk_res['b'])), mnk_res['sb'] / (2 * sqrt(abs(mnk_res['b']))) )
    a0_teor = (L[0]/sqrt(12), L[1]/sqrt(12))
    T_min = [2*PI*sqrt(2*a0_mnk[0]/g[0]), 0]
    T_min[1] = 2 * sqrt(2) * PI * T_min[0] * sqrt((a0_mnk[1]/a0_mnk[0])**2 + (g[1]/g[0])**2) # погрешность по формуле для умножения
    print(f"A = {mnk_res['a']}±{mnk_res['sa']}")
    print(f"B = {mnk_res['b']}±{mnk_res['sb']}")
    print(f"g = {g[0]}±{g[1]}")
    print(f"a0_граф = ? (см. график output_117_T_a.png)")
    print(f"a0_мнк = {a0_mnk[0]}±{a0_mnk[1]}")
    print(f"a0_теор = {a0_teor[0]}±{a0_teor[1]}")
    print(f"T_min_граф = ? (см. график output_117_mnk.png)")
    print(f"T_min_теор = {T_min[0]}±{T_min[1]}")
    print(
f"""{'#' * 26} !!!ВНИМАНИЕ!!! {'#' * 26}
#    Пункт 8 автоматизировать будет очень сложно, в связи с чем    #
# предоставляется читателю (и, увы, автору) на ручную обработку... #
{'#' * 68}"""
    )

def g(l, T): return 4 * PI * PI * l / T / T

def task2():
    n = 10
    print('-' * 15, "УПРАЖНЕНИЕ 1", '-' * 15)
    print("Так, ну поделить на 10 вручную легче, чем вводить это в комп а потом переписывать")
    print("так что периоды посчитаете сами")
    print("Введите t_01 и t_02 на одной строке через пробел")
    T0 = avg(list(map(lambda x: float(x)/n, input().split())))
    T_sigma = float(input("Введите погрешность одного измерения периода (sigma_суб.):\n"))
    print("Введите L_пр и погрешность на одной строке (м)")
    l_pr = [*map(float, input().split())]
    args = [l_pr[0], T0]
    sigmas = [l_pr[1], T_sigma / sqrt(2) / sqrt(3)] # корни от 2 расчётов среднего
    g_ = (g(*args), sigma(g, args, sigmas))
    print(f"T0 = {T0}±{sigmas[1]}\ng = {g_[0]}±{g_[1]}")

if __name__ == '__main__':
    task1()
    task2()
