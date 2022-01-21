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
#include "frmmain.h"
#include "ui_frmmain.h"
#include <QDebug>
#include <QTime>
#include <QDate>

// Needed for drag & drop events
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>

#include <QMessageBox>

#include "frmvideocapture.h"
#include "frmabout.h"

FrmMain::FrmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmMain)
{
    ui->setupUi(this);

    process = NULL;
    selectedDevice = -1;
    scanForNewDevices();

    // Accept drops for .APK installation
    setAcceptDrops(true);
}

FrmMain::~FrmMain()
{
    for(int i = 0; i < device.size(); ++i)
    {
        delete device[i];
        device[i] = NULL;
    }//for

    delete ui;
}

//------------------------------------------------------------------------
// Scan for devices
//------------------------------------------------------------------------
void FrmMain::scanForNewDevices()
{
    // ADB needs to be in PATH
    if(process == NULL)
    {
        process = new QProcess();
        process->start("adb", QStringList() << "devices" << "-l");

        connect(process, SIGNAL(finished(int)), this, SLOT(processFinished()));
        connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(receiveDevices()));
    }//if
}


//------------------------------------------------------------------------
// Displays data of the currently selected device
//------------------------------------------------------------------------
void FrmMain::displayDeviceInformation()
{
    if(selectedDevice >= 0)
    {
        // Device information
        ui->edtModel->setText(device[selectedDevice]->GetDeviceInformation()->model);
        ui->edtSWVer->setText(device[selectedDevice]->GetDeviceInformation()->swVersion);
        ui->edtOSVer->setText(device[selectedDevice]->GetDeviceInformation()->osVersion);
        ui->edtMaxAppSDK->setText(device[selectedDevice]->GetDeviceInformation()->maxAppSDK);
        ui->edtCSC->setText(device[selectedDevice]->GetDeviceInformation()->csc);
        ui->gbDeviceInformation->setTitle(device[selectedDevice]->GetDeviceInformation()->model);
    }
    else
    {
        // Device information
        ui->edtModel->setText("");
        ui->edtSWVer->setText("");
        ui->edtOSVer->setText("");
        ui->edtMaxAppSDK->setText("");
        ui->edtCSC->setText("");
        ui->gbDeviceInformation->setTitle("<Device Name>");
    }
}

//------------------------------------------------------------------------
// Public Slots
//------------------------------------------------------------------------
void FrmMain::processFinished()
{
    process->kill();
    process = NULL;
}

//------------------------------------------------------------------------
// Show selected item
//------------------------------------------------------------------------
void FrmMain::receiveDevices()
{
    Device* dev;
    QString id; // Single device ID
    QString string = process->readAllStandardOutput(); // Unprocessed process output;

    //qDebug() << string;

    // Remove first line ("List of devices attached")
    string = string.remove(0, string.indexOf("\n")+1);

    // Grab all device ids
    while(1)
    {
        id = string.mid(0, string.indexOf(" "));

        // Triple check for end
        if(id == "") break;
        if(id.indexOf(" ") == 0) break;
        if(id.indexOf("\n") > 0 && id.indexOf("\n") < 4) break;

        // Everything below five characters is probably not a device
        if(id.length() < 5) break;

        //qDebug() << "Device found, id: " << id;

        // Create a new device
        dev = new Device(id);
        dev->ObtainInformation();
        connect(dev, SIGNAL(update()), this, SLOT(updateDevices()));
        device.append(dev);

        string.remove(0, string.indexOf("\n")+1);
    }// while
}

//------------------------------------------------------------------------
// Update device list
//------------------------------------------------------------------------
void FrmMain::updateDevices()
{
    ui->lwDevices->clear();
    for(int i = 0; i < device.size(); ++i)
    {
        ui->lwDevices->addItem(QString::number(i+1) + ". " + device[i]->GetDeviceInformation()->model);
    }//for
}

//------------------------------------------------------------------------
// Show selected item
//------------------------------------------------------------------------
void FrmMain::on_lwDevices_currentRowChanged(int currentRow)
{
    selectedDevice = currentRow;
    displayDeviceInformation();
}

//------------------------------------------------------------------------
// Devices refresh reqeusted
//------------------------------------------------------------------------
void FrmMain::on_actionRefresh_Device_triggered()
{
    // Remove all devices
    for(int i = 0; i < device.size(); ++i)
    {
        delete device[i];
        device[i] = NULL;
    }//for

    device.clear();

    // Reset selection
    selectedDevice = -1;

    // Scan and display new devices
    scanForNewDevices();
    updateDevices();
    displayDeviceInformation();
}

//------------------------------------------------------------------------
// Show about window
//------------------------------------------------------------------------
void FrmMain::on_actionAbout_triggered()
{
    FrmAbout fa;
    fa.exec();
}

//------------------------------------------------------------------------
// Opens "This PC" explorer on windows
//------------------------------------------------------------------------
void FrmMain::on_btnDirectory_clicked()
{
    QProcess::startDetached("explorer \"\"");
}

//------------------------------------------------------------------------
// Take a screenshot
//------------------------------------------------------------------------
void FrmMain::on_btnScreenshot_clicked()
{
    if(selectedDevice >= 0)
    {
        device[selectedDevice]->TakeScreenShot();
    }
}

//------------------------------------------------------------------------
// Start & stop screen recording
//------------------------------------------------------------------------
void FrmMain::on_btnVideo_clicked()
{
    if(process == NULL)
    {
        device[selectedDevice]->StartScreenRecording();

        FrmVideoCapture vc;
        connect(&vc, SIGNAL(OK(QDialog*)), this, SLOT(screenRecorderFrmClosed(QDialog*)));
        connect(&vc, SIGNAL(Cancel(QDialog*)), this, SLOT(screenRecorderFrmClosed(QDialog*)));
        vc.exec();
    }
}

void FrmMain::screenRecorderFrmClosed(QDialog *dialog)
{
    device[selectedDevice]->StopScreenRecording();
    //dialog->close();
}

void FrmMain::screenRecordingFinished()
{
    //dialog->close();
}

//------------------------------------------------------------------------
// Drag & Drop handling
//------------------------------------------------------------------------
void FrmMain::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void FrmMain::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();

        if(selectedDevice >= 0)
        {
            device[selectedDevice]->InstallAPK(fileName);
        }
    }
}
