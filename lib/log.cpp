#include "log.h"

int logLevel = 4;
FILE *logFile = stderr;

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg) {
    if (type < logLevel) return;
    QMutex mutex;
    QMutexLocker locker(&mutex);

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QByteArray datetime = QDateTime::currentDateTime()
                              .toString("yyyy-MM-dd hh.mm.ss.zzz")
                              .toLocal8Bit();
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
        case QtDebugMsg:  // 0
            fprintf(stderr, "%s Debug: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
        case QtInfoMsg:  // 4
            fprintf(stderr, "%s Info: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
        case QtWarningMsg:  // 1
            fprintf(stderr, "%s Warning: %s (%s:%u, %s)\n",
                    datetime.constData(), localMsg.constData(), file,
                    context.line, function);
            break;
        case QtCriticalMsg:  // 2
            fprintf(stderr, "%s Critical: %s (%s:%u, %s)\n",
                    datetime.constData(), localMsg.constData(), file,
                    context.line, function);
            break;
        case QtFatalMsg:  // 3
            fprintf(stderr, "%s Fatal: %s (%s:%u, %s)\n", datetime.constData(),
                    localMsg.constData(), file, context.line, function);
            break;
    }
}
