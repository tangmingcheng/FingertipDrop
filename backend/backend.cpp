/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Design Studio.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "backend.h"
#include "syncserver.h"

int i = 0;

Backend::Backend(QObject *parent) :
    QObject(parent)
    ,m_httpServerAddress("")
    ,m_httpServerPort(0)
{
    syncServer = new SyncServer(this);

    connect(syncServer,&SyncServer::deviceConnected,this,&Backend::addDeviceListData);
    connect(this, &Backend::filePathReady, syncServer, &SyncServer::handleFilePath);
    connect(syncServer, &SyncServer::clientsChanged, this, &Backend::updateClientCount);
    connect(syncServer, &SyncServer::clientsStatusChanged, this, &Backend::updateClientStatus);

    // 动态绑定 SyncServer 的绑定值
    m_httpServerAddress.setBinding([this]() {
        return syncServer->bindableHttpServerAddress().value();
    });
    m_httpServerPort.setBinding([this]() {
        return syncServer->bindableHttpServerPort().value();
    });
}

void Backend::sendFile(const QString &filePath)
{
    emit filePathReady(filePath);
}

void Backend::startButtonClicked()
{
    qDebug() << " Start Button was clicked!";

    syncServer->startServers();
}

void Backend::addDeviceListData(const QString &deviceName, const QString &deviceIP, const QString &status)
{
    m_listModel.addItem(deviceName,deviceIP,status);
}

MyListModel* Backend::listModel()
{
    return &m_listModel;
}

QString Backend::httpServerAddress() const
{
    qDebug()<<"[httpServerAddress]"<<m_httpServerAddress;
    return m_httpServerAddress;
}

QBindable<QString> Backend::bindableHttpServerAddress()
{
    qDebug()<<"[bindableHttpServerAddress]"<<m_httpServerAddress;
    return &m_httpServerAddress;
}

quint16 Backend::httpServerPort() const
{
    qDebug()<<"[httpServerPort]"<<m_httpServerPort;
    return m_httpServerPort;
}

QBindable<quint16> Backend::bindableHttpServerPort()
{
    qDebug()<<"[bindableHttpServerPort]"<<m_httpServerPort;
    return &m_httpServerPort;
}

