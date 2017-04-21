#include <QtWidgets>

#include "MainWindow.h"


MainWindow::MainWindow()
  :
	m_LRSplitter					{new QSplitter (Qt::Orientation::Horizontal, this)},
	m_TopicOutboxSplitter	{new QSplitter (Qt::Orientation::Vertical, m_LRSplitter)},
	m_MessagePaneSplitter	{new QSplitter (Qt::Orientation::Vertical, m_LRSplitter)},
	m_TopicsView				{new QTreeView (m_TopicOutboxSplitter)},
	m_OutboxView				{new QListView (m_TopicOutboxSplitter)},
	m_MessagesView			{new QTreeView (m_MessagePaneSplitter)},
	m_MessageView				{new QTextBrowser (m_MessagePaneSplitter)}
  {
	setWindowTitle ("Cabman");
	setWindowIcon (QIcon::fromTheme ("tools-check-spelling"));	// current theme gives us an "A" for Ameol-ish :-)
	setCentralWidget (m_LRSplitter);

	// get saved sizes and positions

  }

MainWindow::~MainWindow()
  {

  }
