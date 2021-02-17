#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QListView>
#include <QTreeView>
#include <QTextBrowser>
#include <QWidget>
#include <QSplitter>

#include "Settings.h"
#include "CixModel.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow();
	~MainWindow() override;

public slots:
	void	onConfTopicsUpdated();

private:

	QSplitter			  *m_LRSplitter;
	QSplitter			  *m_TopicOutboxSplitter;
	QSplitter			  *m_MessagePaneSplitter;
	QTreeView			  *m_TopicsView;
	QListView			  *m_OutboxView;
	QTreeView			  *m_MessagesView;
	QTextBrowser	  *m_MessageView;

	Settings					m_Settings;
	Cix::Model			  *m_CixModel;

};

#endif // MAINWINDOW_H
