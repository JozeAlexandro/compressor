/** ****************************************************************************
 * \brief Исходные коды для алгоритма RLE
 *
 * \file cAlgorithmRLE.cpp
 * ****************************************************************************/

#include "algorithm/cAlgorithmRLE/h/cAlgorithmRLE.h" /// Заголовок класса

/** ****************************************************************************
 * Определения публичной части класса
 * ****************************************************************************/

std::string cAlgorithmRLE::compress( const std::string & oldData )
{
    /// Для результата
    std::string result;

    /// Переменная для хранения "предыдущего" значения последовательсности
    char prevVal = oldData[ 0 ];

    /// С единицы, т.к. будем сравнивать с предыдущим
    for( size_t curIndex = 1; curIndex <= oldData.size(); ++curIndex )
    {
        /// Текущий тип последовательности (одиночный или цепочка)
        eTypeOfSequence curType = oldData[ curIndex ] == prevVal ? SEQ_TYPE_SET
                                                                 : SEQ_TYPE_SINGLE;

        /// Счетчик. Количество одинаковых при SEQ_TYPE_SET
        /// или количество подряд идущих разных при SEQ_TYPE_SINGLE
        size_t counter = 0;
        if( SEQ_TYPE_SET == curType )
        {
            /// Раз совпало - уже 2 (предыдущий + текущий)
            counter = 2;

            /// Считаем
            while( oldData[ ++curIndex ] == prevVal &&
                   counter < MAX_SIZE_SET &&
                   curIndex < oldData.size() )
            {
                counter++;
            }

            /// Добавляем в результат
            result += getServiceByte( curType, counter );
            result += prevVal;

            /// Обновление "предыдущего" значения
            prevVal = oldData[ curIndex ];
        }
        else
        {
            std::string buffer;

            /// Считаем
            size_t startIndex = curIndex;
            while( oldData[ curIndex ] != oldData[ curIndex - 1] &&
                   curIndex <= oldData.size() )
            {
                prevVal = oldData[ curIndex - 1 ];
                buffer += prevVal;
                curIndex++;
            }
            counter = curIndex - startIndex;

            /// Добавляем в результат
            while( buffer.size() > MAX_SIZE_SINGLE )
            {
                result += ( getServiceByte( curType, MAX_SIZE_SINGLE ) );
                std::move( buffer.begin(),
                           buffer.begin() + MAX_SIZE_SINGLE,
                           std::back_inserter( result ) );
                buffer.erase( buffer.begin(), buffer.begin() + MAX_SIZE_SINGLE );
            }

            result += getServiceByte( curType, buffer.size() );
            result += buffer;

            /// Обновление предыдущего значения и индекса
            prevVal = oldData[ curIndex ];
            curIndex--;
        }
    }

    return result;
}

std::string cAlgorithmRLE::decompress( const std::string & oldData )
{
    std::string decomprData;
    size_t index = 0;
    while( index < oldData.size() )
    {
        sServiceByteInfo infoServByte = readServiceByte( oldData.data(), index );

        if( SEQ_TYPE_SET == infoServByte.mType )
        {
            while( infoServByte.mCount-- )
                decomprData += ( oldData[ index + 1 ] );

            index += 2;
        }
        else
        {
            while( infoServByte.mCount -- )
                decomprData += ( oldData[ ++index ] );

            index += 1;
        }
    }

    return decomprData;
}


/** ****************************************************************************
 * Определения приватной части класса
 * ****************************************************************************/

cAlgorithmRLE::sServiceByteInfo cAlgorithmRLE::readServiceByte( const char data[], size_t index ) noexcept
{
    sServiceByteInfo info;
    symbol_t dataByte = data[ index ];
    info.mType = dataByte & SEQ_TYPE_SET ? SEQ_TYPE_SET
                                         : SEQ_TYPE_SINGLE;

    info.mCount = dataByte & ( ~SEQ_TYPE_SET );

    info.mCount += SEQ_TYPE_SET == info.mType ? COUNT_INCREMENT_SET
                                              : COUNT_INCREMENT_SINGLE;
    return info;
}

