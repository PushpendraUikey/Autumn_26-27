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

# Calculating the Mean-Shift Update
def mean_shift_step(x, samples, h):
    xnew = 0
    dnom = 0
    for si in samples:
        enumr = (x-si)/h
        num = np.exp(-(enumr**2)/2)
        xnew += (num * si)
        dnom += num

    xnew /= dnom
    return xnew

def mean_shift(x0, samples, h, tol, max_iter):
    currx = x0
    iteration = 0
    # visited = [currx]
    while iteration < max_iter:
        newx = mean_shift_step(currx, samples, h)
        delta = abs(newx-currx)
        currx = newx
        # visited.append(currx)
        iteration += 1
        print(f"for x0 : {x0}, current x : {currx}, iteration : {iteration}")
        if delta < tol:
            break

    # plt.plot(visited, label=f"means shift for start:{x0} and h:{h}")
    # plt.legend()
    # plt.show()
    # plt.close()
    return currx, iteration

def cluster_modes(modes, merge_tol):

    np.sort(modes)
    mode_label = {}
    i = 0
    m = 1
    mode_label.setdefault(m, []).append(modes[0])
    while i < len(modes)-1:
        if abs(modes[i] - modes[i+1]) > merge_tol:
            m += 1
        mode_label.setdefault(m, []).append(modes[i+1])
        i += 1

    return mode_label

def create_synthetic_img(h, w, channels):
    img = np.zeros((h, w, channels), dtype=np.uint8)
    for i in range(h):
        for j in range(w):
            img[i, j, 0] = int((i/h)*255)
            img[i, j, 1] = int((j/w)*255)
            img[i, j, 2] = int(((i+j)/(h+w))*255)

    return img

def convert_into_feature(img):
    h, w, c = img.shape
    featurev = np.zeros((h*w, c+2), dtype=np.float32)

    for i in range(h):
        for j in range(w):
            idx = i*w + j
            featurev[idx][0] = img[i, j, 0]
            featurev[idx][1] = img[i, j, 1]
            featurev[idx][2] = img[i, j, 2]
            featurev[idx][3] = j
            featurev[idx][4] = i
    return featurev

def feature_scaling(featurev, hr, hs):
    featurecopy = featurev.copy()
    if hr > 0 and hs > 0:
        featurecopy[:, :3] /= hr
        featurecopy[:, 3:] /= hs

    return featurecopy

if __name__ == "__main__":
    # lst = [0.0, 0.2, 0.4, 3.0, 3.2, 3.4]
    # nlst = []
    # h = 0.2
    # for val in lst:
    #     nlst.append(B(val, h, lst))

    # print(A(0, h))
    # print(nlst)
    # query_points = np.linspace(-1, 5, 500)
    # h_values = [0.2, 0.6, 2.0] # small h, weights concentrated around the data points, large h, weights spread out

    # for hv in h_values:
    #     t_kde = []
    #     for qpts in query_points:
    #         t_kde.append(B(qpts, hv, lst))

    #     plt.plot(query_points, t_kde, label=f"h : {hv}")

    # plt.legend()
    # plt.savefig("kde-estimate.png")
    # plt.close()

    # x_pos = [0.1, 1.6, 3.3]
    # h = 0.6
    # ux_pos = []
    # for xv in x_pos:
    #     ux_pos.append(mean_shift_step(xv, lst, 0.6))
    #     print(ux_pos[-1])

    # modes = []
    # for xv in lst:
    #     modes.append(mean_shift(xv, lst, h, 1e-6, 100)[0])
    #     print(modes[-1])
    # modes = np.sort(modes)
    # mode_cluster = cluster_modes(modes, 0.1)
    # for m in mode_cluster:
    #     print(f"mode label : {m}  --- mode value : {np.average(mode_cluster.get(m))}")

    # for m in modes:
    #     print(f"{m}, ")
    
    ex1_img = create_synthetic_img(2, 3, 3)
    featurev = convert_into_feature(ex1_img)
    for i in range(featurev.shape[0]):
        print(f"feature vector : {featurev[i]}")