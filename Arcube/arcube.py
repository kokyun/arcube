import cv2 as cv
import numpy as np

def draw_cube(img, corners, imgpts):
    imgpts = np.int32(imgpts).reshape(-1,2)
    img = cv.drawContours(img, [imgpts[:4]],-1,(0,255,0),3)
    for i,j in zip(range(4),range(4,8)):
        img = cv.line(img, tuple(imgpts[i]), tuple(imgpts[j]),(255),3)
    img = cv.drawContours(img, [imgpts[4:]],-1,(0,0,255),3)
    return img

criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

objp = np.zeros((6*7,3),np.float32)
objp[:,:2]=np.mgrid[0:7,0:6].T.reshape(-1,2)

cv.namedWindow('windw')

img = cv.imread("./images/chess2.jpg")
gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
H,W,C = img.shape

ret, corners = cv.findChessboardCorners(gray, (7,6),None)

if ret == True:
    # cv.imshow('windw',img)
    # cv.waitKey(1000)
    objpoints= np.array(objp)
    cv.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
    imgpoints = np.array(corners)
    cv.drawChessboardCorners(img, (7,6), corners,ret)
    # cv.imshow('windw',img)
    # cv.waitKey(1000)

    fx = 3464.102
    fy = 3464.102
    cx = W/2
    cy = H/2

    matA = np.array([[fx, 0, cx], [0, fy, cy], [0, 0, 1]])
    distCoeff =  np.zeros((4,1))

    _, rvec, tvec = cv.solvePnP(objpoints,imgpoints ,matA, distCoeff)
    R = cv.Rodrigues(rvec)[0]
    t = tvec
    pos = -np.matrix(R).T * np.matrix(tvec)

    axis = np.float32([[3,2,0], [3,4,0], [5,4,0], [5,2,0],
                   [3,2,-2],[3,4,-2],[5,4,-2],[5,2,-2] ])    # this for draw a cube
    
    cmpoints = np.dot(R, axis.transpose(1,0)).transpose(1,0) + t.reshape(1,3)
    cmpoints = np.float32(cmpoints)
    pixel_coordinate_cube = []
    for i in cmpoints:
        pixel_coor = matA@i.reshape(3,1)
        x = pixel_coor.reshape(1,3)[0][0]/pixel_coor.reshape(1,3)[0][2]
        y = pixel_coor.reshape(1,3)[0][1]/pixel_coor.reshape(1,3)[0][2]
        new_coord = np.array([x,y])
        pixel_coordinate_cube.append(new_coord)
    new1 = (cmpoints[0][0],cmpoints[0][1])
    new2 = (cmpoints[1][0],cmpoints[1][1])
    frame = draw_cube(img,corners,pixel_coordinate_cube)
    cv.imshow('windw',frame)
    cv.waitKey()

    cv.destroyAllWindows()



