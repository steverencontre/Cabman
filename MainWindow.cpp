#include <QtWidgets>

#include "MainWindow.h"
#include "CixNetworkIo.h"


MainWindow::MainWindow()
  :
	m_LRSplitter					{new QSplitter (Qt::Orientation::Horizontal, this)},
	m_TopicOutboxSplitter	{new QSplitter (Qt::Orientation::Vertical, m_LRSplitter)},
	m_MessagePaneSplitter	{new QSplitter (Qt::Orientation::Vertical, m_LRSplitter)},
	m_TopicsView				{new QTreeView (m_TopicOutboxSplitter)},
	m_OutboxView				{new QListView (m_TopicOutboxSplitter)},
	m_MessagesView			{new QTreeView (m_MessagePaneSplitter)},
	m_MessageView				{new QTextBrowser (m_MessagePaneSplitter)},
	m_CixModel					{new Cix::Model (this)}
  {
	setWindowTitle ("Cabman");
	setWindowIcon (QIcon::fromTheme ("tools-check-spelling"));	// current theme gives us an "A" for Ameol-ish :-)
	setCentralWidget (m_LRSplitter);

	m_TopicsView->setModel (m_CixModel->GetConfListModel());

	// get saved sizes and positions

	connect (m_CixModel, SIGNAL (ConfTopicsUpdated()), this, SLOT (onConfTopicsUpdated()), Qt::QueuedConnection);

	m_CixModel->start();
  }


void MainWindow::onConfTopicsUpdated()
{
	m_TopicsView->dataChanged ({}, {}, {});
}




MainWindow::~MainWindow()
  {

  }
