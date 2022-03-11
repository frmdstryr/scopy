#include "dataloggertoolgenericmenu.hpp"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "customSwitch.hpp"

using namespace adiscope::gui;

DataLoggerToolGenericMenu::DataLoggerToolGenericMenu(QWidget *parent):
	GenericMenu(parent)
{
	connect(this, &DataLoggerToolGenericMenu::historySizeIndexChanged, this, &DataLoggerToolGenericMenu::setHistorySize );
	connect(this, &DataLoggerToolGenericMenu::lineStyleIndexChanged, this, &DataLoggerToolGenericMenu::changeLineStyle );
}

DataLoggerToolGenericMenu::~DataLoggerToolGenericMenu()
{}

void DataLoggerToolGenericMenu::init(QString title, QColor* color, bool showAll){
	this->initInteractiveMenu();
	this->setMenuHeader(title,color,false);

	auto *scaleSection = new adiscope::gui::SubsectionSeparator("Settings", false,this);

	//show all
	if(showAll){
		QWidget *showAllWidget = new QWidget(this);
		auto showAllLayout = new QHBoxLayout(showAllWidget);
		auto showAllSWitch = new CustomSwitch(showAllWidget);

		showAllLayout->addWidget(new QLabel("Show all",showAllWidget));
		showAllLayout->addWidget(showAllSWitch);

		scaleSection->setContent(showAllWidget);
		connect(showAllSWitch, &CustomSwitch::toggled, this, [=](bool toggled){
			Q_EMIT DataLoggerToolGenericMenu::toggleAll(toggled);
		});
	}

	//scale
	QWidget *scaleWidget = new QWidget(this);
	auto scaleLayout = new QHBoxLayout(scaleWidget);
	scaleSWitch = new CustomSwitch(scaleWidget);
	scaleSWitch->setChecked(true);

	scaleLayout->addWidget(new QLabel("Scale",scaleWidget));
	scaleLayout->addWidget(scaleSWitch);

	scaleSection->setContent(scaleWidget);

	////history
	auto *historyWidget = new QWidget(this);
	auto *historyLayout = new QVBoxLayout(historyWidget);
	auto *h1layout = new QHBoxLayout(historyWidget);

	historySwitch = new CustomSwitch(historyWidget);
	historySwitch->setChecked(true);
	historyLayout->addWidget(historySwitch);
	historyLayout->setAlignment(historySwitch,Qt::AlignRight);

	historySize = new QComboBox(historyWidget);
	historySize->addItem(QString("1s"));
	historySize->addItem(QString("10s"));
	historySize->addItem(QString("60s"));
	historySize->setCurrentIndex(1);

	auto *historySizeLabel = new QLabel("History Size",historyWidget);
	h1layout->addWidget(historySizeLabel);
	h1layout->addWidget(historySize);

	historyLayout->addItem(h1layout);

	auto *h2layout = new QHBoxLayout(historyWidget);
	historyStyle = new QComboBox(historyWidget);
	historyStyle->addItem(QString("Solid Line"));
	historyStyle->addItem(QString("Dot Line"));
	historyStyle->addItem(QString("Dash Line"));
	historyStyle->addItem(QString("Dash-Dot Line"));
	historyStyle->addItem(QString("Dash-Dot-Dot Line"));
	historyStyle->setCurrentIndex(0);

	auto *historyStyleLabel = new QLabel("History Style",historyWidget);

	h2layout->addWidget(historyStyleLabel);
	h2layout->addWidget(historyStyle);
	historyLayout->addItem(h2layout);

	auto *historySection = new adiscope::gui::SubsectionSeparator("History", true,this);
	historySection->setContent(historyWidget);
	////end history

	////Peak holder
	auto *peakHolderWidget = new QWidget(this);
	auto *peakHolderLayout = new QHBoxLayout(peakHolderWidget);
	peakHolderSwitch = new CustomSwitch(peakHolderWidget);
	peakHolderSwitch->setChecked(true);
	peakHolderReset = new QPushButton("Reset",peakHolderWidget);
	peakHolderReset->setProperty("blue_button",true);
	peakHolderReset->setMinimumHeight(30);

	peakHolderLayout->addWidget(peakHolderReset);
	peakHolderLayout->addWidget(peakHolderSwitch);



	auto *peakHolderSection = new adiscope::gui::SubsectionSeparator("Peak Holder", true,this);
	peakHolderSection->setContent(peakHolderWidget);
	///end peak holder

	this->insertSection(scaleSection);
	this->insertSection(historySection);
	this->insertSection(peakHolderSection);

	this->layout()->addItem( new QSpacerItem(10,10, QSizePolicy::Fixed, QSizePolicy::Expanding));

	connect(scaleSWitch, &CustomSwitch::toggled, this, &DataLoggerToolGenericMenu::toggleScale);
	connect(historySwitch,  &CustomSwitch::toggled, this, &DataLoggerToolGenericMenu::toggleHistory);
	connect(peakHolderReset, &QPushButton::clicked,this, &DataLoggerToolGenericMenu::resetPeakHolder);
	connect(peakHolderSwitch, &CustomSwitch::toggled, this, &DataLoggerToolGenericMenu::togglePeakHolder);
	connect(historySize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DataLoggerToolGenericMenu::historySizeIndexChanged);
	connect(historyStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DataLoggerToolGenericMenu::lineStyleIndexChanged);
}

void DataLoggerToolGenericMenu::scaleToggle(bool toggled)
{
	scaleSWitch->setChecked(toggled);
}

void DataLoggerToolGenericMenu::peakHolderToggle(bool toggled)
{
	peakHolderSwitch->setChecked(toggled);
}

void DataLoggerToolGenericMenu::historyToggle(bool toggled)
{
	historySwitch->setChecked(toggled);
}

void DataLoggerToolGenericMenu::peakHolderResetClicked()
{
	peakHolderReset->click();
}

void DataLoggerToolGenericMenu::historySizeChanged(int size)
{
	historySize->setCurrentIndex(size);
	setHistorySize(size);
}

void DataLoggerToolGenericMenu::changeLineStyle(int index)
{
	historyStyle->setCurrentIndex(index);
	Q_EMIT lineStyleChanged(lineStyleFromIdx(index));
}

void DataLoggerToolGenericMenu::setHistorySize(int idx)
{
	Q_EMIT changeHistorySize(numSamplesFromIdx(idx));
}

double DataLoggerToolGenericMenu::numSamplesFromIdx(int idx)
{
	switch(idx) {
	case 0:	return 1.0;
	case 1:	return 10.0;
	case 2:	return 60.0;
	default:throw std::runtime_error("Invalid INDEX");
	}
}

Qt::PenStyle DataLoggerToolGenericMenu::lineStyleFromIdx(int idx)
{
	switch (idx) {
	case 0:	return Qt::PenStyle::SolidLine;
	case 1:	return Qt::PenStyle::DotLine;
	case 2:	return Qt::PenStyle::DashLine;
	case 3:	return Qt::PenStyle::DashDotLine;
	case 4: return Qt::PenStyle::DashDotDotLine;
	}
	return Qt::PenStyle::SolidLine;
}

