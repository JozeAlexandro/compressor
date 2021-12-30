/** ****************************************************************************
 * \brief Исходные коды для алгоритма
 *
 * \file cAlgorithmRLE.cpp
 * ****************************************************************************/

#include "algorithm/cAlgorithmHaffman/h/cAlgorithmHaffman.h" /// Заголовок модуля
#include <queue> /// Очереди
#include <map> /// Отображение

/** ****************************************************************************
 * Определения публичной части класса
 * ****************************************************************************/

std::string cAlgorithmHaffman::compress( const std::string & oldData)
{
    /// Создание дерева Хаффмана
    sNode *root = buildTree( std::move( oldData ) );
    /// Создание отображения символ - код
    mapSym2VtorBit_t sym2code = encodeTree( root );
    /// Очистка кучи
    deleteTree( root );

    /// Результат сжатия
    std::string result;

    /// Кодовая таблица
    const std::string codeTable( encodeMap( sym2code ) );
        result += codeTable;

    /// Код Хаффмана исходных данных каждый элемент bufferHfmn 0 или 1
    std::string bufferHfmn;
    for( const auto &sym : oldData )
        bufferHfmn += sym2code[ sym ];

    /// Результирующее количество бит для записи кода
    const uint64_t totalDataBitCount = bufferHfmn.size();

    /// Уплотнение и запись данных
    result += ( sealCollection( bufferHfmn ) );

    /// Запись количества бит даннх в сжатый файл.
    writeSize2StartOfClctn( result, 0, totalDataBitCount );

    return result;
}


std::string cAlgorithmHaffman::decompress( const std::string & oldData )
{
    /// Чтение таблицы символов и получение сдвига к данным
   const std::tuple< mapSym2VtorBit_t, size_t > codeAndTblSize =
            readCodeTableFromCmprData( oldData );

    const size_t serviceShift =
            std::get< 1 >( codeAndTblSize ) + SHIFT_TABLE_SIZE + SHIFT_BITS_COUNT;

    const mapSym2VtorBit_t &sym2code = std::get< 0 >( codeAndTblSize );

    /// Получение количества значимых бит
    const uint64_t significantBitCount = readSizeFromStartOfClctn< uint64_t >( oldData );

    /// Заполнение данных
    return fillDecomrData( oldData, serviceShift, sym2code, significantBitCount );
}



/** ****************************************************************************
 * Определения приватной части класса
 * ****************************************************************************/

cAlgorithmHaffman::sNode *cAlgorithmHaffman::buildTree( const std::string_view data )
{
    /// Подсчет частот появления символов в тексте
    std::map< symbol_t, size_t > freq;
    for( auto &&sym : data )
        freq[ sym ]++;

    /// Функция для сравнения частот узлов для очереди с приоритетом
    auto comparer = []( sNode* l, sNode* r ){ return l->mFreq > r->mFreq; };

    /// Создаем очередь с приоритетом для заполнения дерева
    std::priority_queue< sNode *,
                         std::deque< sNode* >,
                         decltype( comparer ) > pq( comparer );

    /// Созддание узлов
    for( const auto &pair : freq )
        pq.push( new sNode( pair.first, pair.second ) );

    /// Пока в очереди более 1 элемента
    while( pq.size() > 1 )
    {
        /// Получение 2х узлов с наивысшим приоритетом
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();

        /// Создание нового узла, частота которого равна сумме частот двух
        /// листьев
        const size_t sumFreq = left->mFreq + right->mFreq;
        pq.push( new sNode( '\0', sumFreq, left, right ) );
    }

    /// Корень дерева
    return pq.top();
}


cAlgorithmHaffman::mapSym2VtorBit_t cAlgorithmHaffman::encodeTree( sNode *root, const std::string &code )
{
    if( !root )
        return mapSym2VtorBit_t();

    /// Отображение
    static mapSym2VtorBit_t sym2HfmnCode;

    /// Очистка отображения, если метод был вызван не рекурсивно, значит это
    /// первый вызов и нужно новое отображение
    if( code.empty() )
        sym2HfmnCode.clear();

    /// Поиск листьев
    if ( !root->mpLeft && !root->mpRight )
    {
        sym2HfmnCode[ root->mSym ] = code;
    }

    /// Дозапись кода
    std::string leftCode = code.data();
    leftCode += '0';

    std::string rigthCode = code.data();
    rigthCode += '1';

    encodeTree( root->mpLeft, leftCode );
    encodeTree( root->mpRight, rigthCode );

    return sym2HfmnCode;
}


std::string cAlgorithmHaffman::encodeMap( const mapSym2VtorBit_t &sym2map )
{
    /// Для результирующей таблицы
    std::string result;

    for( const auto &symAndCode : sym2map )
    {
        /// Размер кода в битах
        result += static_cast< char >( symAndCode.second.size() );
        /// Символ
        result += symAndCode.first;

        /// Код Хаффмана символа
        result += sealCollection( symAndCode.second );
    }

    /// Запись размера таблицы в байтах
    const uint32_t mapSize( result.size() );
    writeSize2StartOfClctn( result, 0, mapSize );

    return result;
}

std::tuple< cAlgorithmHaffman::mapSym2VtorBit_t,
            size_t > cAlgorithmHaffman::readCodeTableFromCmprData( const std::string_view code )
{
    mapSym2VtorBit_t sym2code;

    /// Получение размера таблицы.
    uint32_t tableSize = readSizeFromStartOfClctn< uint32_t >( code, SHIFT_BITS_COUNT );

    for( size_t row = 0; row < tableSize; )
    {
        /// Сдвиг для получения количество бит для текущего символа
        size_t shiftBitCount = SHIFT_IN_TABLE_BIT_COUNT + SHIFT_TABLE_SIZE + SHIFT_BITS_COUNT + row;
        /// Сдвиг для получения текущего символа
        size_t shiftSym = SHIFT_IN_TABLE_SYM + SHIFT_TABLE_SIZE + SHIFT_BITS_COUNT + row;
        /// Сдвиг для получения кода символа
        size_t shiftCode = SHIFT_IN_TABLE_CODE + SHIFT_TABLE_SIZE + SHIFT_BITS_COUNT + row;

        std::string codeSym;

        /// Количество целых байт в коде символа
        size_t fullByteInCode = code[ shiftBitCount ] / BIT_2_SYM;
        /// Количество используемых старших бит в крайнем незаполненном(если есть) байте
        const size_t remaindBitInLastByteCode = code[ shiftBitCount ] % BIT_2_SYM;

        for( size_t bt = 0, shift = BIT_2_SYM - 1;
             bt < fullByteInCode * BIT_2_SYM;
             ++bt, --shift )
        {
            codeSym += ( ( code[ shiftCode ] & ( 1 << shift ) ) ? '1' : '0' );

            if( 0 == shift )
                shift = BIT_2_SYM;
        }

        for( size_t shift = BIT_2_SYM - 1;
             shift > ( BIT_2_SYM - 1 - remaindBitInLastByteCode );
             shift-- )
        {
            codeSym += ( code[ shiftCode + fullByteInCode ] & ( 1 << shift ) ? '1' : '0' );
        }

        sym2code[ code[ shiftSym ] ] = codeSym;

        /// Размер + символ
        row += 2;
        /// Количество целых байт
        row += fullByteInCode;
        /// Неполноценный байт, если такой есть
        if( remaindBitInLastByteCode )
            row += 1;
    }

    return std::make_tuple( sym2code, tableSize );
}


std::string cAlgorithmHaffman::fillDecomrData(const std::string_view oldData,
                                               size_t servDataShift,
                                               const cAlgorithmHaffman::mapSym2VtorBit_t &sym2code,
                                               uint64_t significantBitCount )
{
    /// Для хранения результата
    std::string result;
    //result.reserve( significantBitCount / BIT_2_SYM );

    /// Отображение кода на символ. Было принято решение создать его, т.к.
    /// создать и инициализировать такое в общем случае быстрее, чем каждый раз
    /// проходить по всем значениям sym2code в поисках нужного
    using mapVtorBit_t2sym = std::map< std::string, char >;
    mapVtorBit_t2sym code2sym;
    for( const auto &treeEl : sym2code  )
        code2sym[ treeEl.second ] = treeEl.first;

    /// Текущий счетчик бита
    uint64_t bitCounter = 0;

    std::string buff;
    auto addBit = [ &buff, &significantBitCount, &bitCounter ]( char val )
    {
        if( bitCounter < significantBitCount )
        {
            buff.push_back(val);
            bitCounter++;
        }

    };

    auto checkAndAdd = [ sym2code, &buff, &result, &code2sym ]()
    {
        if( code2sym.end() != code2sym.find( buff ) )
        {
            result += code2sym[ buff ];
            buff.clear();
        }
    };


    for( size_t it = servDataShift;
         it < oldData.size();
         it++ )
    {
        for( int shift = BIT_2_SYM - 1; shift >= 0; shift-- )
        {
            /// Добавление очередной 1 или 0
            addBit( oldData[ it ] & ( 1 << shift ) ? '1' : '0' );
            checkAndAdd();
        }
        checkAndAdd();
    }

    return result;
}


void cAlgorithmHaffman::deleteTree( sNode *root ) const
{
    if( !root )
        return;

    if ( !root->mpLeft && !root->mpRight )
    {
        delete root;
        return;
    }

    deleteTree( root->mpLeft );
    deleteTree( root->mpRight );
}

std::string cAlgorithmHaffman::sealCollection( const std::string_view collection ) const
{
    /// Полное количество символов и остаток значимых бит в незаполненном,
    /// если такой есть символе
    const size_t fullSym = collection.size() / BIT_2_SYM;
    const size_t remaindBit = collection.size() % BIT_2_SYM;

    /// Текущий символ для записи. В данном случае символ - это код из 0 и 1
    /// размером 8 BIT_2_SYM
    uint8_t currSym = 0;

    /// Для результата
    std::string result;

    /// Кодирование полных байт
    for( size_t bitCount = 0, shift = BIT_2_SYM - 1;
         bitCount < fullSym * BIT_2_SYM;
         ++bitCount, --shift )
    {
        /// Уплотнение нулей и единиц в размер символа
        currSym |= ( collection[ bitCount ] == '1' ) ? ( 1 << shift ) : ( 0 << shift );
        /// Если буфер уже заполнен
        if( 0 == shift )
        {
            shift = BIT_2_SYM;
            result += ( currSym );
            currSym = 0;
        }
    }

    //// Кодирование неполноценного байта
    for( size_t shift = BIT_2_SYM - 1, bitCount = 0;
         shift > ( BIT_2_SYM - 1 - remaindBit );
         shift--, bitCount++ )
    {
        currSym |= ( collection[ fullSym * BIT_2_SYM + bitCount ] == '1' ) ? ( 1 << shift ) : ( 0 << shift );
    }
    if( remaindBit )
        result += currSym;

    return result;
}
