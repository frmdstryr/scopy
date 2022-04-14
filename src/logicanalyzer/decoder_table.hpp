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

#ifndef DECODER_TABLE_H
#define DECODER_TABLE_H

#include <QAbstractTableModel>
#include <QTableView>
#include "decoder.h"
#include "decoder_table_item.hpp"
#include "logic_analyzer.h"

namespace adiscope {


namespace logic {

class DecoderTableModel;

class DecoderTable : public QTableView {
    Q_OBJECT

public:
    DecoderTable(QWidget *parent = nullptr);

    void setLogicAnalyzer(LogicAnalyzer* logicAnalyzer);
    void activate(bool logic);
    void deactivate();

    // Shortcut to get a reference to the model. It be a nullptr.
    DecoderTableModel* decoderModel() const;

private:
    bool m_active;
};
} // namespace logic
} // namespace adiscope

#endif // DECODER_TABLE_H
