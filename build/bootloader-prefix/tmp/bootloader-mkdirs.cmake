# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/ESP_IDF/v5-14/esp-idf/components/bootloader/subproject"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/tmp"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/src"
  "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Abhinav/SuperSaber/LightsaberMPU6050/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
