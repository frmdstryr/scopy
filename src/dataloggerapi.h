#ifndef DATALOGGERAPI_H
#define DATALOGGERAPI_H

#include <QObject>
#include <qvector.h>
#include <QTimer>
#include <QMap>
#include <filemanager.h>
#include <QDateTime>
#include <QtConcurrent>

namespace adiscope {

enum class Type{
	INT,
	DOUBLE,
	QSTRING
};

enum class ch_Filter{
	LAST_VALUE,
	AVERAGE,
	ALL
};

struct Channel{
	Type type;
	ch_Filter filter;
	QVector<QString> values;
};

class DataLoggerAPI: public QObject
{
	Q_OBJECT

public:
	DataLoggerAPI();
	~DataLoggerAPI();
	void createChannel(QString name, Type type, ch_Filter filter);
	void distroyChannel(QString name);
	void resetChannel(QString name);
	void logData();

public Q_SLOTS:
	void receiveValue(QString name, QString value);
	void setPath(QString path);
	void setTimerInterval(double interval);
	void startLogger(bool overwrite);
	void stopLogger();

private:
	QString m_path;
	QTimer  *m_timer;
	QMap<QString,Channel> m_channels;
	const QString SEPARATOR = ",";

	double computeAvg(QVector<QString> values);
	bool isNumber(const QString &str);
};
}
#endif // DATALOGGERAPI_H
