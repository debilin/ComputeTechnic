## Building

### Linux
Install dependencies (tested on Debian 11)
```
apt-get install -y \
    build-essential \
    cmake \
    python3 \
    googletest \
    libgtest-dev \
    libeigen3-dev \
    libz-dev \
    python3-matplotlib \
    python3-numpy
```

Then run
```
cmake -B build -S . && cmake --build build
```
You will get an executable `lego_technic_main` inside the `build` folder.

### Windows

:thinking:

## Usage
To use one of the included models run
```
./lego_technic_main -path_select <model ID>
```
There are 33 models included with their corresponding ID:
```
1 	 line
2 	 cube
3 	 siggraph
4 	 robotarm
5 	 lifter_remove_3
6 	 flyingkite
7 	 bookshelf
8 	 small_crossbow
9 	 normalbow
10 	 plane
11 	 crossbow
12 	 3Dprinter
13 	 bridge
14 	 tokyotower
15 	 tokyo_temple
16 	 spacesation
17 	 temple_head
18 	 triangle
19 	 tian
20 	 triangle_tile
21 	 wind
22 	 castle
23 	 fan
24 	 fail_1
25 	 fail_2
26 	 user_ruihui
27 	 cartoon_car
28 	 bicycle
29 	 BOC
30 	 glass
31 	 wall-e
32 	 wheel
33 	 bird
```

To use your own model run
```
./lego_technic_main -path_provide <path to model file>
```
For example,
```
./lego_technic_main -path_provide ./my-model-sketch.obj
```
To visualize the edges run `lego_vis_edges.py` inside the `python` folder and pass the `unitEdges` file to it. For example,
```
python lego_vis_edges.py crossbow_unitEdges_2.txt
```