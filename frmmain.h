/*
    TEsting tool for Android (TEA) Copyright (C) 2015-2022 bandrewk (Bryan Andrew King)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QProcess>
#include "device.h"
#include <QListWidget>

namespace Ui {
class FrmMain;
}

class FrmMain : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FrmMain(QWidget *parent = 0);
    ~FrmMain();
    
private:
    Ui::FrmMain*    ui;
    QProcess*       process;
    int             selectedDevice;
    QVector<Device*> device;

    void    scanForNewDevices();
    void    displayDeviceInformation();

    void    dragEnterEvent(QDragEnterEvent *e);
    void    dropEvent(QDropEvent *e);

public slots:
    void    processFinished();
    void    receiveDevices();
    void    updateDevices();
    void    screenRecorderFrmClosed(QDialog* dialog);
    void    screenRecordingFinished();

private slots:
    void on_lwDevices_currentRowChanged(int currentRow);
    void on_actionRefresh_Device_triggered();
    void on_actionAbout_triggered();
    void on_btnDirectory_clicked();
    void on_btnScreenshot_clicked();
    void on_btnVideo_clicked();
};

#endif // FRMMAIN_H
