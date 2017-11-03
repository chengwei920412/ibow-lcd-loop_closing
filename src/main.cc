/**
* This file is part of ibow-lcd.
*
* Copyright (C) 2017 Emilio Garcia-Fidalgo <emilio.garcia@uib.es> (University of the Balearic Islands)
*
* ibow-lcd is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ibow-lcd is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ibow-lcd. If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>

#include <boost/filesystem.hpp>
#include <opencv2/features2d.hpp>

#include "ibow-lcd/lcdetector.h"

void getFilenames(const std::string& directory,
                  std::vector<std::string>* filenames) {
    using namespace boost::filesystem;

    filenames->clear();
    path dir(directory);

    // Retrieving, sorting and filtering filenames.
    std::vector<path> entries;
    copy(directory_iterator(dir), directory_iterator(), back_inserter(entries));
    sort(entries.begin(), entries.end());
    for (auto it = entries.begin(); it != entries.end(); it++) {
        std::string ext = it->extension().c_str();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png" || ext == ".jpg" ||
            ext == ".ppm" || ext == ".jpeg") {
            filenames->push_back(it->string());
        }
    }
}

int main(int argc, char** argv) {
  // Creating feature detector and descriptor
  cv::Ptr<cv::Feature2D> detector = cv::ORB::create(1500);  // Default params

  // Loading image filenames
  std::vector<std::string> filenames;
  getFilenames(argv[1], &filenames);
  unsigned nimages = filenames.size();

  // Creating the loop closure detector object
  ibow_lcd::LCDetectorParams params;  // Assign desired parameters
  ibow_lcd::LCDetector lcdet(params);

  // Processing the sequence of images
  for (unsigned i = 0; i < nimages; i++) {
    // Processing image i
    std::cout << "Processing image " << i << std::endl;

    // Loading and describing the image
    cv::Mat img = cv::imread(filenames[i]);
    std::vector<cv::KeyPoint> kps;
    detector->detect(img, kps);
    cv::KeyPointsFilter::retainBest(kps, 1000);
    cv::Mat dscs;
    detector->compute(img, kps, dscs);

    ibow_lcd::LCDetectorResult result;
    result = lcdet.process(i, kps, dscs);

    // If a loop has been found
    if (result.isLoop()) {
      std::cout << "Loop detected: " << result.train_id << std::endl;
    }
  }

  return 0;
}
