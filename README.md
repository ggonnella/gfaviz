# GfaViz (Working Title)

A program to visualize GFA graphs

## Getting Started

GfaViz needs Qt5 and OGDF to run. Get OGDF here: [Latest Snapshots](http://ogdf.net/doku.php/tech:download)

Note that GfaViz has only been tested with OGDF Snapshot 2017-07-23 yet, so download that one.

### Building
After building OGDF, clone GfaViz, run qmake with ``OGDFDIR`` specified to the path of your OGDF installation, then run make:
```
git clone https://github.com/niehus/gfaviz
cd gfaviz
qmake-qt5 OGDFDIR=<PathToYourOGDFInstallation>
make
```

### SVG support

If the QT SVG library header are not installed in your system, you may either install them, or disable
the SVG support by using the ``NOSVG`` parameter in the ``qmake-qt5`` call:

```qmake-qt5 OGDFDIR=<PathToYourOGDFInstallation> NOSVG=true```

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
