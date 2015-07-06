rm -R out/
mkdir out/

#-s 1024x768
#avprobe -show_streams walker.mov  -show_format -of json

avconv -i $1 -vsync 1 -r 25 -an -y -qscale 1  out/out_%05d.png
./dxt_creator out/out_%05d.png video.dxt5 0 100
