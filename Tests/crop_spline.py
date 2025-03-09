import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
import os
import scipy.interpolate as interpolate

# path = "/home/tim/Documents/ArbeitTestData/TestResultsCrops/Crops"
# files = os.listdir(path)
# file = os.path.join(path, np.random.choice(files))
file = "/home/tim/Documents/ArbeitTestData/TestResultsCrops/Crops/CUSCO-Peru-2020_M4_PISCO20200315_155239.4700_1.9bar_223.png"
print(file)
img = cv.imread(file)

plt.subplot(141)
plt.imshow(img[:, :, 0], cmap="inferno")
plt.subplot(142)
plt.imshow(img[:, :, 1], cmap="inferno")
plt.subplot(143)
plt.imshow(img[:, :, 2], cmap="inferno")
# plt.subplot(144)
# plt.imshow(img[::-1])
plt.show()

# # img = img[:, :, 0]
# contour = img[:, :, 1].astype(np.float64) + img[:, :, 2].astype(np.float64) * 255
# print(contour.shape)
# # plt.imshow(contour)
#
# cnt = np.where(contour > 0)
# cnt = list(np.array(cnt).T)
# cnt = np.array(sorted(cnt, key=lambda x: contour[x[0], x[1]]))
# cnt = cnt[::3]
# # print(cnt)
#
#
# # img = img[:, :, 2]
# x = cnt[:, 1]
# y = cnt[:, 0]
#
# # # print(CubicSpline(x, y))
# tck, u = interpolate.splprep([x, y], per=True, s=0)
# print(tck)
# x_i, y_i = interpolate.splev(np.linspace(0, 1, 1000), tck)
# # plt.scatter(x, y, zorder=3)
# plt.plot(x_i, y_i)
# #
# plt.show()
#
# # img = cv.resize(img, (0, 0), fx=5, fy=5)
# # cv.imshow("img", img)
# # while cv.waitKey(0) != ord("q"):
# #     ...
