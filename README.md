# GfaViz (Working Title)

A program to visualize GFA graphs

## Getting Started

GfaViz needs Qt5 and OGDF to run. Get OGDF here: [Latest Snapshots](http://ogdf.net/doku.php/tech:download)

Note that GfaViz has only been tested with OGDF Snapshot 2017-07-23 yet, so download that one.

### Building
After building OGDF, clone GfaViz, run qmake with OGDFDIR specified to the path of your OGDF installation, then run make:
```
git clone https://github.com/niehus/gfaviz
cd gfaviz
qmake OGDFDIR=<PathToYourOGDFInstallation>
make
```

### Running
Run
```
./viz.x --help
```
to see a list of all available commands.

### Example:
Run
```
./viz.x testdata/*
```
to visualize all files in the testdata/ directory.
