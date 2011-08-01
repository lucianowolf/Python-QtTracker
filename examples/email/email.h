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
 * email.h
 *
 *  Created on: Jan 13, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef EMAIL_H_
#define EMAIL_H_

#include <QtGui>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nmo.h>

using namespace SopranoLive;
using namespace SopranoLive::Ontologies;
using namespace SopranoLive::BackEnds;

class EmailViewer
	: public QSplitter
{
	Q_OBJECT

public:
	QWidget *query_area, *left_manage, *right_public;

	QVBoxLayout *query_layout;
	QListView *query_view;
	QPushButton *refresh_query_button, *live_query_button;
	LiveNodes query_model;


	QVBoxLayout *manage_layout;

	QHBoxLayout *email_id_layout;
	QLabel *email_id_label;
	QLineEdit *email_id;

	QHBoxLayout *buttons;
	QPushButton *load_button, *save_button, *live_button;
	QHBoxLayout *headername_layout;
	QLabel *headername_label;
	QLineEdit *headername;
	QPlainTextEdit *headers;


	QVBoxLayout *public_layout;

	QHBoxLayout *from_layout;
	QLabel *from_label;
	QLineEdit *from;

	QHBoxLayout *recipients_layout;
	QLabel *recipients_label;
	QLineEdit *recipients;

	QHBoxLayout *subject_layout;
	QLabel *subject_label;
	QLineEdit *subject;

	QPlainTextEdit *body;


	EmailViewer();

public Q_SLOTS:
	void refreshEmails(bool isLive);
	void refreshEmails();
	void liveEmails();
	void setEmailId(const QModelIndex &item);
	Live<nco::Contact> getContactFromName(QString name) const;
	void loadEmail();
	void saveEmail();
	void liveEmail();
};

#endif /* EMAIL_H_ */
