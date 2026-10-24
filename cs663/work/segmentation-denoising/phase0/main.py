import numpy as np
import math
import matplotlib.pyplot as plt

# Gaussian Kernel
def A(val: float, h:float) -> float :
    num = np.exp(-(val/h)**2/2)
    den = h * np.sqrt(2*math.pi)
    kernel = num/den

    return kernel


# Kernel Density Estimate
def B(x : float, h: float, lst: list) -> float :
    val = 0
    n = len(lst)
    for xi in lst:
        val += A(x-xi, h)

    val /= n

    return val

if __name__ == "__main__":
    lst = [0.0, 0.2, 0.4, 3.0, 3.2, 3.4]
    nlst = []
    h = 0.2
    for val in lst:
        nlst.append(B(val, h, lst))

    # print(A(0, h))
    # print(nlst)
    query_points = np.linspace(-1, 5, 500)
    h_values = [0.2, 0.6, 2.0] # small h, weights concentrated around the data points, large h, weights spread out

    for hv in h_values:
        t_kde = []
        for qpts in query_points:
            t_kde.append(B(qpts, hv, lst))

        plt.plot(query_points, t_kde, label=f"h : {hv}")

    plt.legend()
    plt.savefig("kde-estimate.png")
        