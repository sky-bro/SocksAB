#ifndef LOG_H
#define LOG_H
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>

// enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg,
// QtInfoMsg, QtSystemMsg = QtCriticalMsg };
extern int logLevel;
extern FILE *logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg);

#endif  // LOG_H
