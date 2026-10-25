import numpy as np

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

def mean_shift_weight(f1, f2, hs, hr):

    v1 = np.sum(np.square(f1[:3]-f2[:3]))/2
    v2 = np.sum(np.square(f1[3:]-f2[3:]))/2
    if hr > 0 and hs > 0:
        v1 = v1 / (hr**2)
        v2 = v2 / (hs**2)
    else:
        raise ValueError("hr and hs should be greater than 0")
    wt = np.exp(-(v1+v2))

    return wt


def mean_shift_step(current, samples, hs, hr):
    """
    Current is one 5-dimensional vector, representing the pixel whose position in feature space we're updating.
    samples is a matrix of 5-dimensional vectors, representing all pixels that influence the update
    """
    wt = 0
    fnew = np.zeros_like(current)
    for i in range(samples.shape[0]):
        wi = mean_shift_weight(current, samples[i], hs, hr)
        wt += wi
        fnew = np.add(fnew, (wi * samples[i]))

    if wt > 0:
        fnew = np.divide(fnew, wt)

    return fnew


def mean_shift(current, samples, hs, hr, tol, max_iter):
    iteration = 0
    converged = False
    while iteration < max_iter:
        newc = mean_shift_step(current, samples, hs, hr)
        delta = np.linalg.norm(newc-current)
        current = newc
        iteration += 1
        if delta < tol:
            converged = True
            break

    if converged:
        print(f"Converged after {iteration} iterations")
    else:
        print(f"Did not converge after {iteration} iterations")

    return current, iteration

if __name__ == '__main__':
    f1 = np.array([0, 0, 0, 0, 0], dtype=float)
    f2 = np.array([0, 85, 51, 1, 0], dtype=float)

    hs = 2
    hr = 60

    print(mean_shift_weight(f1, f1, hs, hr))  # 1.0
    print(mean_shift_weight(f1, f2, hs, hr))  # Less than 1

    f1 = create_synthetic_img(2, 3, 3)
    samples = convert_into_feature(f1)
    updated_features = []

    for feature_ in samples:
        updated_features.append(mean_shift_step(feature_, samples, hs, hr))
        print(updated_features[-1])

    print()
    print(f"\nConverged features after mean shift\n")

    tol = 0.1
    max_iter = 100
    converged_features = []
    for feature_ in samples:
        fnew, iteration = mean_shift(feature_, samples, hs, hr, tol, max_iter)
        converged_features.append(fnew)

    