// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "autogen/environment.h"

#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <Windows.h>

void registerContextMenu(const QString &exePath, const QString &iconPath) {
    HKEY hKey;
    LONG result;

    // 定义注册表路径
    QString fileKeyPath = R"(Software\Classes\*\shell\FingertipDrop)";
    QString folderKeyPath = R"(Software\Classes\Directory\shell\FingertipDrop)";
    QString desktopKeyPath = R"(Software\Classes\DesktopBackground\shell\FingertipDrop)";
    QString commandFileKeyPath = fileKeyPath + "\\command";
    QString commandFolderKeyPath = folderKeyPath + "\\command";
    QString commandDesktopKeyPath = desktopKeyPath + "\\command";

    // 转换为Windows路径分隔符
    QString fixedExePath = QDir::toNativeSeparators(exePath);
    QString fixedIconPath = QDir::toNativeSeparators(iconPath);

    // 文件右键菜单
    result = RegCreateKeyEx(HKEY_CURRENT_USER, fileKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)L"Open with FingertipDrop",
                      (DWORD)(wcslen(L"Open with FingertipDrop") * sizeof(wchar_t)));
        RegSetValueEx(hKey, L"Icon", 0, REG_SZ, (const BYTE*)fixedIconPath.toStdWString().c_str(),
                      (DWORD)(fixedIconPath.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create file context menu key.";
        return;
    }

    result = RegCreateKeyEx(HKEY_CURRENT_USER, commandFileKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        QString command = QString("\"%1\" \"%2\"").arg(fixedExePath).arg("%1");
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)command.toStdWString().c_str(),
                      (DWORD)(command.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create file command key.";
        return;
    }

    // 文件夹右键菜单
    result = RegCreateKeyEx(HKEY_CURRENT_USER, folderKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)L"Open with FingertipDrop",
                      (DWORD)(wcslen(L"Open with FingertipDrop") * sizeof(wchar_t)));
        RegSetValueEx(hKey, L"Icon", 0, REG_SZ, (const BYTE*)fixedIconPath.toStdWString().c_str(),
                      (DWORD)(fixedIconPath.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create folder context menu key.";
        return;
    }

    result = RegCreateKeyEx(HKEY_CURRENT_USER, commandFolderKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        QString command = QString("\"%1\" \"%2\"").arg(fixedExePath).arg("%1");
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)command.toStdWString().c_str(),
                      (DWORD)(command.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create folder command key.";
        return;
    }

    // 添加桌面背景右键菜单
    result = RegCreateKeyEx(HKEY_CURRENT_USER, desktopKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)L"Open FingertipDrop",
                      (DWORD)(wcslen(L"Open FingertipDrop") * sizeof(wchar_t)));
        RegSetValueEx(hKey, L"Icon", 0, REG_SZ, (const BYTE*)fixedIconPath.toStdWString().c_str(),
                      (DWORD)(fixedIconPath.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create desktop context menu key.";
        return;
    }

    // 添加桌面背景右键菜单的命令
    result = RegCreateKeyEx(HKEY_CURRENT_USER, commandDesktopKeyPath.toStdWString().c_str(),
                            0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS) {
        QString command = QString("\"%1\"").arg(fixedExePath);
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (const BYTE*)command.toStdWString().c_str(),
                      (DWORD)(command.toStdWString().size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    } else {
        qWarning() << "Failed to create desktop command key.";
        return;
    }

    qDebug() << "Context menu registered successfully.";
}

int main(int argc, char *argv[])
{
    set_qt_environment();
    QGuiApplication app(argc, argv);

    QString exePath = QGuiApplication::applicationFilePath();
    QString dirPath = QGuiApplication::applicationDirPath();
    QString iconPath = dirPath + "/FingertipDrop.ico";

    registerContextMenu(exePath,iconPath);

    QDir::setCurrent(dirPath);

    QQmlApplicationEngine engine;
    const QUrl url(mainQmlFile);
    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
