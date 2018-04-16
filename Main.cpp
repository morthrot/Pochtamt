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

#include <QtCore>
#include <QtNetwork>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#endif

#include "Pochtamt.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
#ifdef AM_USING_QT4
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );
#endif
    srand(QDateTime::currentDateTime().toMSecsSinceEpoch());
    
    quint16 port = (argc > 1 ? atoi(argv[1]) : 6888);
    QString service = (argc > 2 ? argv[2] : "Banderol");
    
    Pochtamt pochtamt("service");
    
    bool listen_ok = pochtamt.bind(port);
    if(listen_ok == false) { return EXIT_FAILURE; }

#ifdef Q_OS_UNIX
    if(getuid() == 0) {
        struct passwd * entry = getpwnam("_pochtamt");
        if(entry == NULL) { perror("getpwnam"); return EXIT_FAILURE; } 
        
        if(chroot("/var/pochtamt") != 0) { perror("chroot"); return EXIT_FAILURE; }
        if(chdir("/") != 0) { perror("chdir"); return EXIT_FAILURE; }
        
        if(setgid(entry->pw_gid) != 0) { perror("setgid"); return EXIT_FAILURE; }
        if(setuid(entry->pw_uid) != 0) { perror("setuid"); return EXIT_FAILURE; }
        }
    
#ifdef USE_PLEDGE
    if(pledge("stdio inet",NULL) != 0) { perror("pledge"); return EXIT_FAILURE; }
#endif
#endif

    return app.exec();
    return EXIT_SUCCESS;    
    }
