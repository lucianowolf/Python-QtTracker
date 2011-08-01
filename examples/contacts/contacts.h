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
 * contact.h
 *
 *  Created on: Jan 13, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#ifndef CONTACTS_H_
#define CONTACTS_H_

#include <QtGui>
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

class ContactViewer
	: public QSplitter
{
	Q_OBJECT

public:
	QWidget *query_area, *left_manage, *right_public;

	QVBoxLayout *query_layout;
	QListView *query_view;
	QPushButton *refresh_query_button;
	LiveNodes query_model;


	QVBoxLayout *manage_layout;

	QHBoxLayout *contact_id_layout;
	QLabel *contact_id_label;
	QLineEdit *contact_id;

	QHBoxLayout *buttons;
	QPushButton *load_button, *save_button;
	QHBoxLayout *headername_layout;
	QLabel *headername_label;
	QLineEdit *headername;
	QPlainTextEdit *headers;


	QVBoxLayout *public_layout;

	QHBoxLayout *fullname_layout;
	QLabel *fullname_label;
	QLineEdit *fullname;

	QHBoxLayout *birthdate_layout;
	QLabel *birthdate_label;
	QLineEdit *birthdate;

	QPlainTextEdit *body;


	ContactViewer();

public Q_SLOTS:
	void refreshContacts();
	void setContactId(const QModelIndex &item);
	void loadContact();
	void saveContact();
};

#endif /* CONTACTS_H_ */
