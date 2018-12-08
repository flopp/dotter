[![Build Status](https://travis-ci.org/flopp/dotter.svg?branch=master)](https://travis-ci.org/flopp/dotter)

# dotter
Qt based viewer for GraphViz (.dot) files

![Dotter](https://github.com/flopp/dotter/raw/master/examples/screenshot.png)

## Compile

To compile dotter, you need a working Qt5 installation.
On Ubuntu, you can install the required packages by

```
sudo apt install qt5-default libqt5svg5-dev graphviz
```


```
git clone https://github.com/flopp/dotter.git
mkdir build
(cd build && qmake ../dotter.pro && make)
```
