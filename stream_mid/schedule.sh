ps -ef | grep "streamtool" | grep -v "grep"
if [ "$?" -eq 1 ]
then
/home/pi/stream_mid/streamtool  /home/pi/stream_mid/ipc_ips.cfg  /home/pi/stream_mid/server_domain.cfg  /home/pi/stream_mid/avdict.cfg h264_mmal
fi
