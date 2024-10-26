import os, sys

# HACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACKHACK
cwd = os.getcwd()
cwd = cwd.rstrip('/1234567890') # remove characters from string
sys.path.insert(0, cwd)
from praklib import *

def v(l, t): return l/t
def v2(l, t): return (l/t)**2
def a_teor(dh2, L): return 9.807 * dh2 / L

def main():
    print("Введите L1-L3 (м), l1-l3 (м), M1-M3 (г)")
    Ls = [float(input()) for _ in range(3)]
    ls = [float(input()) for _ in range(3)]
    Ms = [float(input())/1000.0 for _ in range(3)]
    
    for i in Ls, ls, Ms:
        sig_sist = float(input(f"сист. погрешность для {getvarbyval(i, locals())}(м/Кг): "))
        print(f"{getvarbyval(i, locals())}: mean={avg(i)}; stddev={stddev(i)}; sigma={sigsum(sig_sist, stddev(i))}")
        i.append(sigsum(sig_sist, stddev(i)))

    print("Введите 3 массы тележек");
    h = 6 # mm
    rest = {float(input())/1000.0 : [[round(0.2 * (j+1), 1), 0, 0] for j in range(5)] for i in range(3)}
    for m in sorted(rest.keys()):
        for x in rest[m]:
            print(f"введите t1, t2, t3 для dh=6mm, m={m*1000}г, x={x[0]}м")
            ts = [float(input()) for _ in range(3)]
            x[1] = avg(ts)
            x[2] = stddev(ts)
            print(f"t_avg = {x[1]}, t_dev = {x[2]}")
    
    l = avg(ls[:-1]); sl = ls[-1]
    resv = {i : [np.zeros((5,), dtype=float) for _ in range(4)] for i in sorted(rest.keys())}
    for m in sorted(rest.keys()):
        for i, x in enumerate(rest[m]):
            args = [l, x[1]]
            sigmas = [sl, x[2]]
            _v = v(*args)
            _v2 = v2(*args)
            _v2s = sigma(v2, args, sigmas)
            resv[m][0][i] = x[0]
            resv[m][1][i] = _v
            resv[m][2][i] = _v2
            resv[m][3][i] = _v2s
            print(f"для m={m*1000}г, x={x[0]}м: v={_v}, v^2={_v2}, sigma_v={_v2s}")
    for m in sorted(resv.keys()):
        data = resv[m]
        d = mnk(data[0], data[2], data[3])
        A = d['a']; 
        sA = d['sa']
        B = d['b']; sB = d['sb']
        a = A/2; sa = sA/2
        a_t = a_teor(h / 1e3, avg(Ls[:-1]))
        sa_t = sigma(a_teor, [h/1e3, avg(Ls[:-1])], [float(input("Введите погрешность dh_2: ")), Ls[-1]])
        print(f"A = {A}+-{sA}\nB = {B}+-{sB}\na = {a}+-{sa}\na_teor = {a_t}, sigma_a_teor = {sa_t}")

if __name__ == "__main__":
    main()

