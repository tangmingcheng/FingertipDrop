#ifndef MYLISTMODEL_H
#define MYLISTMODEL_H

#include <QStandardItemModel>
#include <QStringList>
#include <QVariant>

class MyListModel : public QAbstractListModel
{
public:
    struct Item {
        QString data1;
        QString data2;
        QString data3;
    };

    explicit MyListModel(QObject *parent = nullptr);

    enum Roles {
        Value1Role = Qt::UserRole + 1,
        Value2Role,
        Value3Role
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public:
    void addItem(const QString &value1, const QString &value2,const QString &value3);

private:
    QList<Item> m_data;
};

#endif // MYLISTMODEL_H
