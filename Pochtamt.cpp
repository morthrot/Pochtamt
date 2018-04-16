/*

Copyright (c) 2018 Д.С. Андреев

Этот файл — часть Pochtamt.

Pochtamt - свободная программа: вы можете перераспространять ее и/или
изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
в каком она была опубликована Фондом свободного программного обеспечения;
либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
версии.

Pochtamt распространяется в надежде, что она будет полезной,
но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
общественной лицензии GNU.

Вы должны были получить копию Стандартной общественной лицензии GNU
вместе с этой программой. Если это не так, см.
<http://www.gnu.org/licenses/>.)


This file is part of Pochtamt.

Pochtamt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pochtamt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Composer.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Pochtamt.h"

QTextCodec * Pochtamt::_codec = QTextCodec::codecForName(
#ifdef Q_OS_WIN32
    "CP866");
#else
    "UTF-8");
#endif

Pochtamt::Pochtamt(const QString & service,QObject * parent) : QObject(parent) {
    message("Инициализация экземпляра");
    
    _service = service;
    
    _channel = new QUdpSocket(this);
    connect(_channel,SIGNAL(readyRead()),this,SLOT(recieveDatagram()));

    _uid = rand();    
    
    QTimer * timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(keepAlive()));
    timer->start(15 * 60 * 1000);
    }

Pochtamt::~Pochtamt(void) {
    message("Деинициализация экземпляра");
    }

void Pochtamt::message(const QString & text) {
    QDateTime current_dt = QDateTime::currentDateTime();
    
    QString entry = QString("[%1 %2] %3")
            .arg(current_dt.date().toString("dd.MM.yyyy"))
            .arg(current_dt.time().toString("HH:mm:ss.zzz"))
            .arg(text);

    printf("%s\n", _codec->fromUnicode(entry).constData());    
    }

bool Pochtamt::bind(quint16 port) {
    if(_channel->state() != QAbstractSocket::UnconnectedState) {
        message("Сокет не находится в неподключённом состоянии");
        return false;
        }
    
    bool bind_ok = _channel->bind(QHostAddress::Any,port,QUdpSocket::ShareAddress);
    if(bind_ok == false) {
        message( QString("Ошибка привязки сокета к порту %1").arg(port) );
        return false;
        }
    
    message( QString("Успешная привязка сокета к порту %1").arg(port) );
    return true;
    }

void Pochtamt::recieveDatagram(void) {
    while(_channel->hasPendingDatagrams() == true) {
        QByteArray bytes(_channel->pendingDatagramSize(),0x00);
        QHostAddress host; quint16 port;
        _channel->readDatagram(bytes.data(),bytes.length(),&host,&port);

        message( QString("Входящее сообщение от %1:%2").arg(host.toString()).arg(port) );
        
        QDataStream ds(bytes);
        ds.setVersion(QDataStream::Qt_4_7);
        
        QVariantMap map;
        ds >> map;
        
        if(map.value("service") != _service) {
            message("Сообщение не содержит или содержит неправильный service");
            continue;
            }
        
        if(map.contains("uid") == false) {
            message("Сообщение не содержит uid");
            continue;
            }

        unsigned int uid = map.value("uid").toUInt();
        if(uid == _uid) { message("Уже обработанный uid"); continue; }
        else { _uid = uid; }

        _hosts_map[ (quint64)host.toIPv4Address() | (quint64)port << 32 ] = QDateTime::currentDateTime();
        
        QList<quint64> peer_list = _hosts_map.keys();
        for(int i=0; i<peer_list.count(); i++) {
            quint64 peer = peer_list.at(i);
            
            QHostAddress peer_host( (quint32)(peer & 0xFFFFFFFFLL) );
            quint16 peer_port = (quint16)(peer >> 32);

            message( QString("Отправка сообщения %1:%2").arg(peer_host.toString()).arg(peer_port) );
            _channel->writeDatagram(bytes,peer_host,peer_port);
            }        
        }
    }

void Pochtamt::keepAlive(void) {
    message("Очистка кэша пиров");
    QDateTime current_dt = QDateTime::currentDateTime();
    
    QList<quint64> peer_list = _hosts_map.keys();
    for(int i=0; i<peer_list.count(); i++) {
        quint64 peer = peer_list.at(i);        
        QDateTime dt = _hosts_map.value(peer);

        if(dt.msecsTo(current_dt) < (8 * 60 * 60 * 1000)) { continue; }
        
        QHostAddress peer_host( (quint32)(peer & 0xFFFFFFFFLL) );
        quint16 peer_port = (quint16)(peer >> 32);

        message( QString("Удаление пира %1:%2").arg(peer_host.toString()).arg(peer_port) );        
        _hosts_map.remove(peer);
        }
    }
