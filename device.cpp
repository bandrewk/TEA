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
#include "device.h"

#include <QDebug>
#include <QTime>
#include <QDate>
#include <QStandardPaths>
#include <QFile>
#include <QMessageBox>
#include <windows.h>

const QString Device::FileExtVideo = ".mp4";
const QString Device::FileExtPicture = ".png";

Device::Device(QString id)
{
    data.id = id;
    currentRequest = Model;
    IsScreenRecording = false;
    process = NULL;
}

//------------------------------------------------------------------------
// Query device information
//------------------------------------------------------------------------
void Device::ObtainInformation()
{
    if(process == NULL)
    {
        QString item;
        switch(currentRequest)
        {
            case Model:
            {
                item = "ro.product.model";
            }break;

            case SWVersion:
            {
                item = "ro.build.version.incremental";
            }break;

            case OSVersion:
            {
                item = "ro.build.version.release";
            }break;

            case MaxAppSDK:
            {
                item = "ro.build.version.sdk";
            }break;

            case CSC:
            {
                item = "ro.csc.sales_code";
            }break;
        }//switch

        process = new QProcess();
        process->start("adb", QStringList() << "-s" << data.id << "shell" << "getprop" << item);

        connect(process, SIGNAL(finished(int)), this, SLOT(processFinished()));
    }
}

//------------------------------------------------------------------------
// Takes a single screenshot, moves it to the host pc and removes any leftovers from the device
//------------------------------------------------------------------------
void Device::TakeScreenShot()
{
    QString filename, destination;
    filename =  QDate::currentDate().toString("yyyyMMdd") + QTime::currentTime().toString("hhmmss");
    destination = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/"+filename+"-screenshot" + FileExtPicture;

    // Step 1: Take screenshot and save it to phone storage
    QProcess proc;
    proc.execute("adb", QStringList() << "-s" << data.id << "shell" <<  "screencap" << "-p" << "/sdcard/"+filename+"-screenshot"+FileExtPicture);

    // Step 2: Copy screenshot to PC
    proc.execute("adb", QStringList() << "-s" << data.id << "pull" << "/sdcard/"+filename+"-screenshot" + FileExtPicture << destination);

    // Step 3: Delete screenshot on phone
    proc.execute("adb", QStringList() << "-s" << data.id << "shell" << "rm" << "/sdcard/"+filename+"-screenshot" + FileExtPicture);

    // Step 4: Check if the written file actually exists and notify user
    if(QFile::exists(destination))
    {
        QMessageBox::information(NULL, "Success!", "Screenshot taken. Saved " + filename + "-screenshot" + FileExtPicture +" on your desktop.");
    }
    else QMessageBox::warning(NULL, "Failed!", "Taking screenshot failed. Double-check phone connection and available disk space.");
}

//------------------------------------------------------------------------
// Starts a video screen recording
//------------------------------------------------------------------------
void Device::StartScreenRecording()
{
    if(process == NULL)
    {
        screenRecordFileName =  QDate::currentDate().toString("yyyyMMdd") + QTime::currentTime().toString("hhmmss");

        process = new QProcess();
        process->start("adb", QStringList() << "-s" << data.id << "shell" << "screenrecord" << "/sdcard/"+screenRecordFileName+".mp4");

        IsScreenRecording = true;
    }
}

//------------------------------------------------------------------------
// Stops a running video screen recording, moves it to the host pc and removes any leftovers from the device
//------------------------------------------------------------------------
void Device::StopScreenRecording()
{
    if(IsScreenRecording)
    {
        qDebug() << "StopScreenRecording()";

        // Step 1: Check if process is really running on the device
        QProcess proc;
        proc.start("adb", QStringList() << "-s" << data.id << "shell");
        proc.write("top | grep screenrecord");
        proc.closeWriteChannel();
        proc.waitForReadyRead(10000);
        QString output = proc.readAllStandardOutput();
        output = output.trimmed();
        proc.kill();

        // Step 2: Kill the process, this will stop the screenrecord command as well
        process->kill();
        process = NULL;

        // Step 3: If the screenrecord process was found copy the video to desktop and remove it form device
        if(output.length() > 0)
        {
            QString destination = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/"+screenRecordFileName+FileExtVideo;
            output = output.mid(0, output.indexOf(" "));

            // Copy data to PC
            proc.execute("adb", QStringList() << "-s" << data.id << "pull" << "/sdcard/"+screenRecordFileName+FileExtVideo << destination);

            // Remove video from phone
            proc.execute("adb", QStringList() << "-s" << data.id << "shell" << "rm" << "/sdcard/"+screenRecordFileName+FileExtVideo);

        }//if
        else
        {
            QMessageBox::warning(NULL, "Failed!", "Screenrecord process not found. Please check phone storage (root)!");
        }

        IsScreenRecording = false;
    }
}

//------------------------------------------------------------------------
// Sideloads an application to the device
// Note that this is blocking and can take a while depending on the size of the app
//------------------------------------------------------------------------
void Device::InstallAPK(const QString file)
{
    // adb install path
    QProcess proc;
    proc.execute("adb", QStringList() << "-s" << data.id << "install" << file);

    QMessageBox::information(NULL, "Success!", "Application installed. Please check device.");
}

//------------------------------------------------------------------------
// Process requested device information. Works hand in hand with ObtainInformation() method
//------------------------------------------------------------------------
void Device::processFinished()
{
    // Read process output and terminate it
    QString out = process->readAllStandardOutput();
    process->kill();
    process = NULL;

    // Remove new line
    out = out.remove(out.indexOf("\n"), out.indexOf("\n")+1);

    switch(currentRequest)
    {
        case Model:
        {
            data.model = out;
        }break;

        case SWVersion:
        {
            data.swVersion = out;
        }break;

        case OSVersion:
        {
            data.osVersion = out;
        }break;

        case MaxAppSDK:
        {
            data.maxAppSDK = out;
        }break;

        case CSC:
        {
            data.csc = out;
        }break;
    }//switch

   // qDebug() << "Read: " << out;

    // Continue to grab information..
    currentRequest++;
    if(currentRequest <= CSC)
    {
        ObtainInformation();
    }//if
    else
    {
        emit(update());
    }
}

DeviceInformation* Device::GetDeviceInformation()
{
    return &data;
}
