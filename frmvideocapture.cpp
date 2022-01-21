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
#include "frmvideocapture.h"
#include "ui_frmvideocapture.h"

FrmVideoCapture::FrmVideoCapture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmVideoCapture)
{
    ui->setupUi(this);
}

FrmVideoCapture::~FrmVideoCapture()
{
    delete ui;
}

void FrmVideoCapture::on_buttonBox_accepted()
{
    emit(OK(this));
}

void FrmVideoCapture::on_buttonBox_rejected()
{
    emit(Cancel(this));
}

void FrmVideoCapture::on_btnOK_clicked()
{
    emit(OK(this));
}
