import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt



# img_path = "/home/tim/Documents/ArbeitTestData/selection/SO308_1-5-1_PISCO2_0013.11dbar-20.10S-036.10E-25.75C_20241106-15320415.png"
# img_file = "/home/tim/Documents/ArbeitTestData/TestResults/objects_img_" + img_path.split("/")[-1][:-3] + "dat" 
#
# crop_data = []
# contours = []
# with open(img_file, "r") as f:
#     min_area = 1000
#     lines = f.readlines()
#     for line in lines[1:]:
#         line = line.split(",")
#         area = float(line[6])
#         if area > min_area:
#             elements = [float(l) for l in line[2:-1]]
#             crop_data.append(line[:2] + elements)
#             contour = line[-1][1:-2].split("|")
#             contour = [np.array([int(p) for p in l.split(";")]) for l in contour]
#             contours.append(np.array(contour, dtype=np.int32))
#
# idx = 3
# cnt = contours[idx]
# obj = crop_data[idx]
# crop_path = "/home/tim/Documents/ArbeitTestData/TestResults/crops/" + obj[1][:-4] + "_" + obj[0] + ".png"
# crop = cv.imread(crop_path, cv.IMREAD_GRAYSCALE)
# img = cv.imread(img_path, cv.IMREAD_GRAYSCALE)
#
#
# res = cv.matchTemplate(img, crop, cv.TM_CCOEFF_NORMED)
# min_pos = np.min(res)
# min_pos = np.where(res == min_pos)
#
# plt.subplot(121)
# img_color = cv.cvtColor(img, cv.COLOR_GRAY2BGR)
# img_color = cv.drawContours(img_color, [cnt], -1, (0, 255, 0))
# plt.imshow(img_color)
# # img = cv.resize(img, (0, 0), fx=0.5, fy=0.5)
#
# plt.subplot(122)
# plt.imshow(res)
# plt.scatter(min_pos[1], min_pos[0], color="r")
# plt.show()

path = "/home/tim/Documents/Arbeit/TestVideos/Julia_1_r.MP4"
cap = cv.VideoCapture(path)

backgrounds = []
nBackgrounds = 20
ret = True
last_cnt = None
cnt = None
while (ret):
    ret, frame = cap.read()

    if len(backgrounds) < nBackgrounds:
        backgrounds.append(frame)
        continue
    else:
        bg = np.min(backgrounds, axis=0)

    corrected = cv.absdiff(frame, bg)
    corrected = corrected[:, :, 2]
    
    thresh = cv.threshold(corrected, 0, 255, cv.THRESH_TRIANGLE)[1]
    contours = cv.findContours(thresh, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)[0]

    contours = list(contours)
    contours.sort(key=lambda x: cv.contourArea(x), reverse=True)

    last_cnt = cnt
    cnt = contours[0]
    x, y, w, h = cv.boundingRect(cnt)

    if last_cnt is None:
        continue
    else:
        score = cv.matchShapes(last_cnt, cnt, 1, 0)
        cv.putText(frame, str(score), (10, 50), cv.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 2)

    cv.drawContours(frame, [cnt], -1, (0, 255, 0))




    canvas = np.zeros(frame.shape, dtype=np.uint8)
    cv.drawContours(canvas, [cnt], -1, (0, 255, 0), -1)
    cv.drawContours(canvas, [last_cnt], -1, (0, 0, 255), -1)
    
    frame = np.concatenate([frame, canvas], axis=1)
    frame = cv.resize(frame, (0, 0), fx=0.25, fy=0.5)
    cv.imshow("Video", frame)
    if cv.waitKey(10) == ord("q"):
        break



