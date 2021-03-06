^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package web_video_server
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.0.2 (2015-02-20)
------------------
* Merge pull request #10 from mitchellwills/develop
  Added option to specify server address
* Added option to specify server address
* Merge pull request #3 from mitchellwills/develop
  Remove old http_server implementation and replace it with async_web_server_cpp package
* Moved from using built in http server to new async_web_server_cpp package
* Did some cleanup of streamers
* Update package.xml
* Contributors: Mitchell Wills, Russell Toris

0.0.1 (2014-10-30)
------------------
* missed travis file
* cleanup and travis build
* ROS auto-format
* Merge pull request #1 from mitchellwills/develop
  Initial implementation of a http web server that serves ROS image topics as multiple web compatible formats
* Made some changes suggested by catkin_lint and did some package cleanup
* Added support for libavstreamer on Ubuntu 13.10 version of libav
* Added support for specifying vp8 quality parameter
* Implemented lazy initialization for libav buffers so that output size can be infered from the first image
* Updated README
* Added vp8 support
* Broke image encodings out into different files
* Made write operations async
  Send timestamps for mjpeg stream
* Initial commit
* Update README.md
* Update README.md
* Update README.md
* Initial commit
* Contributors: Mitchell Wills, Russell Toris
