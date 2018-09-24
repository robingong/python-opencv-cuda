/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
//################################################################################
//
//                    Created by Neeraj Gulia
//
//################################################################################

#include "precomp.hpp"

namespace cv
{
    namespace pythoncuda
    {

        CV_EXPORTS_W void cpuOpticalFlowFarneback( InputArray prev, InputArray next, InputOutputArray flow,
                                           double pyr_scale, int levels, int winsize,
                                           int iterations, int poly_n, double poly_sigma, int flags )
        {
            cv::calcOpticalFlowFarneback(prev, next, flow, pyr_scale, levels, winsize,
                                        iterations, poly_n, poly_sigma, flags);
        }
        
        CV_EXPORTS_W void gpuOpticalFlowFarneback( InputArray prev, InputArray next, InputOutputArray flow,
                                           double pyr_scale, int levels, int winsize,
                                           int iterations, int poly_n, double poly_sigma, int flags )
        {
            cv::Ptr<cv::cuda::FarnebackOpticalFlow> farn = cv::cuda::FarnebackOpticalFlow::create();
            farn->setPyrScale(pyr_scale);
            farn->setNumLevels(levels);
            farn->setFastPyramids(false);
            farn->setWinSize(winsize);
            farn->setNumIters(iterations);
            farn->setPolyN(poly_n);
            farn->setPolySigma(poly_sigma);
            farn->setFlags(flags);

            cv::cuda::GpuMat d_flow, d_prev, d_next;
            d_prev.upload(prev);
            d_next.upload(next);
            farn->calc(d_prev, d_next, d_flow);
            d_flow.download(flow);
        }
        
        CV_EXPORTS_W void cpuOpticalFlowPyrLK( InputArray prevImg, InputArray nextImg,
                                        InputArray prevPts, InputOutputArray nextPts,
                                        OutputArray status, OutputArray err,
                                        Size winSize = Size(21,21), int maxLevel = 3,
                                        TermCriteria criteria = TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                        int flags = 0, double minEigThreshold = 1e-4 )
        {
            cv::calcOpticalFlowPyrLK(prevImg, nextImg, prevPts, nextPts, status, err, 
                                winSize, maxLevel, criteria, flags, minEigThreshold );
        }
        
        CV_EXPORTS_W void gpuOpticalFlowPyrLK( InputArray prevImg, InputArray nextImg,
                                        InputArray prevPts, InputOutputArray nextPts,
                                        OutputArray status, OutputArray err,
                                        Size winSize = Size(21,21), int maxLevel = 3, int iterations = 30)
        {                       
            Ptr<cuda::SparsePyrLKOpticalFlow> d_pyrLK_sparse = cuda::SparsePyrLKOpticalFlow::create(winSize, maxLevel, iterations);
            const cv::cuda::GpuMat d_prevImg(prevImg);
            const cv::cuda::GpuMat d_nextImg(nextImg);
            const cv::cuda::GpuMat d_err;
            const cv::cuda::GpuMat d_pts(prevPts.getMat().reshape(2, 1)); //convert rows to 1
            cv::cuda::GpuMat d_nextPts;
            cv::cuda::GpuMat d_status;
            
            d_pyrLK_sparse->calc(d_prevImg, d_nextImg, d_pts, d_nextPts, d_status, d_err);
            cv::Mat& nextPtsRef = nextPts.getMatRef();
            d_nextPts.download(nextPtsRef);
            nextPtsRef = nextPtsRef.t(); //revert the matrix to its actual shape
            d_status.download(status);
            d_err.download(err);
        }
    }
}
