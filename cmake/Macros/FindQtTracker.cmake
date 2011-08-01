#find_path(SIGNON_INCLUDE_DIR signon-qt/SignOn/signon.h /usr/include)
find_path(QTTRACKER_INCLUDE_DIR tracker.h PATHS /usr/include/QtTracker NO_DEFAULT_PATH)

find_library(QTTRACKER_LIBRARY qttracker PATH /usr/lib)

if(QTTRACKER_INCLUDE_DIR AND QTTRACKER_LIBRARIES)
   set(QTTRACKER_FOUND TRUE)
endif(QTTRACKER_INCLUDE_DIR AND QTTRACKER_LIBRARIES)

if(QTTRACKER_FOUND)
   if(NOT QtTracker_find_QUIETLY)
      message(STATUS "Found QtTracker: ${QTTRACKER_LIBRARIES}")
   endif(NOT QtTracker_find_QUIETLY)
else(QTTRACKER_FOUND)
   if(QtTracker_find_REQUIRED)
      message(FATAL_ERROR "Could not find QtTracker")
   endif(QtTracker_find_REQUIRED)
endif(QTTRACKER_FOUND)
