import cv2 as cv
import numpy as np
import os

offset = 0


path = "/home/tim/Documents/ArbeitTestData/Planktwin Example/Planktwin Example/test_sample2"
path_upper = path + "/upper"
path_lower = path + "/lower"

files_upper = [os.path.join(path_upper, f) for f in os.listdir(path_upper)]
files_upper.sort()
files_lower = [os.path.join(path_lower, f) for f in os.listdir(path_lower)]
files_lower.sort()

for i in range(len(files_upper)):
    img_upper = cv.imread(files_upper[i])
    print(img_upper.shape)
    img_lower = cv.imread(files_lower[i])
    img = np.concatenate([img_upper, img_lower], axis=1)
    img = cv.resize(img, [0, 0], fx=0.5, fy=0.5)
    cv.imshow("img", img)
    if cv.waitKey() == ord("q"):
        break



# files = [os.path.join(path, f) for f in os.listdir(path)]
#
# file = files[30]
# img = cv.imread(file, cv.IMREAD_GRAYSCALE)
# print(img.shape)
# img1 = img[:img.shape[0] // 2, :]
# img2 = img[img.shape[0] // 2:, :]
#
# def on_trackbar(val):
#     global offset 
#     offset = val
#     color = np.zeros((*img.shape,3 ), dtype=np.uint8)
#     color[:img1.shape[0],:,0] = img1
#     # color[img2.shape[0] - offset:2 * img2.shape[0] - offset,:,1] = img2
#     color = color[img2.shape[0] - offset:2 * img2.shape[0] - offset,:,:]
#     color[:,:,1] = img2
#     res = cv.resize(color, [0, 0], fx=0.25, fy=0.25)
#     cv.imshow("img", res)
#     
#
# slider_max = img2.shape[0]
# trackbar_name = 'Alpha x %d' % slider_max
#
# cv.namedWindow("img")
# cv.createTrackbar(trackbar_name, "img" , 0, slider_max, on_trackbar)
#
# # img = np.concatenate([img1, img2], axis=1)
#
# while True:
#     if cv.waitKey() == ord("q"):
#         break
