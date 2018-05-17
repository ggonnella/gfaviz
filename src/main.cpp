#include "vizapp.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
    case QtDebugMsg:
      fprintf(stderr, "Debug: %s\n", localMsg.constData());
      break;
    case QtInfoMsg:
      fprintf(stderr, "Info: %s\n", localMsg.constData());
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s\n", localMsg.constData());
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Error: %s\n", localMsg.constData());
      exit(1);
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal error: %s\n", localMsg.constData());
      abort();
  }
}

int main(int argc, char *argv[]) {
  qInstallMessageHandler(messageHandler);
  VizApp app(argc, argv);
  return app.exec();
}
