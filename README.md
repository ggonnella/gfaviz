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

GfaViz is implemented in C++, using the Qt framework. For this reason, Qt5
needs to be installed on your system.

Compiling GfaViz was successfully completed using GCC version 7.1.0 or newer
and clang version 3.8.0 or newer. Warning: using an older version of the
compiler can result in compilation errors.

The graph computation in GfaViz is performed by the OGDF library, which comes prebundled with GfaViz.

### Building

The following code will install GfaViz:
```
git clone https://github.com/ggonnella/gfaviz
cd gfaviz
qmake-qt5
make
```

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

Multiple options are available for changing the visual representation of specific kind of elements. This section does not handle the label styles (see the following section for them).

#### GUI

To change the style of specific element types, use the label options in the element type tab in the Style widget.
To change the style of specific elements, select the element and use the options in the element type tab, making sure that the checkbox "apply changes to selection only" is on.

#### CLI

Several options can be used for changing the representation style of elements.
The options names are constructed by indicating first the type of objects on which the option applies (seg for segments; edge; dovetail and internal for particular kind of edges, i.e. dovetail overlaps and internal overlaps; group; gap; fragment) followed by the kind of option, e.g. width, outline width, color, etc. The name of the options combined with the help obtained using the ``--help`` command line option should help in identifying the function of each option.

##### Segments

The representation of a segment is a polygon, which aims at being proportional to the length of the DNA segment which it represents. The polygon itself has a color (``--seg-color``) and a given width (``--seg-width``). Furthermore, it has, by default, a colored outline (``--seg-outline-color``), which can be disabled by setting its width to zero (``--seg-outline-width``). The segments can be represented optionally as arrows (``--seg-as-arrow``), which is appropriate for some kind of graphs to indicate the directionality of the DNA strand.

A weight factor, applied to segments and fragments, controls the length of the segment in proportion to the DNA length (``--weight-factor``). For a visible representation of very small segments, a minimal length is set by the ``--minweight`` option.

Segments are internally represented by computing the position of some points, e.g. their extreme points, and connecting them. If only extreme points would be connected, segments would be straight. For a visually better representation, the representation of segments is not always straight. Thus the segment is divided into subsegments, and some internal points are so connected to each other. The number of subsegments depends of the segment length. Using the option ``--seg-max-sub`` allows to control this behaviour. Setting this to 1 will draw all segments as straight lines, which will be desirable in some applications.

##### Edges

The representation of edges (GFA2) and the equivalent links and containments (GFA1) is controlled using the following options. The width of the line representing the edge is set by ``--edge-width``, its color by ``--edge-color``.

Further options differentiate between internal edges (which also affect containments) and dovetail edges. The options ``--internal-width``, ``--internal-color``, ``--dovetail-width``, ``--dovetail-color`` allow to set the width and color of the two kind of edges independently. The options ``--internal-length`` and ``--dovetail-length`` allow to set the length of the edges of the two kind. By default internal edges have a longer length, so that they affect less the layout of the segments.

The position of the alignment of edges on the segments can be visualized as colored polygons on the segment. These are called "highlights" in GfaViz. To turn on highlights, use the ``--edge-highlights-show`` option. The color can be set using the ``--edge-highlights--color`` option. To use different random colors for each highlight, use ``--edge-highligths-color-random``. 

##### Gaps

Gaps are represented by dotted lines. By default, they connect segments, but their presence does not affect the layout. This can be changed by using the flag ``--gaps-as-edges``. If so, the length of the lines can be set by using ``--gap-length``.

Furthermore, the color of the line is set using ``--gap-color``. This however only affects positive-sized gaps (missing sequences). In some cases scaffolder add negative-sized gaps (overlaps of contigs, found during the scaffolding process). Their color can be changed using ``--neg-gap-color``. 

##### Fragments


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

The graph can be rendered to vector graphics and raster bitmap formats, using the following options:

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

Note that the options do not block the start of the GUI. In order to run GfaViz from the command line only, please use the ``--no-gui`` option.

### Stylesheets

Stylesheets allow to use the same style for different graphs. Usually style files will be created using the GUI from an existing graph (in a following version of this manual, the syntax for style files will be specified in detail, to support creating stylesheets from scratch). Examples of stylesheets are in the directory "style" of the repository.

For applying a stylesheet, the usestyle option shall be used:

```
  --usestyle <filename>          Use the style options represented by the
                                 stylesheet <filename>.
```

### Tree navigation

The tree navigation pane shows the content of the GFA file in form of a navigable tree. 

The functions of the tree are:
- obtain more information about an element, e.g. the length of a segment, or a tag of some element, without opening the text file
- selecting an element, by clicking on it on the tree
- exploring the connections of an element to other elements (e.g. edges connected to a link)

### Search for an element

In the search button, the ID of an element can be entered. This allows to search for any named element. Some elements always have an ID (segments), while other elements have an optional ID (e.g. edges, gaps, groups).

### List of options

To see a list of all available commands use: 
```
./viz.x --help
```

## Credits

The graph computation in GfaViz is performed by the OGDF library. Their excellent work can be found here: http://www.ogdf.net/

## License

The software is released under the ISC licence. Please see LICENSE.txt for details.

## Known issues

The GUI allows to select the format for saving the graph, between GFA1 and GFA2. Actually the file is always saved in the same format it was previously, regardless. The format selection option will be removed from the GUI in the following versions. To convert the format between GFA1 and GFA2, please use GfaPy.
