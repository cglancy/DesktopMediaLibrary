#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include "textindex.h"

class TreeModel;
class ListModel;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);

    void loadData();

    TreeModel * treeModel() const;
    ListModel * listModel() const;

    Q_INVOKABLE void search(const QString &text);
    Q_INVOKABLE void setCategory(const QModelIndex &index);

private:
    TreeModel *_treeModel;
    ListModel *_listModel;
    TextIndex _textIndex;
};

#endif // MAINCONTROLLER_H