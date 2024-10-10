#!/home/main/coding/py/venv/bin/python3

from prak2 import *
from math import pow, sqrt 


PI = 3.141592657 # ;)

def main():
    timer = [];
    photo = [];
    Ns = [];
    Tsqs = [];
    STsqs = [];
    H = float(input('Введите H и погрешность H: '))
    H_err = float(input())
    m0 = int(input('Введите m0 (количество измерений): '))
    n = float(input('Введите n (количество колебаний на 1 измерение): '))
    subj_err = 0.3

    print('\n', '-' * 35, "\nОБРАБОТКА С РУЧНЫМИ ЧАСАМИ\n", '-' * 35);
    for i in range(m0 + 1):
        print("введите t_1, t_2, t_3 для N  = ", i);
        list = [float(input()) for _ in range(3)];
        mean = avg(list);
        dev = stddev(list);
        mean_err = sqrt(dev * dev + pow(1e-3 * mean, 2) + subj_err*subj_err);
        T = mean / n;
        T_err = mean_err / n;
        tsq = T * T;
        tsq_err = 2 * T * T_err;
        timer.append((i, tsq, tsq_err));
        print(f"avg = {mean}; err = {mean_err}; T = {T} ± {T_err}\nT^2 = {tsq} ± {tsq_err}");
    Ns = np.array([i[0] for i in timer]);
    Tsqs = np.array([i[1] for i in timer]);
    STsqs = np.array([i[2] for i in timer]);
    res = mnk(Ns, Tsqs, STsqs);
    print(f"A = {res['a']} ± {res['sa']}\nB = {res['b']} ± {res['sb']}");
    g = 4 * PI * PI * H / res['a'] / m0;
    g_err = g * sqrt(pow(res['sa'] / res['a'], 2) + pow(H_err / H, 2));
    l = res['b'] * g / 4 / PI / PI;
    l_err = l * sqrt(pow(res['sb'] / res['b'], 2) + pow(g_err/g , 2));
    print(f"g = {g} ± {g_err}\nl = {l} ± {l_err}");


    print('\n', '-' * 35, "\nОБРАБОТКА С ФОТОДАТЧИКАМИ\n", '-' * 35);
    for i in range(m0 + 1):
        print("Введите t, St для N = ", i);
        mean = float(input())
        dev = float(input())
        mean_err = sqrt(dev * dev + pow(1e-3 * mean, 2));
        T = 2 * mean;
        T_err = 2 * mean_err;
        tsq = T * T;
        tsq_err = 2 * T * T_err;
        photo.append((i, tsq, tsq_err));
        print(f"T = {T} ± {T_err}\nT^2 = {tsq} ± {tsq_err}");
    Ns = np.array([i[0] for i in photo]);
    Tsqs = np.array([i[1] for i in photo]);
    STsqs = np.array([i[2] for i in photo]);
    res = mnk(Ns, Tsqs, STsqs);
    print(f"A = {res['a']} ± {res['sa']}\nB = {res['b']} ± {res['sb']}");
    g = 4 * PI * PI * H / res['a'] / m0;
    g_err = g * sqrt(pow(res['sa'] / res['a'], 2) + pow(H_err / H, 2));
    l = res['b'] * g / 4 / PI / PI;
    l_err = l * sqrt(pow(res['sb'] / res['b'], 2) + pow(g_err/g , 2));
    print(f"g = {g} ± {g_err}\nl = {l} ± {l_err}");
    
    print('\n', '-' * 35, "\nОБРАБОТКА ВТОРОГО УПРАЖНЕНИЯ\n", '-' * 35);
    omegas = [];
    Us = [0.3 * (i+1) for i in range(6)];
    U_errs = [0.003 * i + 0.002 for i in Us];
    U_errs[0] -= 0.0015;
    for U, U_err in zip(Us, U_errs):
        tg = float(input('Введите t_д: '));
        tg_err = float(input('Введите погрешность t_д: '));
        omega = 2*PI / tg;
        omega_err = omega * tg_err / tg;
        omegas.append((U, omega, U_err, omega_err));
        print(f"\nomega = {omega} ± {omega_err}");
    with open("outputU.data", "w+") as f:
        for a, b, c, d in omegas:
            f.write(f"{a} {b} {c} {d}\n");
    with open("outputT2_timer.data", "w+") as f:
        for a, b, c in timer:
            f.write(f"{a} {b} {c}\n")
    with open("outputT2_photo.data", "w+") as f:
        for a, b, c in photo:
            f.write(f"{a} {b} {c}\n")


if __name__ == "__main__": main();

