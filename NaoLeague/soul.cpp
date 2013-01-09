/*
 * Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the COPYING file.
 */
#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include "Vision\imageGrabber.h"

int main()
{
	ImageGrabber imageGrabber("169.254.95.24");
	ImageContainer* imageContainer;
	imageContainer = NULL;
	while (true){
		if(imageContainer != NULL){
			delete imageContainer;
		}
		imageContainer = imageGrabber.pollImage();
		cv::imshow("Polled Image", imageContainer->image);
		cv::waitKey(10);
	}
}
