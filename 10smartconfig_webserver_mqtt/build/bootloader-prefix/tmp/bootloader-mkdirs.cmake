# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/components/bootloader/subproject"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix/tmp"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix/src/bootloader-stamp"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix/src"
  "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/esp/Espressif/frameworks/esp-idf-v4.4.4/10smartconfig_webserver_mqtt/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
