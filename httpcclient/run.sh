ps -ef | grep "dest" | grep -v "grep"
if [ "$?" -eq 1 ]
then
/home/pi/newmethod/dest rtsp://admin:HikRASYVE@10.0.0.201/Streaming/Channels/1  h264_mmal  10 10.0.0.3
fi
