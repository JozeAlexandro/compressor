/** ****************************************************************************
 * \file main.cpp
 *
 * \mainpage Титульная страница
 * \details Программа по сжатию и распаковке данных. Реализованы алгоримтмы:
 * \ref AlgorithmRLE и \ref AlgorithmHaffman.
 *
 * Программа состоит из нескольких функциональных модулей:
 * \ref WindowGUI - Графический интерфейс,
 * \ref FileWorker - Работа с файлами,
 * \ref Algorithm - Алгоритмы сжатия,
 * а также дополнительного модуля общих констант программы \ref Common
 * ****************************************************************************/

#include <windowGUI/h/windowGUI.h> /// GUI
#include <QApplication> /// Qt управление GUI программами


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    windowGUI wg;
    return wg.exec();
}

