/*
    qtparted - a frontend to libparted for manipulating disk partitions
    Copyright (C) 2002-2003 Vanni Brutto

    Vanni Brutto <zanac (-at-) libero dot it>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <qregexp.h>

#include "qp_fswrap.h"
#include "qp_common.h"
#include "qp_options.h"

#define NOTFOUND tr("command not found")

bool QP_FSWrap::fs_open(QString cmdline) {

    /*---this force stderr to stdout---*/
    QString dupcmdline = QString("%1 %2")
                    .arg(cmdline)
                    .arg(QString("2>&1"));

    /*---open a pipe from the command line---*/
    fp = popen(dupcmdline, "r");

    if (fp)
        return true;
    else
        return false;
}

char * QP_FSWrap::fs_getline() {
    bool rc = fgets(line, sizeof line, fp);

    if (rc) return line;
    else    return NULL;
}

int QP_FSWrap::fs_close() {
    return fclose(fp);
}

QP_FSWrap * QP_FSWrap::fswrap(QString name) {
    if (name.compare("ntfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSNtfs();
        return fswrap;
    } else
    if (name.compare("jfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSJfs();
        return fswrap;
    } else
    if (name.compare("ext3") == 0) {
        QP_FSWrap *fswrap = new QP_FSExt3();
        return fswrap;
    } else
    if (name.compare("xfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSXfs();
        return fswrap;
    }
    else
        return NULL;
}



/*---NTFS WRAPPER-----------------------------------------------------------------*/
QP_FSNtfs::QP_FSNtfs() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKNTFS));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();

    /*---check if the wrapper is installed---*/
    cmdline = QString("which %1")
            .arg(lstExternalTools->getPath(NTFSRESIZE));
    fs_open(cmdline);

    while ((cline = fs_getline()))
        wrap_resize = true;
    fs_close();
}

bool QP_FSNtfs::resize(bool write, QString dev, PedSector newsize) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---calculate size of the partition in bytes---*/
    PedSector size = (PedSector)((newsize-1)*512);

    /*---read-only test---*/
    sprintf(szcmdline, "-n -ff -s %lld %s", size, dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(NTFSRESIZE))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool error = false;
    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;

        if (!error) {
            rx = QRegExp("^ERROR.*: (.*)");
            if (rx.search(line) == 0) {
                QString captured = rx.cap(1);
                _message = QString(captured);
                error = true;
            }
        }

        if (!error) {
            rx = QRegExp("^The volume end is fragmented.*");
            if (rx.search(line) == 0) {
                _message = QString("The partition is fragmented.");
                error = true;
            }
        }

        rx = QRegExp("^Now You could resize at \\d* bytes or (\\d*) .*");
        if (rx.search(line) == 0) {
            QString captured = rx.cap(1);
            _message = QString("The partition is fragmented. Try to defragment it, or resize to %1MB")
                        .arg(captured);
            error = true;
        }
    }
    fs_close();

    if (error) return false;

    /*---if the user want to run a readonly test just return true---*/
    if (!write) return true;


    /*---ok, the readonly test seems ok... now we resize it!---*/
    sprintf(szcmdline, "-ff -s %lld %s", size, dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(NTFSRESIZE))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^Successfully resized NTFS on device");
        if (rx.search(line) == 0)
            success = true;
        rx = QRegExp("^Nothing to do: NTFS volume size is already OK.");
        if (rx.search(line) == 0)
            success = true;
        rx = QRegExp("^ERROR.*: (.*)");
        if (rx.search(line) == 0) {
            QString captured = rx.cap(1);
            _message = QString(captured);
            printf("messsss: %s\n", _message.latin1());
        }
    }
    fs_close();

    if (success) return true;
    else {
        if (_message.isNull()) _message = QString("An error occured! :(");
        return false;
    }
}

bool QP_FSNtfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-f -s 512 %s", dev.latin1());
    else
        sprintf(szcmdline, "-f -s 512 -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKNTFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^mkntfs completed successfully. Have a nice day.");
        if (rx.search(line) == 0)
            success = true;

        rx = QRegExp("^ERROR.*: (.*)");
        if (rx.search(line) == 0) {
            QString captured = rx.cap(1);
            _message = QString(captured);
            success = false;
        }
    }
    fs_close();

    return success;
}

QString QP_FSNtfs::fsname() {
    return QString("ntfs");
}



/*---JFS WRAPPER-----------------------------------------------------------------*/
QP_FSJfs::QP_FSJfs() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_JFS));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
}

bool QP_FSJfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-q %s", dev.latin1());
    else
        sprintf(szcmdline, "-q -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_JFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^Format completed successfully.");
        if (rx.search(line) == 0)
            success = true;
    }
    fs_close();

    return success;
}

QString QP_FSJfs::fsname() {
    return QString("jfs");
}



/*---EXT3 WRAPPER----------------------------------------------------------------*/
QP_FSExt3::QP_FSExt3() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_EXT3));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
}

bool QP_FSExt3::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "%s", dev.latin1());
    else
        sprintf(szcmdline, "-L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_EXT3))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool writenode = false;
    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^Writing inode tables");
        if (rx.search(line) == 0) {
            writenode = true;
        }

        rx = QRegExp("^Creating journal");
        if (rx.search(line) == 0) {
            writenode = false;
            emit sigTimer(90, QString(tr("Writing superblocks and filesystem.")), QString::null);
        }

        if (writenode) {
            QString linesub = line;

#ifdef QT30COMPATIBILITY
            linesub.replace(QRegExp("\b"), " ");
#else
            linesub.replace(QChar('\b'), " ");
#endif
            rx = QRegExp("^.* (\\d*)/(\\d*) .*$");
            if (rx.search(linesub) == 0) {
                QString capActual = rx.cap(1);
                QString capTotal = rx.cap(2);
                
                bool rc;
                int iActual = capActual.toInt(&rc);
                int iTotal = capTotal.toInt(&rc);

                int iPerc = iActual*80/iTotal; //The percentual is calculated in 80% ;)
                emit sigTimer(iPerc, QString(tr("Writing inode tables.")), QString::null);
            }
        }

        rx = QRegExp("^Writing superblocks and filesystem accounting information: done");
        if (rx.search(line) == 0)
            success = true;
    }
    fs_close();

    return success;
}

QString QP_FSExt3::fsname() {
    return QString("ext3");
}

/*---XFS WRAPPER-----------------------------------------------------------------*/
QP_FSXfs::QP_FSXfs() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_XFS));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
}

bool QP_FSXfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-q -f %s", dev.latin1());
    else
        sprintf(szcmdline, "-q -f -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_XFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^Format completed successfully.");
        if (rx.search(line) == 0)
            success = true;
    }
    fs_close();

    return success;
}

QString QP_FSXfs::fsname() {
    return QString("xfs");
}
