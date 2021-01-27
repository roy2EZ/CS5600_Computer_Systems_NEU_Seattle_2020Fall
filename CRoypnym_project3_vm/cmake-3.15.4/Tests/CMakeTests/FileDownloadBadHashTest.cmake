if(NOT "/home/roychen/cs5600/hw3/CRoypnym_project3_vm/cmake-3.15.4/Tests/CMakeTests" MATCHES "^/")
  set(slash /)
endif()
set(url "file://${slash}/home/roychen/cs5600/hw3/CRoypnym_project3_vm/cmake-3.15.4/Tests/CMakeTests/FileDownloadInput.png")
set(dir "/home/roychen/cs5600/hw3/CRoypnym_project3_vm/cmake-3.15.4/Tests/CMakeTests/downloads")

file(DOWNLOAD
  ${url}
  ${dir}/file3.png
  TIMEOUT 2
  STATUS status
  EXPECTED_HASH SHA1=5555555555555555555555555555555555555555
  )
