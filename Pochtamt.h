/*

Copyright (c) 2018 Д.С. Андреев

Этот файл — часть Pochtamt.

jam - свободная программа: вы можете перераспространять ее и/или
изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
в каком она была опубликована Фондом свободного программного обеспечения;
либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
версии.

Письмуй распространяется в надежде, что она будет полезной,
но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
общественной лицензии GNU.

Вы должны были получить копию Стандартной общественной лицензии GNU
вместе с этой программой. Если это не так, см.
<http://www.gnu.org/licenses/>.)


This file is part of Pochtamt.

jam is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Composer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Composer.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef POCHTAMT_H
#define	POCHTAMT_H

#include <QtCore>
#include <QtNetwork>

class Pochtamt : public QObject {
    Q_OBJECT
private:
    static QTextCodec * _codec;
    
    QString _service;
    QUdpSocket * _channel;
    unsigned int _uid;
    
    QMap<quint64,QDateTime> _hosts_map;
public:
    Pochtamt(const QString & service,QObject * parent = 0);
    ~Pochtamt(void);
public slots:
    static void message(const QString & text);
    bool bind(quint16 port = 6888);
    
    void recieveDatagram(void);    
    void keepAlive(void);
    };

#endif	/* POCHTAMT_H */

