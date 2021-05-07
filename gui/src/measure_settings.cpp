/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "ui_measure_settings.h"

#include <QHeaderView>
#include <QStandardItem>
#include <QTreeView>

#include <scopy/gui/dropdown_switch_list.hpp>
#include <scopy/gui/measure_settings.hpp>

using namespace scopy::gui;

MeasureSettings::MeasureSettings(GenericMenu* parent)
	: GenericMenu(parent)
	, m_ui(new Ui::MeasureSettings)
	, m_channelName("")
	, m_chnUnderlineColor()
	, m_horizMeasurements(new DropdownSwitchList(2, this))
	, m_vertMeasurements(new DropdownSwitchList(2, this))
	, m_emitActivated(true)
	, m_emitStatsChanged(true)
	, m_emitDeleteAll(true)
	, m_emitStatsDeleteAll(true)
	, m_are_dropdowns_filled(false)
	, m_enableDisplayAll(false)
	, m_selectedChannel(-1)
{
	m_ui->setupUi(this);

	initUi();

	connect(m_ui->widgetMenuHeader->getEnableBtn(), &QPushButton::toggled,
		[=](bool toggled) { Q_EMIT enableBtnToggled(toggled); });
}

void MeasureSettings::initUi()
{
	QTreeView* treeView;

	m_ui->hLayoutHMeasurements->addWidget(m_horizMeasurements);
	m_ui->hLayoutVMeasurements->addWidget(m_vertMeasurements);

	m_horizMeasurements->setTitle(tr("Horizontal"));
	m_horizMeasurements->setColumnTitle(0, tr("Name"));
	m_horizMeasurements->setColumnTitle(1, tr("Measure"));
	m_horizMeasurements->setColumnTitle(2, tr("Stats"));
	m_horizMeasurements->setMaxVisibleItems(4);
	treeView = static_cast<QTreeView*>(m_horizMeasurements->view());
	treeView->header()->resizeSection(0, 122);
	treeView->setIconSize(QSize(30, 20));

	m_vertMeasurements->setTitle(tr("Vertical"));
	m_vertMeasurements->setColumnTitle(0, tr("Name"));
	m_vertMeasurements->setColumnTitle(1, tr("Measure"));
	m_vertMeasurements->setColumnTitle(2, tr("Stats"));
	m_vertMeasurements->setMaxVisibleItems(4);
	treeView = static_cast<QTreeView*>(m_vertMeasurements->view());
	treeView->header()->resizeSection(0, 122);
	treeView->setIconSize(QSize(30, 20));

	m_ui->widgetStatisticsSubsSep->setLabel("STATISTICS");
	m_ui->widgetStatisticsSubsSep->setButtonVisible(false);

	m_ui->widgetMeasurementsSubsSep->setLabel("MEASUREMENTS");
	m_ui->widgetMeasurementsSubsSep->setButtonVisible(false);

	m_ui->widgetGatingSettingsSubsSep->setLabel("GATING SETTINGS");
	m_ui->widgetGatingSettingsSubsSep->setButtonVisible(false);

	m_ui->widgetCustomSelectionSubsSep->setLabel("CUSTOM SELECTION");
	m_ui->widgetCustomSelectionSubsSep->setButtonVisible(false);

	m_ui->widgetMenuHeader->setEnableBtnVisible(true);
}

MeasureSettings::~MeasureSettings() { delete m_ui; }

QString MeasureSettings::channelName() const { return m_channelName; }

void MeasureSettings::setChannelName(const QString& name)
{
	m_channelName = name;
	m_ui->widgetMenuHeader->setLabel("Measure " + name);
}

QColor MeasureSettings::channelUnderlineColor() const { return m_chnUnderlineColor; }

void MeasureSettings::setChannelUnderlineColor(const QColor* color)
{
	m_chnUnderlineColor = color->name();
	m_ui->widgetMenuHeader->setLineColor(color);
}

bool MeasureSettings::emitActivated() const { return m_emitActivated; }
void MeasureSettings::setEmitActivated(bool en) { m_emitActivated = en; }

bool MeasureSettings::emitStatsChanged() const { return m_emitStatsChanged; }
void MeasureSettings::setEmitStatsChanged(bool en) { m_emitStatsChanged = en; }

QList<MeasurementItem> MeasureSettings::measurementSelection() { return m_selectedMeasurements; }

QList<MeasurementItem> MeasureSettings::statisticSelection()
{
	QList<MeasurementItem> statistics;

	for (int i = 0; i < m_selectedStatistics.size(); i++)
		statistics.push_back(m_selectedStatistics[i].measurementItem);

	return statistics;
}

void MeasureSettings::setMenuButton(bool toggled) { m_ui->widgetMenuHeader->setEnabledBtnState(toggled); }

void MeasureSettings::activateDisplayAll() { m_ui->btnMeasDisplayAll->setChecked(m_enableDisplayAll); }
