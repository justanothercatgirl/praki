import os, sys

# HACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACK
cwd = os.getcwd()
cwd = cwd.rstrip('/1234567890') # remove characters from string
sys.path.insert(0, cwd)
from praklib import *

from math import sqrt
from random import randint

PI = 3.141592657 # :)))))))))))))))))

def E(m, g, l, a, D, A, b):
    return (m * g * 4 * l * a) / (PI * D * D * A * b)

def _E(m, g, l, a, D, dN, b):
    return (m * g * 4 * l * a) / (PI * D * D * dN * b)

def main():
    AVGDIAM = 0
    DEVDIAM = 1
    D_NS = 2
    STDDEV_NS = 3
    print("3 значения толщины для стали: ")
    steel = [float(input()) for i in range(3)]
    print("3 значения толщины для меди: ")
    copper = [float(input()) for i in range(3)]
    print("3 значения толщины для дюраля: ")
    dural = [float(input()) for i in range(3)]
    l = (0.95, 0.005)
    a = (0.1065, 0.001)
    b = (0.0400, 0.001)
    m0 = (0.2127, 0.0003)
    materials = {
            "сталь":    [avg(steel), stddev(steel)],
            "медь":     [avg(copper), stddev(copper)],
            "дюраль":   [avg(dural), stddev(dural)]
    }
    for i in materials.keys():
        print("Материал:", i)
        d_ns = []
        nups = ndowns = []
        for j in range(10):
            print(f"delta N_{j} (добавление грузов): ")
            up = float(input())
            print(f"delta N_{j} (убирание грузов): ")
            down = float(input())
            nups.append(up)
            ndowns.append(down)
            d_ns.append([j, (up + down) / 2]);
        # формула: корень(сумма( (n_вверх - n_сред)^2 + (n_вниз - n_сред)^2 ) / (20*19) ) для каждого n = [0..10]
        sigma_ns = sqrt(                                                                                \
                sum(( (avg-u)**2 + (avg-d)**2 for u, d, (_, avg) in zip(nups, ndowns, d_ns) ))    \
                / (2*len(nups)) / (2*len(nups)-1)                                                       \
            ); # не 1 в 1 как сказано в задании, но по факту лучше
        materials[i].append(d_ns)
        materials[i].append([sigma_ns for _ in range(len(d_ns))])
        with open(f"output_120_{i}.data", "w+") as file:
            for (n, dn), s in zip(materials[i][D_NS], materials[i][STDDEV_NS]):
                file.write(f"{n} {dn} {s}\n") # я не знаю, работает ли write с любым кол-вом аргументов и документация питона говно
        
        mnk_res = mnk(np.array([i[0] for i in d_ns], dtype=float), np.array([i[1] for i in d_ns], dtype=float), np.array(materials[i][STDDEV_NS]))
        args = [m0[0], 9.807, l[0], a[0], materials[i][AVGDIAM], mnk_res['a'], b[0]] 
        sigmas = [m0[1], 0.01, l[1], a[1], materials[i][DEVDIAM], mnk_res['sa'], b[1]]
        print('-' * 10, ' ', i.upper(), ' ', '-' * 10);
        print(f"D = {materials[i][AVGDIAM]}")
        print('\n'.join([f"delta_N_{i[0]}: {i[1]}" for i in d_ns]))
        print(f"Погрешность N: {sigma_ns}")
        print(f"результат МНК:\n\tA={mnk_res['a']}±{mnk_res['sa']}\n\tB={mnk_res['b']}±{mnk_res['sb']}")
        print(f"E: E={E(*args)}±{sigma(E, args, sigmas)}")
        index1 = randint(1, 9)
        args[0] = m0[0] * d_ns[index1][0]
        args[5] = d_ns[index1][1]
        sigmas[0] = m0[1] * index1
        print(f"для i={index1}: E={_E(*args)}±{sigma(_E, args, sigmas)}")
        index2 = index1
        while (index2 == index1): index2 = randint(1, 9);
        args[0] = m0[0] * d_ns[index2][0]
        args[5] = d_ns[index2][1]
        sigmas[0] = m0[1] * index1
        print(f"для i={index2}: E={_E(*args)}±{sigma(_E, args, sigmas)}")


if __name__ == "__main__":
    main()
