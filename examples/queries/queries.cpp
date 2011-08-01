#include <QtGui/QTableView>
#include <QtGui/QApplication>
#include <QtDebug>

#include "queries.h"

// Think in relational database terms
// RDFVariable is not an object representing one node
//   but instead a constraint
// that later on is realized into actual nodes
//                                 one or many, in sequence
// within the result set


struct TrackerQueryBuilders
{
	/*
	 *	SELECT ?artistname COUNT(?album) AS count SUM(?length) AS len WHERE {
	 *		?song nid3:leadArtist ?artist ;
	 *			  nid3:length ?length ;
	 *			  nid3:albumTitle ?album .
	 *		?artist nco:fullname ?artistname
	 *	} GROUP BY ?artist
	 */
	static void prepareQueryMusicArtistList(RDFSelect &query)
	{
		RDFVariable song;

		// I am an unrealized artist
		//   limited by an unrealized song
		RDFVariable artist = song.property<nid3::leadArtist>();

		query.groupBy(artist);

		query.newColumn("artistname") = artist.property<nco::fullname>();
		query.newCountColumn("count") = song.property<nid3::albumTitle>();
		query.newSumColumn("length") = song.property<nid3::length>();

		// Realization! Fulfillment! Satisfaction!
		//   but art is never done, so we live on
		//                     to see another song
	}


	/*
	 *	Used by:
	 * 		prepareQueryRecentEmails
	 *		prepareQueryRecentEmailDetailed
	 */
	static RDFVariable queryEmailMessage(RDFSelect &query, RDFVariable msg = RDFVariable())
	{
		query.newColumn("subject") = msg.property<nmo::messageSubject>();
		query.newColumn("date") = msg.property<nmo::receivedDate>();
		query.orderBy(msg.property<nmo::receivedDate>(), RDFSelect::Descending);
		query.limit(10);
		return msg;
	}

	/**
	 *	SELECT ?subject ?date WHERE {
	 *		?msg nmo:messageSubject ?subject ;
	 *			 nmo:receivedDate ?date
	 *	} ORDER BY DESC(?date) LIMIT 10
	 */
	static void prepareQueryRecentEmails(RDFSelect &query)
	{
		queryEmailMessage(query);
	}

	/**
	 *	SELECT ?subject ?date ?firstname ?lastname WHERE {
	 *	    ?msg nmo:messageSubject ?subject ;
	 *	         nmo:receivedDate ?date ;
	 *	         nmo:from .property<
	 *	             nco:nameGiven ?firstname ;
	 *	             nco:nameFamily ?lastname
	 *	         ]
	 *	} ORDER BY DESC(?date) LIMIT 10
	 */
	static void prepareQueryRecentEmailedDetailed(RDFSelect &query)
	{
		RDFVariable sender = queryEmailMessage(query).property<nmo::from>();
		query.newColumn("firstname") = sender.property<nco::nameGiven>();
		query.newColumn("lastname") = sender.property<nco::nameFamily>();
		query.limit(10);
	}

	/**
	 *	SELECT COUNT(?msg) AS count WHERE {
	 *		?msg a nmo:Email
	 *	}
	 */
	static void prepareQueryGetEmailCount(RDFSelect &query)
	{
		query.newCountColumn("count") = RDFVariable::fromType<nmo::Email>();
	}

	/**
	 *	SELECT COUNT(?contact) AS count WHERE {
	 *		?contact nco:hasPhoneNumber ?phonenumber
	 * 	}
	 */
	static void prepareQueryGetEmailContactCount(RDFSelect &query)
	{
		RDFVariable contact;
		contact.property<nco::hasPhoneNumber>();
		query.newCountColumn("count") = contact;
	}

	/**
	 *	SELECT ?contact WHERE {
	 *		?contact nco:hasPhoneNumber ?phonenumber
	 * 	} GROUP BY ?contact
	 */
	static void prepareQueryGetEmailContacts(RDFSelect &query)
	{
		RDFVariable contact;
		contact.property<nco::hasPhoneNumber>();
		query.newCountColumn("contact") = contact;
		query.groupBy(contact);
	}


	/**
	 *	SELECT ?fullname ?title WHERE {
	 *		?contact nco:fullname ?fullname ;
	 *		         nco:hasAffiliation [
	 *		         	nco:title ?title
	 *		         ] ;
	 *		         nco:birthDate ?birthdate .
	 *		         ?fullname foo:fts "prefix"
	 *		         FILTER (regex(?fullname, "matti"))
	 *	} ORDER BY ?birthdate
	 */
	static void prepareQueryGetNamesWithTitles(RDFSelect &query)
	{
		RDFVariable contact;
		query.orderBy(contact.property<nco::birthDate>());
		query.newColumn("fullname") = contact.property<nco::fullname>();
		query.newColumn("title") = contact.property<nco::hasAffiliation>().property<nco::title>();
	}


	/**
	 *	SELECT ?fullname ?title WHERE {
	 *		?contact nco:fullname ?fullname ;
	 *		         nco:birthDate ?birthdate
	 *		OPTIONAL {
	 *			?contact nco:hasAffiliation [ nco:title ?title ]
	 *		}
	 *	} ORDER BY ?birthdate
	 */
	static void prepareQueryGetNamesWithOptionalTitle(RDFSelect &query)
	{
		RDFVariable contact;
		query.orderBy(contact.property<nco::birthDate>());
		query.newColumn("fullname") = contact.property<nco::fullname>();
		query.newColumn("title") = contact.optional().property<nco::hasAffiliation>().property<nco::title>();
	}

	/**
	 *	SELECT ?fullname WHERE {
	 *		?contact nco:hasPostalAddress [ nco:region 'PA' ] ;
	 *				 nco:birthDate ?birthdate ;
	 *				 nco:fullname ?fullname
	 *	} ORDER BY ?birthdate
	 */
	static void prepareQueryGetNamesFromRegion(RDFSelect &query)
	{
		RDFVariable contact;

		contact.property<nco::hasPostalAddress>().property<nco::region>() = LiteralValue("PA");

		query.orderBy(contact.property<nco::birthDate>());
		query.newColumn("fullname") = contact.property<nco::fullname>();
	}

	/**
	 *	SELECT ?subject ?object WHERE {
	 *		?subject nmo:headerName ?object
	 *	}
	 */
	static void prepareQueryHeaderHavingEmails(RDFSelect &query)
	{
		RDFVariable subject;
		query.newColumn("subject") = subject;
		query.newColumn("object") = subject.property<nmo::headerName>();
	}

	static void prepareInsertEmailAndQuery(RDFSelect &query)
	{
		Live<nmo::Email> email = ::tracker()->createLiveNode();

		Live<nmo::MessageHeader> header = email->addMessageHeader();

		header->setHeaderValue("groaar");
		header->setHeaderName("foobarbaz");

		prepareQueryHeaderHavingEmails(query);
	}

	/**
	 *	SELECT ?name WHERE {
	 *		?contact a nco:Contact .
	 *		?contact nco:nameGiven ?name .
	 *		FILTER REGEX(?name, "\M\a\t")
	 *	}
	 */
	static void prepareContactsPrefixSearchQuery(RDFSelect &query)
	{
		RDFVariable name = RDFVariable::fromType<nco::Contact>().property<nco::fullname>();
		name.hasPrefix("Mat");
		query.addColumn("name", name);
	}

	static void prepareContactByPhoneNumber(RDFSelect &query)
	{
		RDFVariable contact = RDFVariable::fromType<nco::Contact>();

		  // not an assignment, but a statement/constraint,
		  // that contact->nco::hasPhoneNumber->nco::phoneNumber equals to literal 0401234567
		contact.property<nco::hasPhoneNumber>().property<nco::phoneNumber>() = LiteralValue("0401234567");
		query.addColumn("First name", contact.property<nco::nameGiven>());
		query.addColumn("Last name", contact.property<nco::nameFamily>());
	}

	static void prepareQueryTaggedObjects(RDFSelect &query)
	{
		QStringList tags;
		tags << "cool" << "interesting" << "favourite";

		RDFVariable content_uri;
		content_uri.isOfType(QUrl("http://dummy.org/"), false);
		content_uri.notEqual(QUrl("http://not.this.uri/"));

		RDFPattern current_union = content_uri.pattern().child();
		RDFVariable tag_obj;

		foreach(QString tag, tags)
		{
			RDFVariable tag_obj_in_union = current_union.variable(tag_obj);
			RDFVariable content_uri_in_union = current_union.variable(content_uri);

			tag_obj_in_union.property<nao::prefLabel>() = LiteralValue(tag);
			content_uri_in_union.property<nao::hasTag>() = tag_obj_in_union;
			current_union = current_union.union_();
		}

		query.addColumn("content_uri", content_uri);
		query.addSumColumn("tag_count", tag_obj);
		query.groupBy(content_uri);
	}

	static void prepareQueryInterfaceDelete(RDFSelect &query)
	{
		RDFVariable subject = query.newColumn("subject");
		RDFVariable predicate = query.newColumn("predicate");
		RDFVariable object = query.newColumn("object");

		RDFPattern parent = subject.pattern();

		subject[predicate] = object;

		LiveNode source = LiveNode(QUrl("http://url.com/123"));
		QUrl interface_name = QUrl("http://url.com/interfaceA");

		RDFPattern all_objects = subject.pattern().child();
		all_objects.variable(subject) = all_objects.variable(source);
		all_objects.variable(predicate).property<rdfs::domain>() = interface_name;

		RDFPattern all_subjects = all_objects.union_();
		all_subjects.variable(object) = all_subjects.variable(source);
		all_subjects.variable(predicate).property<rdfs::range>() = interface_name;

		RDFPattern all_types = all_subjects.union_();
		all_types.variable(subject) = all_types.variable(source);
		all_types.variable(subject).type() = interface_name;
	}

	static void prepareUpdateEmailSubjects(RDFSelect &/*query*/)
	{
		RDFUpdate up;
		up.addDeletion(RDFStatement(RDFVariable::fromType<nmo::Email>(), nmo::messageSubject::iri(), RDFVariable()));
		up.addInsertion(RDFStatement(RDFVariable::fromType<nmo::Email>(), nmo::messageSubject::iri(), LiteralValue("Subject here!")));
		qDebug() << up.getQuery();
	}

	static void prepareQueryTestVariableLink(RDFSelect &query)
	{
		RDFVariableLink pointer = query.newColumn("stuff");
		pointer.switchTarget(RDFVariable::fromType<nmo::Email>());
		qDebug() << query.getQuery();
		pointer.switchTarget(RDFVariable::fromType<nco::Contact>());
	}

	static void prepareQuerySelectPhotos(RDFSelect &query)
	{
	    RDFVariable photos = RDFVariable::fromType<nfo::Image>();
	    query.addColumn("images", photos);
	    photos.filter("str").contains("urn").not_();
	    query.limit(18);
	}

	static void loadQueries(QStandardItemModel &queries)
	{
		struct QueryBuilder
		{
			QueryBuilderPtr test;
			const char *name;
		};

		QueryBuilder builders[] =
		{	{ &TrackerQueryBuilders::prepareQueryMusicArtistList, "Build RDFQuery: Music artist list" }
		,	{ &TrackerQueryBuilders::prepareQueryRecentEmails, "Build RDFQuery: Recent emails" }
		,	{ &TrackerQueryBuilders::prepareQueryRecentEmailedDetailed, "Build RDFQuery: Recent emailed detailed" }
		,	{ &TrackerQueryBuilders::prepareQueryGetEmailCount, "Build RDFQuery: Get email count" }
		,	{ &TrackerQueryBuilders::prepareQueryGetEmailContactCount, "Build RDFQuery: Get email contact count" }
		,	{ &TrackerQueryBuilders::prepareQueryGetEmailContacts, "Build RDFQuery: Get email contacts" }
		,	{ &TrackerQueryBuilders::prepareQueryGetNamesWithTitles, "Build RDFQuery: Get names with titles" }
		,	{ &TrackerQueryBuilders::prepareQueryGetNamesWithOptionalTitle, "Build RDFQuery: Get names with optional title" }
		,	{ &TrackerQueryBuilders::prepareQueryGetNamesFromRegion, "Build RDFQuery: Get names from region" }
		,	{ &TrackerQueryBuilders::prepareInsertEmailAndQuery, "Build RDFQuery: Insert email and query all emails" }
		,	{ &TrackerQueryBuilders::prepareContactsPrefixSearchQuery, "Build RDFQuery: Contacts prefix search" }
		,	{ &TrackerQueryBuilders::prepareContactByPhoneNumber, "Build RDFQuery: Contacts by phone number search" }
		,	{ &TrackerQueryBuilders::prepareQueryTaggedObjects, "Build RDFQuery: Select objects tagged by any tag in a given set" }
		,	{ &TrackerQueryBuilders::prepareQueryInterfaceDelete, "Build RDFQuery: Build delete interface select" }
		,	{ &TrackerQueryBuilders::prepareUpdateEmailSubjects, "Build RDFQuery: Update email subjects" }
		,	{ &TrackerQueryBuilders::prepareQueryTestVariableLink, "Build RDFQuery: Test variable links" }
		,	{ &TrackerQueryBuilders::prepareQuerySelectPhotos, "Build RDFQuery: Select photos without 'urn' in their uri" }
		};

		for(QueryBuilder *bi = builders; bi != builders + sizeof(builders) / sizeof(QueryBuilder); ++bi)
		{
			QStandardItem &item = *new QStandardItem(bi->name);
			item.setData(QVariant::fromValue(bi->test));
			qDebug() << "Gained a follower: \"" << bi->name << "\"";
			queries.appendRow(&item);
		}
	}
};

/*
TrackerTable::TrackerTable()
{}
*/
TrackerQueries::TrackerQueries()
	: QSplitter(Qt::Vertical)
{

	addWidget(top_split = new QSplitter(Qt::Horizontal));
	addWidget(trackertable = new TrackerTable());
	setStretchFactor(0, 0);
	setStretchFactor(1, 1);

	top_split->addWidget(query_select_widget = new QWidget());
	top_split->addWidget(query_view_widget = new QWidget());

	top_split->setStretchFactor(0, 0);
	top_split->setStretchFactor(1, 1);

	query_select_widget->setLayout(query_select_box = new QVBoxLayout());
	query_view_widget->setLayout(query_view_box = new QVBoxLayout());

	query_select_box->addWidget(query_select = new QListView());

	select = 0;

	queries = new QStandardItemModel(this);
	queries->setHorizontalHeaderItem(0, new QStandardItem("RDFQuery"));
	TrackerQueryBuilders::loadQueries(*queries);
	query_select->setSelectionMode(QAbstractItemView::SingleSelection);
	query_select->setModel(queries);
	qDebug() << "Gained" << queries->rowCount() << "followers";

	query_select_box->addLayout(select_viewer_buttons = new QHBoxLayout());

	select_viewer_buttons->addWidget(load_button = new QPushButton("&Load query"));
	load_button->connect(load_button, SIGNAL(clicked(bool)), this, SLOT(loadQuery()));

	select_viewer_buttons->addWidget(save_button = new QPushButton("&Save query"));
	save_button->connect(save_button, SIGNAL(clicked()), this, SLOT(saveQuery()));

	query_view_box->addWidget(query_viewer = new QPlainTextEdit());
	query_viewer->connect(query_viewer, SIGNAL(textChanged()), this, SLOT(queryEdited()));

	query_view_box->addWidget(fields_viewer = new QLineEdit());

	query_view_box->addWidget(execute_button = new QPushButton("&Model query"));
	execute_button->connect(execute_button, SIGNAL(clicked()), this, SLOT(executeQuery()));

	updateGeometry();
}

TrackerQueries::~TrackerQueries()
{
	setSelect(0);
}

void TrackerQueries::setSelect(RDFSelect *select)
{
	if(this->select)
		delete this->select;
	this->select = select;
}

void TrackerQueries::loadQuery()
{
	// This is not a trivial exemple of how to use select.
	// See examples/email/email.cpp:refreshEmails for a short one.

	qDebug() << "Dragons ahead...";
	QString text, columns;
	QList<QModelIndex> selection = query_select->selectionModel()->selectedIndexes();
	if(!selection.size())
	{
		qDebug() << "I alone am mighty!";
		return;
	}
	QVariant data = selection.front().data(Qt::UserRole + 1);
	qDebug() << "I choose " << selection.front().data().toString() << "!";

	if(QueryBuilderPtr ptr = data.value<QueryBuilderPtr>())
	{
		setSelect(new RDFSelect());
		(*ptr)(*select);
		text = select->getQuery();
		columns = QString();
		for(RDFSelectColumnList::iterator fi = select->columns().begin(); fi != select->columns().end(); ++fi)
			columns += fi->name() + ";";
		columns.resize(columns.size() - 1);
	} else
	{
		setSelect(0);
		text = data.toString();
		columns = selection.front().data(Qt::UserRole + 2).toString();
	}

	qDebug() << "He brandishes his weapons: " << columns.split(";").join(", ");

	query_viewer->setPlainText(text);
	fields_viewer->setText(columns);
}

void TrackerQueries::queryEdited()
{
	setSelect(0);
	qDebug() << "Query edited";
}

void TrackerQueries::saveQuery()
{}


void TrackerQueries::executeQuery()
{
	if(select)
		trackermodel = ::tracker()->modelQuery(*select);
	else
		trackermodel = ::tracker()->modelQuery( query_viewer->toPlainText()
		                                         , fields_viewer->text().split(";"));
	trackertable->setModel(trackermodel.model());
}

int main(int argc, char *argv[])
{
	qsrand(QDateTime::currentDateTime().toTime_t());

	::tracker()->setVerbosity(3);

	qDebug() << "We live!";

    QApplication app(argc,argv);

    qDebug() << "Go to town.";

	TrackerQueries frame;

	qDebug() << "Past the gates";

	frame.show();

	qDebug() << "And to the wilderness!";

	return app.exec();
}
