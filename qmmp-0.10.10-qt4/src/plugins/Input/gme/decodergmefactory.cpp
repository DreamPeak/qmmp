/***************************************************************************
 *   Copyright (C) 2010-2017 by Ilya Kotov                                 *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QRegExp>
#include <QMessageBox>
#include <QTranslator>
#include <QtPlugin>
#include "settingsdialog.h"
#include "gmehelper.h"
#include "decoder_gme.h"
#include "decodergmefactory.h"

// DecoderGmeFactory

bool DecoderGmeFactory::supports(const QString &source) const
{
    foreach(QString filter, properties().filters)
    {
        QRegExp regexp(filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        if (regexp.exactMatch(source))
            return true;
    }
    return false;
}

bool DecoderGmeFactory::canDecode(QIODevice *) const
{
    return false;
}

const DecoderProperties DecoderGmeFactory::properties() const
{
    DecoderProperties properties;
    properties.name = tr("GME Plugin");
    properties.filters << "*.ay" << "*.gbs" << "*.gym" << "*.hes" << "*.kss" << "*.nsf" << "*.nsfe";
    properties.filters << "*.sap" << "*.spc" << "*.vgm" << "*.vgz";
    properties.description = tr("Game Music Files");
    //properties.contentType = ;
    properties.shortName = "gme";
    properties.hasAbout = true;
    properties.hasSettings = true;
    properties.noInput = true;
    properties.protocols << "gme";
    return properties;
}

Decoder *DecoderGmeFactory::create(const QString &path, QIODevice *input)
{
    Q_UNUSED(input);
    return new DecoderGme(path);
}

QList<FileInfo *> DecoderGmeFactory::createPlayList(const QString &fileName, bool useMetaData, QStringList *ignoredFiles)
{
    QList <FileInfo*> list;
    GmeHelper helper;
    //is it one track?
    if(fileName.contains("://"))
    {
        QString path = fileName;
        path.remove("gme://");
        path.remove(QRegExp("#\\d+$"));
        int track = fileName.section("#", -1).toInt();
        list = createPlayList(path, true, ignoredFiles);
        if (list.isEmpty() || track <= 0 || track > list.count())
        {
            qDeleteAll(list);
            list.clear();
            return list;
        }
        FileInfo *info = list.takeAt(track - 1);
        qDeleteAll(list);
        return QList<FileInfo *>() << info;
    }

    Music_Emu *emu = helper.load(fileName);
    if(!emu)
    {
        qWarning("DecoderGmeFactory: unable to open file");
        return list;
    }
    list = helper.createPlayList(useMetaData);
    return list;
}

MetaDataModel* DecoderGmeFactory::createMetaDataModel(const QString &path, QObject *parent)
{
    Q_UNUSED(path);
    Q_UNUSED(parent);
    return 0;
}

void DecoderGmeFactory::showSettings(QWidget *parent)
{
   SettingsDialog *d = new SettingsDialog(parent);
   d->show();
}

void DecoderGmeFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About GME Audio Plugin"),
                        tr("Qmmp GME Audio Plugin")+"\n"+
                        tr("This plugin uses Game_Music_Emu library to play game music files")+"\n"+
                        tr("Written by: Ilya Kotov <forkotov02@hotmail.ru>"));
}

QTranslator *DecoderGmeFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = Qmmp::systemLanguageID();
    translator->load(QString(":/gme_plugin_") + locale);
    return translator;
}
Q_EXPORT_PLUGIN2(gme,DecoderGmeFactory)
