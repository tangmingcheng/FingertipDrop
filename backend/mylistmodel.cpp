#include "mylistmodel.h"

MyListModel::MyListModel(QObject *parent) : QAbstractListModel(parent)
{
}

int MyListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

QVariant MyListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Item &item = m_data.at(index.row());

    switch (role) {
    case Value1Role:
        return item.data1;
    case Value2Role:
        return item.data2;
    case Value3Role:
        return item.data3;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MyListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Value1Role] = "data1";
    roles[Value2Role] = "data2";
    roles[Value3Role] = "data3";
    return roles;
}

void MyListModel::addItem(const QString &data1, const QString &data2,const QString &data3)
{
    /*add bottom
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_data.append({data1,data2,data3});*/

    /*add top*/
    beginInsertRows(QModelIndex(), 0, 0);
    m_data.prepend({data1,data2,data3});

    endInsertRows();
}
