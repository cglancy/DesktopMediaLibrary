#include "maincontroller.h"
#include "treemodel.h"
#include "listmodel.h"

#include <QRegExp>
#include <QDebug>

MainController::MainController(QObject *parent)
    : QObject(parent),
      _treeModel(0),
      _listModel(0)
{
    _treeModel = new TreeModel(this);
    _listModel = new ListModel(this);
}

void MainController::loadData()
{
    _treeModel->initWithFile("C:\\Test\\lds-media-library-eng.xml");
    _listModel->filterByCategory(_treeModel->rootCategory());

    _textIndex.build(_treeModel->videos());
}

void MainController::search(const QString &text)
{
    if (text.size() > 0)
    {
        TextIndex::ResultsHash results;
        _textIndex.search(text, results);

        qDebug() << "Found " << results.size() << " videos with " << text << ".";
        _listModel->setSearchResults(results);
    }
    else
    {
        _listModel->clearSearchResults();
    }
}

TreeModel * MainController::treeModel() const
{
    return _treeModel;
}

ListModel * MainController::listModel() const
{
    return _listModel;
}

void MainController::setCategory(const QModelIndex &index)
{
    CategoryNode *category = _treeModel->category(index);
    if (category)
        _listModel->filterByCategory(category);
}
