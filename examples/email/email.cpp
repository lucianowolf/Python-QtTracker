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

#include "email.h"

EmailViewer::EmailViewer()
{
	addWidget(query_area = new QWidget);
	addWidget(left_manage = new QWidget);
	addWidget(right_public = new QWidget);

	query_area->setLayout(query_layout = new QVBoxLayout);
	query_layout->addWidget(query_view = new QListView);
	query_view->setSelectionMode(QAbstractItemView::SingleSelection);
	QObject::connect(query_view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setEmailId(const QModelIndex &)));
	query_layout->addWidget(refresh_query_button = new QPushButton("Refresh emails"));
	QObject::connect(refresh_query_button, SIGNAL(clicked()), this, SLOT(refreshEmails()));
	query_layout->addWidget(live_query_button = new QPushButton("Live emails"));
	QObject::connect(live_query_button, SIGNAL(clicked()), this, SLOT(liveEmails()));

	left_manage->setLayout(manage_layout = new QVBoxLayout);

	manage_layout->addLayout(email_id_layout = new QHBoxLayout);
	email_id_layout->addWidget(email_id_label = new QLabel("Email iri: "));
	email_id_layout->addWidget(email_id = new QLineEdit);

	manage_layout->addLayout(buttons = new QHBoxLayout);

	buttons->addWidget(load_button = new QPushButton("Load email"));
	QObject::connect(load_button, SIGNAL(clicked(bool)), this, SLOT(loadEmail()));

	buttons->addWidget(save_button = new QPushButton("Save email"));
	QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(saveEmail()));

	buttons->addWidget(save_button = new QPushButton("Live email"));
	QObject::connect(save_button, SIGNAL(clicked()), this, SLOT(liveEmail()));

	manage_layout->addLayout(headername_layout = new QHBoxLayout);
	headername_layout->addWidget(headername_label = new QLabel("Header iri: "));
	headername_layout->addWidget(headername = new QLineEdit);

	manage_layout->addWidget(headers = new QPlainTextEdit);

	right_public->setLayout(public_layout = new QVBoxLayout);


	public_layout->addLayout(from_layout = new QHBoxLayout);
	from_layout->addWidget(from_label = new QLabel("From: "));
	from_layout->addWidget(from = new QLineEdit);

	public_layout->addLayout(recipients_layout = new QHBoxLayout);
	recipients_layout->addWidget(recipients_label = new QLabel("Recipients: "));
	recipients_layout->addWidget(recipients = new QLineEdit);

	public_layout->addLayout(subject_layout = new QHBoxLayout);
	subject_layout->addWidget(subject_label = new QLabel("Subject: "));
	subject_layout->addWidget(subject = new QLineEdit);

	public_layout->addWidget(body = new QPlainTextEdit);

	refreshEmails();
}

void EmailViewer::refreshEmails(bool is_live)
{
	RDFVariable emails = RDFVariable::fromType<nmo::Email>();

	// add the properties and their subproperties to be cached for each email
	RDFVariable header_cached = emails.addDerivedObject<nmo::messageHeader>();
	header_cached.addDerivedObject<nmo::headerName>();
	header_cached.addDerivedObject<nmo::headerValue>();

	RDFVariable sender_cached = emails.addDerivedObject<nmo::from>();
	sender_cached.addDerivedObject<nco::fullname>();

	RDFVariable recipients_cached = emails.addDerivedObject<nmo::recipient>();
	recipients_cached.addDerivedObject<nco::fullname>();

	emails.addDerivedObject<nmo::messageSubject>();
	emails.addDerivedObject<nmo::plainTextContent>();

	RDFSelect email_selection;
	email_selection.addColumn("Email IRI", emails);
	this->query_model = ::tracker()->modelQuery(email_selection, is_live ? RDFStrategy::LiveStrategy : RDFStrategy::DefaultStrategy);
	this->query_model->setAsModelInto(this->query_view); // set to QListView
}

void EmailViewer::refreshEmails()
{
	refreshEmails(false);
}

void EmailViewer::liveEmails()
{
	refreshEmails(true);
}

void EmailViewer::setEmailId(const QModelIndex &item)
{
	email_id->setText(item.data().toString());
}
void EmailViewer::loadEmail()
{
	QUrl email_id = QUrl(this->email_id->text());

	Live<nmo::Email> email = ::tracker()->liveNode(email_id);

	Live<nmo::MessageHeader> header = email->firstMessageHeader();
	if(!header)
		header = email->addMessageHeader();

	this->headername->setText(header->getHeaderName());
	this->headers->setPlainText(header->getHeaderValue());

	Live<nco::Contact> sender = email->getFrom();
	this->from->setText(sender->getFullname());

	QStringList recipient_string;
	LiveNodes recipients = email->getRecipients();
	int row_count = recipients->rowCount();
	qDebug() << "Recipients(" << row_count << "):";
	for(int i = 0; i < row_count; ++i)
		qDebug() << "Recipient" << i << ":" << recipients->liveNode(i).toString();
	foreach(Live<nco::Contact> recipient, recipients)
		recipient_string << recipient->getFullname();
	this->recipients->setText(recipient_string.join("; "));

	this->subject->setText(email->getMessageSubject());
	this->body->setPlainText(email->getPlainTextContent());
}

Live<nco::Contact> EmailViewer::getContactFromName(QString name) const
{
	Live<nco::Contact> contact
				= ::tracker()->firstResource<nco::Contact>(
						RDFVariable(LiteralValue(name)).subject<nco::fullname>());
	if(!contact)
	{
		contact = ::tracker()->createLiveNode();
		contact->setFullname(name);
	}
	return contact;
}

void EmailViewer::saveEmail()
{
	QUrl email_id = QUrl(this->email_id->text());

	// try to obtain a transaction for aggregating all insertions.
	// If transaction is not committed or explicitly rolled back
	// before it gets destroyed, an implicit rollback is perfomed
	RDFTransactionPtr transaction = ::tracker()->initiateTransaction();

	RDFServicePtr service;
	if(transaction)
		// if transaction was obtained, grab the service from inside it and use it
		service = transaction->service();
	else
		// otherwise, use tracker directly, with no transactions.
		service = ::tracker();

	// by default, initiateTransaction will apply the transaction to the existing
	// service in-place, so in fact transaction->service() will be equal to ::tracker().
	// This would mean that all live objects obtained from ::tracker() before and during
	// the transaction will be wrapped inside the transaction as well.
	// However, it is recommended, best practice to access all the live nodes used
	// inside the transaction through transaction->service().

	Live<nmo::Email> email = service->liveNode(email_id);

	Live<nmo::MessageHeader> header = email->firstMessageHeader();
	if(!header)
		header = email->addMessageHeader();

	header->setHeaderName(this->headername->text());
	header->setHeaderValue(this->headers->toPlainText());

	Live<nco::Contact> sender = email->getFrom();
	sender->setFullname(this->from->text());

	QList<LiveNode> recipient_list;
	foreach(QString recipient_name, this->recipients->text().split(";"))
		recipient_list << this->getContactFromName(recipient_name);
	email->setRecipients(recipient_list);

	email->setMessageSubject(this->subject->text());
	email->setPlainTextContent(this->body->toPlainText());

	// remember to commit the transaction, otherwise all changes will be rolled back.
	if(transaction)
		transaction->commit();

	refreshEmails();
}

void EmailViewer::liveEmail()
{
	QUrl email_id = QUrl(this->email_id->text());
	Live<nmo::Email> email = ::tracker()->liveNode(email_id);

	LiveNodeList headernodes = email->getMessageHeaders();
	Live<nmo::MessageHeader> header;
	if(headernodes.size())
		header = headernodes.front();
	else
		header = email->addMessageHeader();

	header->setHeaderName(this->headername->text());
	header->setHeaderValue(this->headers->toPlainText());

	Live<nco::Contact> sender = email->getFrom();
	sender->setFullname(this->from->text());

	QList<LiveNode> recipient_list;
	foreach(QString recipient_name, this->recipients->text().split(";"))
		recipient_list << this->getContactFromName(recipient_name);
	email->setRecipients(recipient_list);

	email->setMessageSubject(this->subject->text());
	email->setPlainTextContent(this->body->toPlainText());
}


int main(int argc, char *argv[])
{
	::tracker()->setVerbosity(4);
    QApplication app(argc,argv);

    LiveNodes results;
    results->refreshModel(LiveNodeModel::Flush);

    EmailViewer view;

    view.show();

    return app.exec();
}
