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


if __name__ == '__main__':
    ex1_img = create_synthetic_img(2, 3, 3)
    featurev = convert_into_feature(ex1_img)
    for i in range(featurev.shape[0]):
        print(f"feature vector : {featurev[i]}")