#include "dataloggerapi.h"

using namespace adiscope;

DataLoggerAPI::DataLoggerAPI():
	m_path(""),
	m_timer(new QTimer())
{
	QObject::connect(m_timer, &QTimer::timeout , this, [=](){
		QtConcurrent::run(this, &DataLoggerAPI::logData);
	});
}

void DataLoggerAPI::setPath(QString path)
{
	m_path = path;
}

void DataLoggerAPI::setTimerInterval(double interval)
{
	m_timer->setInterval(interval);
}

void DataLoggerAPI::startLogger(bool overwrite)
{
	m_timer->start();
	//when overwrite also create file header
	if(overwrite){
		FileManager *fm = new FileManager("Data Logger");
		fm->open(m_path, FileManager::EXPORT);

		QStringList chNames;
		if(!m_channels.empty()){
			for(auto ch: m_channels.keys()){
				chNames.push_back(ch);
			}
		}
		QMap<QString,QVector<QString>> dataToLog;

		dataToLog["Timestamp"]= {};
		if(!m_channels.empty()){
			for(auto ch: m_channels.keys()){
				dataToLog[ch]= {};
			}
		}
		fm->writeToFile(overwrite,dataToLog);
	}
}

void DataLoggerAPI::stopLogger()
{
	if(m_timer->isActive()){
		m_timer->stop();
	}
}

void DataLoggerAPI::createChannel(QString name, Type type, ch_Filter filter)
{
	m_channels[name].type = type;
	m_channels[name].filter = filter;
}

void DataLoggerAPI::distroyChannel(QString name)
{
	m_channels.remove(name);
}

void DataLoggerAPI::resetChannel(QString name)
{
	m_channels[name].values.clear();
}

void DataLoggerAPI::receiveValue(QString name, QString value)
{
	m_channels[name].values.push_back(value);
}

void DataLoggerAPI::logData()
{
	QMap<QString,QVector<QString>> dataToLog;
	QString data = "";

	for(auto ch: m_channels.keys()){
		if(ch == "Timestamp"){continue;}
		data = "";
		//apply filter on stored data
		if(m_channels[ch].filter == ch_Filter::LAST_VALUE && !m_channels[ch].values.empty()){
			data += m_channels[ch].values.last();
			dataToLog[ch].push_back(data);
			resetChannel(ch);
			continue;
		}
		if(m_channels[ch].filter == ch_Filter::AVERAGE && !m_channels[ch].values.empty()){
			double avg = computeAvg(m_channels[ch].values);
			dataToLog[ch].push_back(QString::number(avg));
			resetChannel(ch);
			continue;
		}
		if(m_channels[ch].filter == ch_Filter::ALL && !m_channels[ch].values.empty()){
			for(auto v : m_channels[ch].values){
				data += v + " ";
			}
			dataToLog[ch].push_back(data);
			resetChannel(ch);
		}
		resetChannel(ch);
	}

	FileManager *fm = new FileManager("Data Logger");
	fm->open(m_path, FileManager::EXPORT);

	QStringList chNames;
	if(!m_channels.empty()){
		for(auto ch: m_channels.keys()){
			chNames.push_back(ch);
		}
	}
	dataToLog["Timestamp"]={QDateTime::currentDateTime().time().toString()};
	fm->writeToFile(false,dataToLog);
}

double DataLoggerAPI::computeAvg(QVector<QString> values)
{
	double result = 0;
	for(QString v : values){
		assert(isNumber(v));
		result += v.toDouble();
	}
	result = result / values.size();
	return result;
}

bool DataLoggerAPI::isNumber(const QString& str)
{
	for (QChar const &c : str) {
		if (!( c.isDigit() || c == ".")) return false;
	}
	return true;
}

DataLoggerAPI::~DataLoggerAPI()
{
	delete m_timer;
}
