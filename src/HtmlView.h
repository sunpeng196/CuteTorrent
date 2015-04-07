#pragma once
#include <QTextBrowser>
#include <QHash>

class QNetworkAccessManager;
class QNetworkDiskCache;
class QNetworkReply;

class HtmlView : public QTextBrowser
{
	Q_OBJECT

public:
	explicit HtmlView(QWidget* parent = 0);
	~HtmlView();

	virtual QVariant loadResource(int type, const QUrl &name);

protected:
	QNetworkAccessManager *m_netManager;
	QNetworkDiskCache *m_diskCache;
	QHash<QUrl, bool> m_activeRequests;

	protected slots:
	void resourceLoaded(QNetworkReply *reply);
};