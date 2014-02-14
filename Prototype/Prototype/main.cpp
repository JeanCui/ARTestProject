//
//  main.cpp
//  Prototype
//
//  Created by JinCui on 1/29/14.
//  Copyright (c) 2014 JinCui. All rights reserved.
//

// OpenCV Simple Augmented Reality

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace cv;
int main(int argc, const char * argv[])
{
    CvCapture   *capture = 0;
    IplImage    *image = 0;
   // IplImage    *frame = 0;
    IplImage    *disp, *neg_img, *cpy_img;
    int         key = 0;
   // int         fcount = 0;
    int         option = 1;
    
    capture = cvCaptureFromCAM(0);
    if (!capture) {
        return -1;
    }
    
    //CvCapture   *vid = cvCreateFileCapture("trailer.avi");
    //if (!vid) {
    //    return -1;
    //}
    
    IplImage    *pic = cvLoadImage("pic.jpg");
    cvFlip(pic, pic, 1);
    
    int         b_width = 5;
    int         b_height = 4;
    int         b_squares = 20;
    
    CvSize      b_size = cvSize(b_width, b_height);
    
    CvMat       *warp_matrix = cvCreateMat(3, 3, CV_32FC1);
    CvPoint2D32f*corners = new CvPoint2D32f[b_squares];
    int         corner_count;
    
    printf("Select an option to run the program\n\n");
    printf("1. Show an Image over the pattern.\n");
    printf("2. Play a Clip over the pattern.\n");
    printf("3. Mark the pattern.\n\n");
    //scanf("%d",&option);
    
    if (!(option >=1 && option <= 3)) {
        printf("Invalid Selection.\n");
        return -1;
    }
    
    cvNamedWindow("Video", CV_WINDOW_AUTOSIZE);
    

    
    
    
    while (key != 'q') {
        image = cvQueryFrame(capture);
        if(!image) break;
        cvFlip(image, image, 1);
        
        disp    = cvCreateImage(cvGetSize(image), 8, 3);
        cpy_img = cvCreateImage(cvGetSize(image), 8, 3);
        neg_img = cvCreateImage(cvGetSize(image), 8, 3);
        
        IplImage    *gray = cvCreateImage(cvGetSize(image), image->depth, 1);
        int found = cvFindChessboardCorners(image, b_size, corners, &corner_count,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        if(!found) break;
            
        cvCvtColor(image, gray, CV_BGR2GRAY);
        cvFindCornerSubPix(gray, corners, corner_count, cvSize(11,11), cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
        
        if (corner_count == b_squares) {
           // if (option == 1) {
                CvPoint2D32f p[4];
                CvPoint2D32f q[4];
                
                IplImage    *blank = cvCreateImage(cvGetSize(pic), 8, 3);
                cvZero(blank);
                cvNot(blank, blank);
                
                //Set of source points to calculate Perspective matrix
                q[0].x= (float) pic->width * 0;
                q[0].y= (float) pic->height * 0;
                q[1].x= (float) pic->width;
                q[1].y= (float) pic->height * 0;
                
                q[2].x= (float) pic->width;
                q[2].y= (float) pic->height;
                q[3].x= (float) pic->width * 0;
                q[3].y= (float) pic->height;
                
                //Set of destination points to calculate Perspective matrix
                p[0].x= corners[0].x;
                p[0].y= corners[0].y;
                p[1].x= corners[4].x;
                p[1].y= corners[4].y;
                
                p[2].x= corners[19].x;
                p[2].y= corners[19].y;
                p[3].x= corners[15].x;
                p[3].y= corners[15].y;
                
                //Calculate Perspective matrix
                cvGetPerspectiveTransform(q,p,warp_matrix);
                
                //Boolean juggle to obtain 2D-Augmentation
                cvZero(neg_img);
                cvZero(cpy_img);
                
                cvWarpPerspective( pic, neg_img, warp_matrix);
                cvWarpPerspective( blank, cpy_img, warp_matrix);
                cvNot(cpy_img,cpy_img);
                
                cvAnd(cpy_img,image,cpy_img);
                cvOr(cpy_img,neg_img,image);
                
                cvShowImage( "Video", image);
           // }
            key = cvWaitKey(1);
        }
        
       
        
    }
    cvDestroyWindow( "Video" );
    //  cvReleaseCapture( &vid );
    cvReleaseMat(&warp_matrix);
    cvReleaseCapture( &capture );
    
    return 0;
}

