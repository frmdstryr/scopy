/*
 * Copyright (c) 2022 frmdstryr <frmdstryr@protonmail.com>
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
#include "decoder_table_model.hpp"

#include <QDebug>
#include <QHeaderView>

namespace adiscope {


namespace logic {

DecoderTableModel::DecoderTableModel(DecoderTable *decoderTable, LogicAnalyzer *logicAnalyzer):
    QAbstractTableModel(decoderTable),
    m_decoderTable(decoderTable),
    m_logic(logicAnalyzer)
{

}

int DecoderTableModel::rowCount(const QModelIndex &parent) const
{
    // Return max of all packets
    size_t count = 0;
    for (const auto &entry: m_dataset) {
        const auto &curve = entry.first;
        for (const auto &rowmap: curve->getAnnotationRows()) {
            if (entry.second.isEnabled(rowmap.first.index())) {
                count = std::max(count, rowmap.second.size());
            }
        }
    }
    // qDebug() << "Decoder table count: " << count << Qt::endl;
    return count;
}
int DecoderTableModel::columnCount(const QModelIndex &parent) const
{
    // One column per curve
    return m_dataset.size();
}

int DecoderTableModel::indexOfCurve(const AnnotationCurve *curve) const
{
    for (size_t i=0; i < m_curves.size(); i++) {
        if (m_curves[i] == curve) return i;
    }
    return -1;
}


void DecoderTableModel::activate()
{
    for (const auto &curve: m_curves) {
        if (curve.isNull()) continue;
        QObject::connect(
            curve,
            &AnnotationCurve::annotationsChanged,
            this,
            &DecoderTableModel::annotationsChanged
        );
    }
}

void DecoderTableModel::deactivate()
{
    // Disconnect signals
    for (const auto &curve: m_curves) {
        if (curve.isNull()) continue;
        disconnect(curve);
    }
    m_active = false;
}

void DecoderTableModel::reloadDecoders(bool logic)
{
    // Disconnect signals
    deactivate();

    // TODO: At some point this should only update changed
    m_curves.clear();
    m_dataset.clear();

    // Reconnect signals for all the annotation curves
    for (const auto &curve: m_logic->getPlotCurves(logic)) {
		if (const auto annCurve = dynamic_cast<AnnotationCurve *>(curve)) {
            m_curves.emplace_back(annCurve);
            m_dataset.emplace(annCurve, displayInfo(annCurve));
		}
    }

    // Reconnect signals
    activate();

    // Recompute samples
    annotationsChanged();
}


inline bool DecoderTableModel::DisplayInfo::isEnabled(int row) const
{
    return (row < 0 or row >= 32) ? false: rowMask.test(row);
}

void DecoderTableModel::setDisplayInfo(const AnnotationCurve* curve, DecoderTableModel::DisplayInfo info)
{
    for (const auto &it: m_dataset) {
        if (it.first == curve) {
            m_dataset[it.first] = info;
            break;
        }
    }
}

DecoderTableModel::DisplayInfo DecoderTableModel::displayInfo(const AnnotationCurve* curve) const
{
    assert(curve);
    for (const auto &rowmap: curve->getAnnotationRows()) {
        const Row &row = rowmap.first;
        const auto title = row.title();
        if (title == "USB packet: Packets") {
            return {row.index(), 0xFFFFFFFF - 3};
        } else if (title == "Parallel: Items") {
            return {row.index(), 0xFFFFFFFF};
        }
        // TODO: What are the others???
    }
    qDebug() << "No mapping found for decoder table" << Qt::endl;
    return {-1, 0};
}

void DecoderTableModel::annotationsChanged()
{
    beginResetModel();

    int visibleRows = 0;
    int rowHeight = 20;
    for (const auto &entry: m_dataset) {
        const auto &curve = entry.first;
        const auto info = entry.second;
        if (curve.isNull()) continue;
        int n = curve->getVisibleRows();
        for (const auto &rowmap: curve->getAnnotationRows()) {
            const Row &row = rowmap.first;
            const RowData &data = rowmap.second;
            const auto i = row.index();
            // Don't take up space for disabled rows
            if (!info.isEnabled(i)) {
                n -= 1;
            }

            // Debugging only
            const char* active = (info.primaryRow == i) ? " (active)": "";
            qDebug() << "decoder: " << i << "-"
                << row.title() << " size: " << data.size() << active << Qt::endl;

        }

        visibleRows = std::max(visibleRows, n);
        rowHeight = std::max(rowHeight, static_cast<int>(curve->getTraceHeight()));
    }
    endResetModel();

    const auto verticalHeader = m_decoderTable->verticalHeader();
    const int spacing = 5;
    verticalHeader->setDefaultSectionSize(rowHeight * visibleRows + spacing);
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
}

QVariant DecoderTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole and orientation == Qt::Horizontal) {
        if (section < 0 or static_cast<size_t>(section) >= m_curves.size()) {
            return QVariant();
        }

        const auto &curve = m_curves.at(section);
        if (curve.isNull()) {
            return QVariant();
        }

        const auto info = m_dataset.at(curve);

        // Use decoder class name
        for (const auto &entry: curve->getAnnotationRows()) {
            const Row &row = entry.first;
            if (row.index() != info.primaryRow) continue;
            if (row.title().size() > 0) {
                return row.title();
            }
        }
        return QString("No decoder");
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant DecoderTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole) {
        const size_t col = index.column();
        if ( col >= m_curves.size() ) return QVariant();
        const auto &curve = m_curves.at(col);
        if (curve.isNull()) return QVariant();

        const auto info = m_dataset.at(curve);
        if (info.primaryRow < 0) return QVariant();

        // Get annotation data by row
        const size_t row = index.row();
        for (const auto &entry: curve->getAnnotationRows()) {
            if (entry.first.index() != info.primaryRow) continue;
            const RowData& data = entry.second;
            if (row < data.size()) {
                const Annotation &ann = data.getAnnAt(row);
                return QVariant::fromValue(DecoderTableItem(
                    curve,
                    info.rowMask.to_ulong(),
                    ann.start_sample(),
                    ann.end_sample()
                ));
            }
        }
    }
    return QVariant();
}

} // namespace logic
} // namespace adiscope
