# GfaViz

GfaViz is a graphical interactive tool for the visualization of sequence graphs in GFA format.
It supports both the GFA1 and GFA2 formats.

## About GFA

GFA (Graphical Fragment Assembly) is an emerging standard format for representing sequence graphs.
Although it was originally conceived as a format for sequence assembly (hence the name), and this
remains its core application, it is more general, and able to represent many different types of sequence graphs,
including scaffolding graphs, alignment graphs, variant graphs and splicing graphs.

The specification of GFA is available here: https://github.com/GFA-spec/GFA-spec.
Two versions of the format were developed. GFA1 was the first released specification and is more focused on
sequence assembly. GFA2 is a more powerful and extensible version of the format.
New applications should implement the GFA2 format.

## Installation

### Dependencies

GfaViz is implemented in C++, using the Qt framework. For this reason, Qt5 needs to be installed on your system.

### Building

The following code will install GfaViz:
```
git clone https://github.com/niehus/gfaviz
cd gfaviz
qmake-qt5
make
```

### Link with your own OGDF installation

TODO: this is most likely unimportant for end users and can be removed.

The graph computation in GfaViz is performed by the OGDF library, which comes prebundled with GfaViz. If you want to link against your own installation of OGDF, use the ``OGDFDIR`` parameter in the ``qmake-qt5`` call:

```qmake-qt5 OGDFDIR=<PathToYourOGDFInstallation>```

Note that GfaViz has been developed using "OGDF Snapshot 2017-07-23", other versions have not been tested.

### SVG support

Some systems have QT preinstalled, but without the libraries necessary for the QT SVG support. If this is the case, you may 
either install the missing headers, or disable
the SVG support in GfaViz by using the ``NOSVG`` parameter in the ``qmake-qt5`` call:

```qmake-qt5 NOSVG=true```

The result will be a version of GfaViz unable to export the graph as SVG. All other functionality will be available.

## Running

GfaViz is run executing the binary file ``viz.x``.

### User interface choice

GfaViz has two user interfaces: a command line interface (CLI) and a graphic user interface (GUI).
The command line interface is not interactive as the GUI, but it is useful e.g. for very large graphs, for batch processing or for including GfaViz in software pipelines.
The default interface is the GUI. Using the option ``--no-gui`` or ``-n`` will allow to run in CLI mode.

### Opening files

When running in GUI mode, one or multiple filenames can be provided as argument of the command line. If none is provided, the files can be loaded from inside the GUI. When running in CLI mode, a single filename shall be provided.
In the GUI, files can be opened from the File menu, under "Open GFA file". The GFA1 and GFA2 formats are accepted.
To be sure that the input files are compatible, one can use format checkers such as GfaLint. To modify Gfa files we suggest to use the Python library GfaPy.
 
### Layout options

Two possible algorithms can be used for computing the layout. These algorithms are implemented by the OGDF library. The algorithms are: "Stress Minimization" (SM) and "Fast Multipole Multilevel Method" (FMMM). While each algorithm has its advantages and disadvantages, in general FMMM is faster than SM, but SM offers a better quality and is the default.

In order to compute the graph using FMMM, use the ``--fmmm`` option. In CLI mode, this will apply to the rendered graph. In GUI mode, this will apply when the graph is first rendered, however, it can be then changed from inside the GUI.

In the GUI the layout options are located in the lower section of the right pane. Currently some parameters for the two layout algorithms can be set in the GUI (these cannot be set from the CLI in the current implementation).

### Style options

Multiple options are available for changing the visual representation of specific kind of elements.

#### GUI

To change the style of specific element types, use the label options in the element type tab in the Style widget.
To change the style of specific elements, select the element and use the options in the element type tab, making sure that the checkbox "apply changes to selection only" is on.

#### CLI

Several options can be used for changing the representation style of elements.
TODO: complete this section.

```
  --seg-width <value>                  Width of the segments.
  --seg-outline-width <value>          Width of the segment outline.
  --seg-color <value>                  Color of the segment.
  --seg-outline-color <value>          Color of the segment outline.
  --seg-max-subnodes <value>           Maximum number of subnodes for a segment
                                       representation.
  --edge-width <value>                 Width of the links/edges.
  --edge-color <value>                 Color of the links/edges.
  --dovetail-width <value>             Width of dovetail links.
  --dovetail-color <value>             Color of dovetail links.
  --internal-width <value>             Width of non-dovetail links.
  --internal-color <value>             Color of non-dovetail links.
  --group-width <value>                Width of the groups.
  --group-colors <value>               Colors of the groups, separated by
                                       commas.
  --gap-width <value>                  Width of the gaps.
  --gap-color <value>                  Color of the gaps.
  --fragment-width <value>             Width of the fragments.
  --fragment-color <value>             Color of the fragments.
```


### Label style options

Most elements in GFA can have a name, which can be vizualized in GfaViz as a text label. The label style options control the vizualization. Labels are not visualized by default.

#### CLI

Labels can be turned on, by using the ``--labels`` option. If only the labels of specific kind of elements shall be turned on, use instead the ``--[seg|edge|gap|group|fragment]-labels`` options.

The representation of the labels can be changed using specific options for single kind of lines, or for all kind of lines. The latter is done using options which start with ``--label-``. In particular, the font, size, color, outline width and outline color can be set. The same can be set for single kind of elements adding the prefix ``seg|gap|group|frag|edge``. An example would be ``--frag-label-outline-color``.

#### GUI

To enable labels and change the visualization style, use the Label tab in the Style widget. 
To change the style of specific element types, use the label options in the element type tab in the Style widget.
To change the label of specific elements, select the element and use the label options in the element type tab, making sure that the checkbox "apply changes to selection only" is on.

### Hiding elements

By default, all elements of the GFA file are visualized. Specific kind of elements or single elements can be hidden.

#### GUI

In the GUI, elements of a given type can be hidden using the Style widget, located in the lower section of the right pane. For each kind of element, an "hide element" checkbox is available.

The GUI also allows to hide a single specific element (instead of all elements of a kind). To do this, select an element (by clicking on it in the graph visualization, using the search box or the graph navigation widget) and click on "hide element" in the Style widget, making sure that the checkbox "apply changes to selection only" is on.

#### CLI

Elements of a specific type can be hidden using the CLI, using the options ``--no-gaps``, ``--no-fragments``, ``--no-groups``. NOTE: ``--no-segments`` and ``--no-edges`` seems to be missing.

In the CLI, single elements cannot be hidden directly. However, it is possible to specify in the GFA file, that an element shall not be visualized, by adding to the JSON tag ``vo`` (which must be added if not present yet) the "no-X:true" value, where X is one of "segments, fragments, groups, edges, gaps". For example to hide a specific segment, which has not ``vo`` tag yet, add a tag ``vo:J:{"no-segments":true}`` to the line.

### Rendering

The graph can be rendered to vector graphics and raster bitmap formats.

```
 -r, --render                         Render graph(s) into file(s).
  -o, --output <filename>              Render graph(s) into <filename>
  -f, --output-format <format>         File format for the output. If no value
                                       is specified, format will be inferred
                                       from the file suffix specified in the
                                       --output option. Possible values: BMP,
                                       PNG, JPG, JPEG, PBM, XBM, XPM, SVG.
                                       Default: PNG
  -W, --width <width>                  Width of the output file in pixels.
  -H, --height <height>                Height of the output file in pixels.
  -t, --transparency                   Transparent background in rendered
                                       images (only png).

  --bg-color <value>                   Background color.
```
TODO: explain the use of these options. Explain how to do it from the GUI. Use of ``-r`` from the CLI.

### Stylesheets

```
  -s, --stylesheet <filename>          Use the style options represented by the
                                       stylesheet <filename>.
```

TODO: Explain Stylesheets function. Explain the syntax. List the possible settings.

### Tree navigation

TODO: Explain the tree navigation functionality.

### Search for an element

TODO: Briefly Explain how the search button in the GUI works.

### List of options

To see a list of all available commands use: 
```
./viz.x --help
```

## Credits

The graph computation in GfaViz is performed by the OGDF library. Their excellent work can be found here: http://www.ogdf.net/

## License

TODO!
