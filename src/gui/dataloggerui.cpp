#include "dataloggerui.hpp"

using namespace adiscope;

DataLoggerUI::DataLoggerUI(bool lastValue, bool average, bool all, QWidget *parent) :
	QWidget(parent)
{

	dataLoggingWidget = new QWidget(this);
	dataLoggingLayout = new QVBoxLayout(dataLoggingWidget);

	dataLoggerFilter = new QComboBox(dataLoggingWidget);
	if(lastValue) {	dataLoggerFilter->addItem(QString("Last Value")); }
	if(average) {	dataLoggerFilter->addItem(QString("Average")); }
	if(all) {	dataLoggerFilter->addItem(QString("All")); }
	dataLoggerFilter->setCurrentIndex(0);
	dataLoggerFilter->setDisabled(true);

	init();
}


void DataLoggerUI::init()
{
	dataLoggingSwitch = new CustomSwitch(dataLoggingWidget);
	dataLoggingLayout->addWidget(dataLoggingSwitch);
	dataLoggingLayout->setAlignment(dataLoggingSwitch,Qt::AlignRight);

	dataLoggingLayout->addWidget(new QLabel("Choose file"));
	dataLoggingFilePath = new QLineEdit(dataLoggingWidget);
	dataLoggingFilePath->setReadOnly(true);
	dataLoggingFilePath->setDisabled(true);

	connect(dataLoggingFilePath, &QLineEdit::textChanged, this, [=](QString path){
		if(filename.isEmpty() && dataLoggingFilePath->isEnabled()) {
			dataLoggingFilePath->setText(tr("No file selected"));
			dataLoggingFilePath->setStyleSheet("color:red");
		}else{
			dataLoggingFilePath->setStyleSheet("color:white");
			Q_EMIT pathChanged(path);
		}
	});

	dataLoggingLayout->addWidget(dataLoggingFilePath);
	dataLogginBrowseBtn = new QPushButton("Browse");
	dataLogginBrowseBtn->setProperty("blue_button",true);
	dataLogginBrowseBtn->setMinimumHeight(30);
	dataLogginBrowseBtn->setDisabled(true);
	dataLoggingLayout->addWidget(dataLogginBrowseBtn);

	connect(dataLogginBrowseBtn, &QPushButton::clicked,
			this, [=](){
		chooseFile();
	});

	overwriteRadio = new QRadioButton("Overwrite");
	overwriteRadio->setChecked(true);
	overwriteRadio->setDisabled(true);
	dataLoggingLayout->addWidget(overwriteRadio);
	appendRadio = new QRadioButton("Append");
	dataLoggingLayout->addWidget(appendRadio);
	appendRadio->setDisabled(true);

	connect(overwriteRadio, &QRadioButton::toggled, [=](bool en) {
		appendRadio->setChecked(!en);
	});

	connect(appendRadio, &QRadioButton::toggled, [=](bool en) {
		overwriteRadio->setChecked(!en);
	});

	data_logging_timer = new PositionSpinButton({
																		{"s", 1},
																		{"min", 60},
																		{"h", 3600}
																	}, tr("Timer"), 0, 3600,
																	true, false, this);

	data_logging_timer->setValue(5);
	data_logging_timer->setDisabled(true);

	connect(data_logging_timer, &PositionSpinButton::valueChanged, this, [=](){
		Q_EMIT timeIntervalChanged(data_logging_timer->value() * 1000); //converts to seconds before emiting value
	});

	dataLoggingLayout->addWidget(data_logging_timer);
	dataLoggingLayout->addWidget(dataLoggerFilter);


	warningMessage = new QLabel("");
	warningMessage->setStyleSheet("color:white;");
	warningMessage->setWordWrap(true);
	warningMessage->setVisible(false);

	dataLoggingLayout->addWidget(warningMessage);

	//on data logging switch pressed enable/disable data logging section and emit data loggin toggled
	connect(dataLoggingSwitch,  &CustomSwitch::toggled, this, [=](bool toggled){
		Q_EMIT toggleDataLogger(toggled);
		enableDataLoggerFields(toggled);
	});

}

void DataLoggerUI::chooseFile()
{
	QString selectedFilter;

	filename = QFileDialog::getSaveFileName(this,
											tr("Export"), "", tr("Comma-separated values files (*.csv);;All Files(*)"),
											&selectedFilter, QFileDialog::Options());
	dataLoggingFilePath->setText(filename);
}


QWidget* DataLoggerUI::getDataLoggerUIWidget()
{
	return dataLoggingWidget;
}


bool DataLoggerUI::isDataLoggingOn()
{
	return dataLoggingSwitch->isChecked();
}

bool DataLoggerUI::isOverwrite()
{
	return overwriteRadio->isChecked();
}

QString DataLoggerUI::getFilter()
{
	return dataLoggerFilter->currentText();
}

void DataLoggerUI::setWarningMessage(QString message)
{
	warningMessage->setText(message);
}

void DataLoggerUI::enableDataLoggerFields(bool en)
{
	dataLoggingFilePath->setDisabled(!en);
	dataLogginBrowseBtn->setDisabled(!en);
	overwriteRadio->setDisabled(!en);
	appendRadio->setDisabled(!en);
	data_logging_timer->setDisabled(!en);
	dataLoggerFilter->setDisabled(!en);
	warningMessage->setVisible(en);

	if(filename.isEmpty() && dataLoggingFilePath->isEnabled()) {
		dataLoggingFilePath->setText(tr("No file selected"));
		dataLoggingFilePath->setStyleSheet("color:red");
	}
}

void DataLoggerUI::disableDataLogging(bool en)
{
	dataLoggingSwitch->setEnabled(!en);
}

void DataLoggerUI::toggleDataLoggerSwitch(bool toggle)
{
	dataLoggingSwitch->setChecked(toggle);
}

void DataLoggerUI::setDataLoggerPath(QString path)
{
	filename = path;
	dataLoggingFilePath->setText(path);
}

QString DataLoggerUI::getDataLoggerPath()
{
	return filename;
}

void DataLoggerUI::setOverwrite(bool en)
{
	if(en){
		overwriteRadio->setChecked(true);
	}else{
		appendRadio->setChecked(true);
	}
}

int DataLoggerUI::getTimerInterval()
{
	return data_logging_timer->value();
}

void DataLoggerUI::setTimerInterval(int interval)
{
	data_logging_timer->setValue(interval);
}

void DataLoggerUI::isDataLoggerRunning(bool en)
{
	overwriteRadio->setDisabled(en);
	appendRadio->setDisabled(en);
}

DataLoggerUI::~DataLoggerUI()
{
	delete dataLoggingWidget;
	delete dataLoggingLayout;
	delete dataLoggingFilePath;
	delete overwriteRadio;
	delete appendRadio;
	delete dataLoggingSwitch;
	delete dataLoggerFilter;
	delete dataLogginBrowseBtn;
}

