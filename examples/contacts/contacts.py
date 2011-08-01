#
# This file is part of LibQtTracker project
#
# Copyright (C) 2009, Nokia
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.
#
#  Created on: Jan 9, 2009
#      Author: Iridian Kiiskinen <ext-iridian.kiiskinen at nokia.com>
#

from PySide.QtCore import *
from PySide.QtGui import *
from QtTracker import *
import sys

class ContactViewer(QSplitter):

    def __init__(self):
        super(ContactViewer, self).__init__()

        self.query_area = QWidget()
        self.left_manage = QWidget()
        self.right_public = QWidget()

        self.addWidget(self.query_area)
        self.addWidget(self.left_manage)
        self.addWidget(self.right_public)

        self.query_layout = QVBoxLayout()
        self.query_area.setLayout(self.query_layout)
        self.query_view = QListView()
        self.query_layout.addWidget(self.query_view)
        self.query_view.setSelectionMode(QAbstractItemView.SingleSelection)
        self.query_view.doubleClicked[QModelIndex].connect(self.setContactId)
        self.refresh_query_button = QPushButton("Refresh contacts")
        self.query_layout.addWidget(self.refresh_query_button)
        self.refresh_query_button.clicked.connect(self.refreshContacts)

        self.manage_layout = QVBoxLayout()
        self.left_manage.setLayout(self.manage_layout)

        self.contact_id_layout = QHBoxLayout()
        self.manage_layout.addLayout(self.contact_id_layout)
        self.contact_id_label = QLabel("Contact iri: ")
        self.contact_id_layout.addWidget(self.contact_id_label)
        self.contact_id = QLineEdit()
        self.contact_id_layout.addWidget(self.contact_id)

        self.buttons = QHBoxLayout()
        self.manage_layout.addLayout(self.buttons)

        self.load_button = QPushButton("Load contact")
        self.buttons.addWidget(self.load_button)
        self.load_button.clicked.connect(self.loadContact)

        self.save_button = QPushButton("Save contact")
        self.buttons.addWidget(self.save_button)
        self.save_button.clicked.connect(self.saveContact)
        '''
        manage_layout->addLayout(headername_layout = new QHBoxLayout);
        headername_layout->addWidget(headername_label = new QLabel("Header iri: "));
        headername_layout->addWidget(headername = new QLineEdit);

        manage_layout->addWidget(headers = new QPlainTextEdit);
        '''
        self.public_layout = QVBoxLayout()
        self.right_public.setLayout(self.public_layout)

        self.fullname_layout = QHBoxLayout()
        self.public_layout.addLayout(self.fullname_layout)
        self.fullname_label = QLabel("Full name: ")
        self.fullname_layout.addWidget(self.fullname_label)
        self.fullname = QLineEdit()
        self.fullname_layout.addWidget(self.fullname)

        self.birthdate_layout = QHBoxLayout()
        self.public_layout.addLayout(self.birthdate_layout)
        self.birthdate_label = QLabel("Birth date: ")
        self.birthdate_layout.addWidget(self.birthdate_label)
        self.birthdate = QLineEdit()
        self.birthdate_layout.addWidget(self.birthdate)

        '''public_layout->addWidget(body = new QPlainTextEdit);'''

        self.refreshContacts()

    def refreshContacts(self):
        #select = RDFSelect()
        #select.newColumn("Contact_iri") = RDFVariable.fromType(Ontologies.nco.Contact)
        select = RDFVariable()#.fromType(nco.Contact)
        query_model = tracker().modelQuery(select)
        self.query_view.setModel(query_model.model())

    def setContactId(self):
        pass
#        self.contact_id.setText(item.data().toString())

    def loadContact(self):
        pass
#        Live<nco::Contact> contact = ::tracker()->liveNode(QUrl(contact_id->text()));
#        fullname->setText(contact->getFullname());
#        birthdate->setText(contact->getBirthDate().toString(Qt::ISODate));

    def saveContact(self):
        pass
#        Live<nco::Contact> contact = ::tracker()->liveNode(QUrl(contact_id->text()));
#        contact->setFullname(fullname->text());
#        contact->setBirthDate(QDateTime::fromString(birthdate->text(), Qt::ISODate));
#        refreshContacts();

def main():
    app = QApplication(sys.argv);

    view = ContactViewer()
    view.show()

    return app.exec_()

if __name__ == "__main__":
    main()
