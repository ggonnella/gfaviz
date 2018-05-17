#include "vizapp.h"

#include <QFileInfo>

QCoreApplication* createApplication(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
        if (!qstrcmp(argv[i], "-no-gui"))
            return new QCoreApplication(argc, argv);
    return new QApplication(argc, argv);
}
VizApp::VizApp(int & argc, char *argv[]) : QApplication(argc, argv) {
  parseArguments();
  if (settings.guiEnabled) {
    window = new QMainWindow();
    ui.setupUi(window);
    window->show();
  }
  
  const QStringList& filenames = optionParser.positionalArguments();
  for (int i = 0; i < filenames.size(); i++) {
    open(filenames[i]);
  }
  //ui.vizCanvas->scale(2.0,2.0);
}

void VizApp::open(const QString& filename) {
  try {
    VizGraph *g = new VizGraph(filename, settings.width, settings.height);
    graphs.push_back(g);
    if (settings.renderEnabled) {
      g->renderToFile(getOutputFilename(filename, graphs.size()), settings.outputFormat);
    }
    if (settings.guiEnabled) {
      g->setDisplaySize(1024,640);
      ui.tabWidget->addTab(g, filename);
    }
  }
  catch (const exception& e) {
    cerr << "Fatal error: " << e.what() << endl << endl;
  }
}
QString VizApp::getOutputFilename(QString filename, int n) {
  if (settings.outputFile == "") {
    QFileInfo fi(filename);
    return fi.fileName();
  }
  return settings.outputFile + (n > 1 ? "." + QString::number(n-1) : "");
}
static bool isValidFormat(QString fmt) {
  fmt = fmt.toUpper();
  return (fmt == "BMP" ||
      fmt == "PNG" ||
      fmt == "JPG" ||
      fmt == "JPEG" ||
      fmt == "PBM" ||
      fmt == "XBM" ||
      fmt == "XPM" ||
      fmt == "SVG");
}
void VizApp::parseArguments() {
  optionParser.addHelpOption();  
  optionParser.addPositionalArgument("filenames", "Name of the file(s) to be opened.", "[filenames...]");
  
  QCommandLineOption optionDisableGui(QStringList() << "n" << "no-gui", "Disable GUI");
  optionParser.addOption(optionDisableGui);
  
  QCommandLineOption optionRender(QStringList() << "r" << "render", "Render graph(s) into file(s).");
  optionParser.addOption(optionRender);
  
  QCommandLineOption optionOutputFile(QStringList() << "o" << "output", "Render graph(s) into <filename>","filename","");
  optionParser.addOption(optionOutputFile);
  
  QCommandLineOption optionOutputFormat(QStringList() << "f" << "output-format", "File format for the output. If no value is specified, format will be inferred from the file suffix specified in the --output option. Possible values: BMP, PNG, JPG, JPEG, PBM, XBM, XPM, SVG. Default: PNG","format");
  optionParser.addOption(optionOutputFormat);
  
  QCommandLineOption optionWidth(QStringList() << "W" << "width", "Width of the output file in pixels.", "width", "1280");
  optionParser.addOption(optionWidth);
  
  QCommandLineOption optionHeight(QStringList() << "H" << "height", "Height of the output file in pixels.", "height", "720");
  optionParser.addOption(optionHeight);
            
  optionParser.process(*this);
  
  settings.renderEnabled = (optionParser.isSet(optionOutputFile) || optionParser.isSet(optionRender));
  settings.guiEnabled = !optionParser.isSet(optionDisableGui);
  settings.outputFile = optionParser.value(optionOutputFile);
  
  settings.outputFormat = "png";
  int lastdot = settings.outputFile.lastIndexOf(".");
  if (lastdot != -1) {
    QString fmt = settings.outputFile.mid(lastdot+1);
    if (isValidFormat(fmt)) {
      settings.outputFormat = fmt;
      settings.outputFile.chop(fmt.length()+1);
    }
  }
  
  if (optionParser.isSet(optionOutputFormat)) {
    if (!settings.renderEnabled) {
      qWarning("--output-format option has no effect without --output or --render.");
    }
    settings.outputFormat = optionParser.value(optionOutputFormat);
    if (!isValidFormat(settings.outputFormat)) {
      qCritical("\"%s\" is not a valid output format!", qUtf8Printable(settings.outputFormat));
    }
  }
  settings.width = optionParser.value(optionWidth).toInt();
  settings.height = optionParser.value(optionHeight).toInt();
  //cout << outputFile.toStdString() << " " << outputFormat.toStdString() << endl;

}
