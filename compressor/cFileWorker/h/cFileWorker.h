/** ****************************************************************************
 * \file cFileWorker.h
 *
 * \defgroup FileWorker Обработчик файлов
 * @{
 *
 * \brief Модуль для работы с файлами
 *
 * \details Предоставляет возможности по открытию/закрытию/переименованию файлов
 * файловой системы ПК.
 * Выполняет алгоритмы из модуля \ref Algorithm c данными, считанными из
 * исходного файла и записывает результат выполнения алгоритма в новый файл.
 *
 * Также, при декомпрессии файла проверяет его постфикс. Постфикс файла -
 * это метка того алгоритма с помощью которого был сжат файл. Например,
 * изначальный файл text.txt сжат алгоритмом RLE. Результат сжатия будет
 * находиться в той же директории, что и файл text.txt, но с именем
 * text.txt.cmprRLE
 *
 * При проверке постфикса файла проверяется совпадение алгоритма и метода, с
 * которым был сжат файл. Декомпрессия применяется в том случае, если постфиксы
 * совпали.
 *
 * После применения разархивации результат будет находиться в той же директории,
 * что и сжатый файл, но с префиксом _. Например, файл text.txt.cmprRLE после
 * декомпрессии будет иметь имя _text.txt и будет идентичен по составу text.txt
 *
 * Реализован с поиощью класса \ref cFileWorker
 * ****************************************************************************/


#ifndef CFILEWORKER_H
#define CFILEWORKER_H

#include "common.h" /// Общие константы программы
#include "algorithm/cAbstractAlgorithm/h/cAbstractAlgorithm.h" /// Интерфейс алгоритмов
#include <fstream> /// Стандартная библоиотека работы с потоками
#include <QFile> /// Qt библиотека работы с фаловой системой

/// \brief Класс, реализующий работу с файлами
/// \class cFileWorker
class cFileWorker final
{
public:
    /// \brief Деструктор класса
    ~cFileWorker( void );

    /// \brief Обновить имя файла для записи результата
    /// \param [in] fileWeitePath Имя файла
    /// \return ERR_STATUS_SUCCESS в случае успеха, ERR_STATUS_BAD_FILE_OPEN
    /// в случае ошибки открытия файла
    eErrStatus updateWriteFile( std::string_view fileWritePath );

    /// \brief Обновить имя файла для чтения исходных данных
    /// \param [in] fileReadPath Имя файла
    /// \return ERR_STATUS_SUCCESS в случае успеха, ERR_STATUS_BAD_FILE_OPEN
    /// в случае ошибки открытия файла
    eErrStatus updateReadFile( std::string_view fileReadPath );

    /// \brief Применить алгоритм к выбранным файлам
    ///
    /// \param [in] algorithm Интерфейс выбранного алгоритма
    /// \param [in] action ACT_TYPE_COMPR - сжатие ACT_TYPE_DECOMPR - распаковка
    ///
    /// \return
    /// Строка - созданный файл;
    /// Статус:
    /// ERR_STATUS_SUCCESS - успех,
    /// ERR_STATUS_BAD_FILE_OPEN - в случае ошибки открытия фалов,
    /// ERR_STATUS_BAD_POSTFIX - при распаковке, если постфикс файла не совпадает
    /// с постфиксом выбранного алгоритма,
    /// ERR_STATUS_EMPTY_SRC_FILE - если исходный файл пуст,
    /// ERR_STATUS_BAD_ALG - если выполнение алгоритма привело к ошибке
    std::tuple<std::string, eErrStatus> applyAlgorithm( cAbstractAlgorithm &algorithm, eTypeOfActions action );

private:
    /// \brief Поток для записи в файл
    std::ofstream mFile2Write;
    /// \brief Поток для чтения файла
    std::ifstream mFile2Read;

    /// \brief Путь до файла записи
    std::string mFile2WritePath;
    /// \brief Путь до файла чтения
    std::string mFile2ReadPath;

    /// \brief Проерить имя архива на корректность
    ///
    /// \details В случае декомпрессии необходимо, чтобы расширение файла
    /// совпадало с тем, с которым работает конкретный алгоритм. Этот метод
    /// призван проверять именно это
    ///
    /// \param [in] algorithm Интерфейс алгоритма
    ///
    /// \return true - Имя корректно, иначе - false
    bool checkPostfix( const cAbstractAlgorithm &algorithm ) const;

    /// \brief Прочесть данные из файла для чтения
    /// \return Вектор прочитанных байт и статус выполнения метода
    std::tuple<std::string, eErrStatus> readData( void );
};

/// @}

#endif // CFILEWORKER_H
