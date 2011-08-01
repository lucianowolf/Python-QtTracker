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
 * main.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
 */

#include "contacts.h"

ContactViewer::ContactViewer()
{
	addWidget(query_area = new QWidget);
	addWidget(left_manage = new QWidget);
	addWidget(right_public = new QWidget);

	query_area->setLayout(query_layout = new QVBoxLayout);
	query_layout->addWidget(query_view = new QListView);
	query_view->setSelectionMode(QAbstractItemView::SingleSelection);
	QObject::connect(query_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setContactId(const QModelIndex &)));
	query_layout->addWidget(refresh_query_button = new QPushButton("Refresh contacts"));
	QObject::connect(refresh_query_button, SIGNAL(clicked()), this, SLOT(refreshContacts()));

	left_manage->setLayout(manage_layout = new QVBoxLayout);

	manage_layout->addLayout(contact_id_layout = new QHBoxLayout);
	contact_id_layout->addWidget(contact_id_label = new QLabel("Contact iri: "));
	contact_id_layout->addWidget(contact_id = new QLineEdit);

	manage_layout->addLayout(buttons = new QHBoxLayout);

	buttons->addWidget(load_button = new QPushButton("Load contact"));
	QObject::connect(load_button, SIGNAL(clicked(bool)), this, SLOT(loadContact()));

	buttons->addWidget(save_button = new QPushButton("Save contact"));
	QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(saveContact()));
/*
	manage_layout->addLayout(headername_layout = new QHBoxLayout);
	headername_layout->addWidget(headername_label = new QLabel("Header iri: "));
	headername_layout->addWidget(headername = new QLineEdit);

	manage_layout->addWidget(headers = new QPlainTextEdit);
*/
	right_public->setLayout(public_layout = new QVBoxLayout);

	public_layout->addLayout(fullname_layout = new QHBoxLayout);
	fullname_layout->addWidget(fullname_label = new QLabel("Full name: "));
	fullname_layout->addWidget(fullname = new QLineEdit);

	public_layout->addLayout(birthdate_layout = new QHBoxLayout);
	birthdate_layout->addWidget(birthdate_label = new QLabel("Birth date: "));
	birthdate_layout->addWidget(birthdate = new QLineEdit);

//	public_layout->addWidget(body = new QPlainTextEdit);

	refreshContacts();
}

void ContactViewer::refreshContacts()
{
	RDFSelect select;
	select.newColumn("Contact_iri") = RDFVariable::fromType<nco::Contact>();
	query_model = ::tracker()->modelQuery(select);
	query_view->setModel(query_model.model());
}
void ContactViewer::setContactId(const QModelIndex &item)
{
	contact_id->setText(item.data().toString());
}
void ContactViewer::loadContact()
{
	Live<nco::Contact> contact = ::tracker()->liveNode(QUrl(contact_id->text()));

	fullname->setText(contact->getFullname());
	birthdate->setText(contact->getBirthDate().toString(Qt::ISODate));
}

void ContactViewer::saveContact()
{
	Live<nco::Contact> contact = ::tracker()->liveNode(QUrl(contact_id->text()));

	contact->setFullname(fullname->text());

	contact->setBirthDate(QDateTime::fromString(birthdate->text(), Qt::ISODate));
	refreshContacts();
}

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    ContactViewer view;
    view.show();

	return app.exec();
}
