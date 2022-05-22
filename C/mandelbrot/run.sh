clear && rm -rf output && mkdir -p output/frames && make && ./mandelbrot && cd output && makevideo && cd ..
