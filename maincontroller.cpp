#include "maincontroller.h"
#include "treemodel.h"
#include "listmodel.h"
#include "filemanager.h"

#include <QRegExp>
#include <QDesktopServices>
#include <QDebug>

MainController::MainController(QObject *parent)
    : QObject(parent),
      _treeModel(0),
      _listModel(0),
      _fileManager(0)
{
    _treeModel = new TreeModel(this);
    _listModel = new ListModel(this);
    _fileManager = new FileManager(this);
}

void MainController::loadData()
{
    _treeModel->initWithFile("C:\\Test\\lds-media-library.xml");
    _listModel->filterByCategory(_treeModel->rootCategory());

    _textIndex.build(_treeModel->videos());
}

void MainController::search(const QString &text)
{
    if (text.size() > 0)
    {
        TextIndex::ResultsHash results;
        _textIndex.search(text, results);
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

void MainController::clickVideoButton(const QString &url)
{
    qDebug() << "MainController::clickVideoButton(" << url << ")";

    FileManager::FileState state = _fileManager->fileState(url);
    if (state == FileManager::NotDownloadedState)
        _fileManager->downloadFile(url);
    else if (state == FileManager::DownloadedState)
    {
        QString fileUrl = "file:///" + _fileManager->localFilePath(url);
        QDesktopServices::openUrl(fileUrl);
    }
}
