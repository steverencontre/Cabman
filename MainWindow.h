#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QListView>
#include <QTreeView>
#include <QTextBrowser>
#include <QWidget>
#include <QSplitter>

class MainWindow : public QMainWindow
  {
	Q_OBJECT

  public:
	explicit MainWindow();
	~MainWindow();

  private:

	QSplitter			  *m_LRSplitter;
	QSplitter			  *m_TopicOutboxSplitter;
	QSplitter			  *m_MessagePaneSplitter;
	QTreeView			  *m_TopicsView;
	QListView			  *m_OutboxView;
	QTreeView			  *m_MessagesView;
	QTextBrowser	  *m_MessageView;

  };

#endif // MAINWINDOW_H
