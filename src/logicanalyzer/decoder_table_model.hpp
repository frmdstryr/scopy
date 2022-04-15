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

#ifndef DECODER_TABLE_MODEL_H
#define DECODER_TABLE_MODEL_H

#include <bitset>
#include <QAbstractTableModel>
#include "annotationcurve.h"
#include "decoder_table.hpp"
#include "decoder_table_item.hpp"


namespace adiscope {


namespace logic {

class DecoderTableModel : public QAbstractTableModel {
    Q_OBJECT

public:

    struct DisplayInfo {
        inline bool isEnabled(int row) const;

        // Primary row to use in the table
        int primaryRow;
        // Set of rows to render in the table
        std::bitset<32> rowMask;
    };

    DecoderTableModel(DecoderTable *decoderTable, LogicAnalyzer *logicAnalyzer);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Return index of the decoder row used for determing table rows
    // Ideally this should be a full "packet" if the protocol being decoded
    // uses packets. Otherwise return -1 to skip the annotation
    DisplayInfo displayInfo(const AnnotationCurve* curve) const;
    void setDisplayInfo(const AnnotationCurve* curve, DisplayInfo info);

    // activate connects signals to listen for new decoded messages
    // deactivate disconnects them.
    void activate();
    void deactivate();

    // Get index of the curve
    int indexOfCurve(const AnnotationCurve* curve) const;

public Q_SLOTS:

    // This slot should be used when any of the decoders / annotation curves
    // have been modified (eg adding/removing decoders, etc..). This clears
    // the curves vecotr reconnects up signals to watch for annotation changes.
    void reloadDecoders(bool logic);

    // Slot that should be invoked when new annotations are decoded
    void annotationsChanged();

protected:

    // Set of curves to observe. Each should have an entry in the dataset
    std::vector<QPointer<AnnotationCurve>> m_curves;

    // Mapping of the curve to the row used to determine a single row in
    // the decode table. Typically a packet.
    std::map<QPointer<AnnotationCurve>, DisplayInfo> m_dataset;

    DecoderTable *m_decoderTable;
    LogicAnalyzer *m_logic;
    bool m_active = false;

};

} // namespace logic
} // namespace adiscope

#endif // DECODER_TABLE_MODEL_H

