/*
 * This file is part of LibQtTracker project
 *
 * Copyright (C) 2009, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * tracker_queries.h
 *
 *  Created on: Jan 7, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef TRACKER_QUERIES_H_
#define TRACKER_QUERIES_H_

#include <QtGui>
#include <QtTracker/QTracker>

using namespace SopranoLive;

typedef void(*QueryBuilderPtr)(RDFSelect &);
Q_DECLARE_METATYPE(QueryBuilderPtr)
typedef QTableView TrackerTable;
/*

class TrackerTable
	: public QTableView
{
public:
	TrackerTable();
};
*/
class TrackerQueries
	: public QSplitter
{
	Q_OBJECT

	LiveNodes trackermodel;
	TrackerTable *trackertable;
	QSplitter *top_split;
	QWidget *query_select_widget, *query_view_widget;
	QVBoxLayout *query_select_box, *query_view_box;

	QListView *query_select;
	RDFSelect *select;
	QStandardItemModel *queries;
	QHBoxLayout *select_viewer_buttons;
	QPushButton *load_button, *save_button;
	QPlainTextEdit *query_viewer;
	QLineEdit *fields_viewer;
	QPushButton *execute_button;

public:
	TrackerQueries();
	~TrackerQueries();

	void setSelect(RDFSelect *select);

public Q_SLOTS:
	void loadQuery();
	void saveQuery();
	void executeQuery();
	void queryEdited();
};

#endif /* TRACKER_QUERIES_H_ */
