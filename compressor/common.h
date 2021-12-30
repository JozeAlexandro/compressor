/** ****************************************************************************
 * \file common.h
 *
 * \defgroup Common Общие константы
 * @{
 *
 * \details Модуль, определяющий общие константы для работы программы
 * ****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <vector> ///< Вектор

/// \brief Типы используемых алгоритмов
/// \enum eTypeOfComprAlgorithm
enum eTypeOfComprAlgorithm
{
    ALG_TYPE_RLE = 0, ///< Алгоритм RLE
    ALG_TYPE_HFMN ///< Алгоритм Хаффмана
};

/// \brief Возможные действия пользователя
/// \enum eTypeOfActions
enum eTypeOfActions
{
    ACT_TYPE_COMPR, ///< Сжатие
    ACT_TYPE_DECOMPR ///< Распаковка
};

/// \brief Ошибки в программе
/// \enum eErrStatus
enum eErrStatus
{
    ERR_STATUS_SUCCESS, ///< Без ошибок
    ERR_STATUS_BAD_ALG, ///< Ошибка при выполнении алгоритма
    ERR_STATUS_BAD_FILE_OPEN, ///< Ошибка при открытии файла
    ERR_STATUS_BAD_POSTFIX, ///< Ошибка расширения файла для декомпрессии
    ERR_STATUS_EMPTY_SRC_FILE ///< Ошибка выбора пустого файла для сжатия
};

/// \brief Псевдоним для считываемого байта
/// \typedef symbol_t
using symbol_t = unsigned char;

/// @}

#endif // COMMON_H
