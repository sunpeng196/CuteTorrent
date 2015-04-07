#ifndef _RSS_MANAGER_INCLUDED_
#define _RSS_MANAGER_INCLUDED_

#include <QHash>
#include <QUrl>
#include <QUuid>
#include <QRunnable>
#include <QQueue>
#include <boost/weak_ptr.hpp>
#include "RssCommon.h"
class QApplicationSettings;

struct FeedTask 
{
	QUrl url;
	QUuid uid;
};

class RssManager : public QObject
{
	Q_OBJECT
public:
	
	QList<RssFeed*> feeds();
	RssFeed* addFeed(QUrl url, bool& isNew);
	~RssManager();
	void SaveFeeds();
	static RssManagerPtr getInstance();
private:
	static boost::weak_ptr<RssManager> m_sInstrance;
	RssManager(QObject* parent = NULL);
	
	QList<FeedTask> m_activeRequests;
	QList<RssFeed*> m_pFeeds;
	QApplicationSettings* m_pSettings;
	
	void LoadFeeds();
signals:
	void FeedChanged(QUuid);
	void FeedRemoved(QUuid);
};

#endif
