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
#ifndef DECODER_TABLE_ITEM_H
#define DECODER_TABLE_ITEM_H

#include "annotationcurve.h"
#include <bitset>
#include <QPainter>
#include <QStyledItemDelegate>


namespace adiscope {
namespace logic {


class DecoderTableItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;
    virtual ~DecoderTableItemDelegate() {};

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};



class DecoderTableItem {

public:

    explicit DecoderTableItem(
        AnnotationCurve* curve=nullptr,
        uint32_t mask=0,
        uint64_t start=0,
        uint64_t end=0
    );

    void paint(QPainter *painter, const QRect &rect, const QPalette &palette) const;
    QSize sizeHint() const;

    double startTime() const;
    double endTime() const;

    // Check if disabled by the mask
    inline bool isRowEnabled(int row) const;

    AnnotationCurve* curve;
    // Bitfield for each row
    std::bitset<32> rowMask;
    uint64_t startSample;
    uint64_t endSample;
    QSize itemSize = QSize(300, 40);

};

} // namespace logic
} // namespace adiscope


// This is needed so it can be stored as a QVariant
Q_DECLARE_METATYPE(adiscope::logic::DecoderTableItem)



#endif // DECODER_TABLE_ITEM_H
