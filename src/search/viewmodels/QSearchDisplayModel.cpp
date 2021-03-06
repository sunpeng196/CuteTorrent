﻿#include <stddef.h>
#include "QSearchDisplayModel.h"
#include "StaticHelpers.h"

QSearchDisplayModel::QSearchDisplayModel(QTreeView* pTorrentListView, QObject* parent) : QAbstractListModel(parent), m_pSearchEngine(SearchEngine::getInstance())
{
	m_pTorrentListView = pTorrentListView;
	QObject::connect(m_pSearchEngine.get(), SIGNAL(GotResults()), this, SLOT(OnNewSearchResults()));
	SearchItemsStorrage* pItems = m_pSearchEngine->GetResults();
	QObject::connect(pItems, SIGNAL(reset()), this, SLOT(OnNewSearchResults()));
}


QSearchDisplayModel::~QSearchDisplayModel()
{
}

int QSearchDisplayModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	return m_pSearchEngine->GetResults()->length();
}

int QSearchDisplayModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	return SEARCH_RESULTS_COLUNS_COUNT;
}

QVariant QSearchDisplayModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
	if(index.isValid())
	{
		int row = index.row();
		int col = index.column();
		SearchItemsStorrage* results = m_pSearchEngine->GetResults();

		if(row > results->length())
		{
			return QVariant();
		}

		SearchResult* res = (*results) [row];

		switch(role)
		{
			case Qt::DisplayRole:
			{
				switch(col)
				{
					case 0:
					{
						return qVariantFromValue(res->Name());
					}

					case 1:
					{
						return qVariantFromValue(res->Size());
					}

					case 2:
					{
						return qVariantFromValue(res->Leechers());
					}

					case 3:
					{
						return qVariantFromValue(res->Seeders());
					}

					case 4:
					{
						return qVariantFromValue(res->Engine());
					}
				}
			}

			case SearchItemRole:
			{
				return qVariantFromValue(res);
			}
		}
	}

	return QVariant();
}

void QSearchDisplayModel::OnNewSearchResults()
{
	reset();
}




