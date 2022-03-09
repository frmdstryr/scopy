#ifndef DATALOGGERTOOL_HPP
#define DATALOGGERTOOL_HPP

#include <QPair>
#include <QTimer>
#include <QWidget>
#include <QFileDialog>
#include <QRadioButton>

#include <filter.hpp>
#include <tool_launcher.hpp>
#include <tool.hpp>
#include "gui/tool_view_builder.hpp"

/* libm2k includes */
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/m2k.hpp>
#include <libm2k/analog/dmm.hpp>
#include <libm2k/contextbuilder.hpp>
#include <scopyExceptionHandler.h>
#include <libm2k/m2kexceptions.hpp>

namespace scopy{
namespace gui {
class GenericMenu;
class ChannelManager;

class DataLoggerToolGenericMenu;
}
}

namespace adiscope {
class ChannelWidget;
class CustomColQGridLayout;
class ChannelMonitorComponent;
class DataLoggerTool_API;
class DataLogger;
class CustomSwitch;


struct activeChannel{
	std::string dmmId;
	libm2k::analog::DMM* dmm;
	QColor chColor;

};

class DataLoggerTool : public Tool
{
	friend class DataLoggerTool_API;

    Q_OBJECT

public:
	explicit DataLoggerTool(struct iio_context *ctx, Filter *filt,
                        ToolMenuItem *toolMenuItem,
                        QJSEngine *engine, ToolLauncher *parent);
	~DataLoggerTool();

    scopy::gui::ToolView* getToolView();

	CustomSwitch* showAllSWitch;
	int getPrecision();
	void setPrecision(int precision);
	int getValueReadingTimeInterval();

private:
	int VALUE_READING_TIME_INTERVAL = 1000;

	QLineEdit *precisionValue;
	PositionSpinButton *recording_timer;

	QTimer* m_timer;
	DataLogger *dataLogger;
	QList<QColor> m_colors;
	QMap<int,QColor> m_color;
	scopy::gui::ToolView* m_toolView;
	CustomColQGridLayout* m_customColGrid;
	libm2k::context::Context* m_context;
	scopy::gui::ToolView* m_monitorToolView;
	std::vector<ChannelWidget*> m_channelList;
	std::vector<libm2k::analog::DMM*> m_dmmList;
	scopy::gui::GenericMenu* m_generalSettingsMenu;
	scopy::gui::ChannelManager* m_monitorChannelManager;
	QMap<int,QPair<activeChannel,ChannelMonitorComponent*>> m_activeChannels;

	QColor generateColor();
	void initMonitorToolView();
	QColor getChannelColor(int chId);
	void updateChannelWidget(int ch);
	scopy::gui::GenericMenu* generateMenu(QString title, QColor* color);
	std::vector<libm2k::analog::DMM*> getDmmList(libm2k::context::Context* m2k_context);
	void createConnections(scopy::gui::DataLoggerToolGenericMenu* mainMenu,scopy::gui::DataLoggerToolGenericMenu* menu,adiscope::ChannelMonitorComponent* monitor);

Q_SIGNALS:
	void PrecisionChanged(int precision);
	void updateValue(QString name, QString value);
	void RecordingIntervalChanged(double recordingInterval);
	void toggleAll(bool showAll);
	void disableActivateChannel(bool disable);

public Q_SLOTS:
    void readChannelValues();
    void toggleTimer(bool enabled);
};
}
#endif // DATALOGGERTOOL_HPP
