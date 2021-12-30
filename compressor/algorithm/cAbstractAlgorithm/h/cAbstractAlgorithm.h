/** ****************************************************************************
 * \file cAbstractAlgorithm.h
 * ****************************************************************************/

/** ****************************************************************************
 * \defgroup Algorithm Алгоритмы
 *
 * \brief Предназначен для реализации алгоритмов сжатия.
 *
 * \details Является основным функциональным модулем проекта.
 * Состоит из абстрактного интерфейса алгоритмов - \ref AlgorithmAbstract
 * и его реализаций: \ref AlgorithmRLE, \ref AlgorithmHaffman
 *
 * ****************************************************************************/

/** ****************************************************************************
 * \defgroup AlgorithmAbstract Интерфейс алгоритмов
 * @{
 * \ingroup Algorithm
 *
 * \brief Модуль, содержащий интерфейс для реализаций алгоритмов сжатия.
 *
 * \details Реализован с поиощью класса \ref cAbstractAlgorithm
 * ****************************************************************************/


#ifndef CABSTRACTALGORITHM_H
#define CABSTRACTALGORITHM_H

#include <fstream> /// Для работы с файлами
#include <vector> /// Вектор
#include <common.h> /// Общие константы
#include <string>

/// \brief Абстрактный класс алгоритмов сжатия данных
/// \class cAbstractAlgorithm
class cAbstractAlgorithm
{
public:
    /// \brief Интерфейс для сжатия данных
    /// \param [in] oldData Исходные данные для сжатия
    /// \return Сжатые в соответсвии с алгоритмом данные
    virtual std::string compress( const std::string & oldData ) = 0;

    /// \brief Интерфейс для распаковки данных
    /// \param [in] oldData Исходные данные для распаковки
    /// \return Распакованные в соответсвии с алгоритмом данные
    virtual std::string decompress( const std::string & oldData ) = 0;

    /// \brief Получить постфикс
    /// \return Строка-расширение для упакованных данных
    inline virtual std::string getPostfix( void ) const = 0;
};

/// @}

#endif // CABSTRACTALGORITHM_H
