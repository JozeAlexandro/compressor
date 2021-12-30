/** ****************************************************************************
 * \file cAlgorithmHaffman.h
 *
 * \defgroup AlgorithmHaffman Алгоритм Хаффмана
 * @{
 *
 * \ingroup Algorithm
 *
 * \brief Модуль, реализующий сжатие данных алгоритмом Хаффмана
 *
 * \details Кодирование Хаффмана. Основа - бинарные деревья Хаффмана.
 * Каждый узел представляет собой сивол, его частоту и ссылку на дочерние
 * элементы.
 *
 * Для создания дерева используется очередь с приоритетом. Узел с минимальной
 * частотой имеет наивысший приоритет.
 * Шаги построения дерева:
 * - Создать узел для каждого символа и добавить его в очередь с приоритетом
 * - Пока в очереди > 1 листа
 * -- Удалить 2 узла с наивысшим приоритетом (наименьшей частотой) из очереди
 * -- Создать новый узел дерева, где вышеудаленные узлы будут наследниками, а
 * частота появления = сумме частот наследников
 * -- Добавить созданный узел в очередь
 * - Последний оставшийся элемент - корень дерева
 *
 * После создания дерева создаем отображение элемента на его код. Все элементы
 * будут находиться именно в листьях дерева. Для получения кода элемента
 * проходим по дереву от корня до элемента. Каждый спуск по левой ветви -
 * +0 к коду, каждый спуск по правой ветви +1 к коду.
 *
 * Для успешного декодирования сжатых данных алгоритму требуется получить
 * дерево из закодированных данных. Без потерь по памяти сделать это невозможно.
 * Решение: добавлять служебную информацию в начало любых закодированных
 * данных. Выделено 4 байта под размер таблицы. Т.е. закодированные данные:
 * [ < Размер таблицы (4 байта) > < Непосредственно таблица, размером,
 * определенном в предыдущем блоке > < Данные > ].
 *
 * Помимо этого существует еще одна проблема: физически нет возможности
 * записывать информацию в файл меньшую, чем 1 байт, но кодирование по Хаффману
 * подразумевает, что количество бит в кодах символов не обязано быть кратным
 * байту. Пояснение, предположим, исходный файл состоит из "abc". После построения
 * дерева получается, что код символа a = 0, b = 11, c = 10. Тогда, сжатые данные
 * (без учета служебной информации) будут: 0 1110. Это меньше байта и запишется
 * в исходные данные, как 0000 1110 или 0111 0000, в зависимости от реализации.
 * В любом случае, декодирование будет некорректным. В первом случае:
 * результат = aaaabc, во втором: abcaaa. Для решения этого принято хранить
 * количество значащих бит в служебном месте сжатого файла. Для этого выделено
 * 8 байт.
 *
 * Так как для декодирования данных необходимо считывать информацию побитно
 * и после каждого считанного бита сравнивать результат буфера со всеми кодами
 * в поисках символа - декомпрессия методом Хаффмана не самое быстрое
 * предприятие. Поэтому на файлах, имеющих большой размер алгоритм выполняется
 * довольно долго.
 *
 * С учетом того, что алгоритм добавляет 8 + 4 + X байт информации в результат
 * сжатия (8 - Количество бит, 4 - размер таблицы, X - таблица) большого смысла
 * от сжатия файлов малых размеров нет.
 *
 * Реализован с поиощью класса \ref cAlgorithmHaffman
 * ****************************************************************************/

#ifndef CALGORITHMHAFFMAN_H
#define CALGORITHMHAFFMAN_H

#include "algorithm/cAbstractAlgorithm/h/cAbstractAlgorithm.h" /// Абстрактный
                                                               /// класс для алгоритмов
#include <map> /// Отображение

/// \brief Класс реализующий алгоритм Хаффмана
/// \class cAlgorithmHaffman
class cAlgorithmHaffman final : public cAbstractAlgorithm
{
public:
    /// \brief Сжатие данных
    /// \param [in] oldData Исходные данные для сжатия
    /// \return Сжатые в соответсвии с кодированием Хаффмана
    virtual std::string compress( const std::string & oldData ) override;

    /// \brief Распаковка данных
    /// \param [in] oldData Исходные данные для распаковки
    /// \return Распакованные данные в кодированием Хаффмана
    virtual std::string decompress( const std::string & oldData ) override;

    /// \brief Получить постфикс
    /// \return Строка-расширение для упакованных данных
    inline virtual std::string getPostfix( void ) const override { return ".cmprHaffman"; }

private:
    /// \brief Структура, описывающая узел дерева Хаффмана
    /// \struct sNode
    struct sNode
    {
    public:
        /// \brief Конструктор
        /// \param [in] sym Символ
        /// \param [in] freq Частота символа в исходном тексте
        /// \param [in] left Левый лист
        /// \param [in] right Правый лист
        sNode( char sym,
               size_t freq,
               sNode *left = nullptr,
               sNode *right = nullptr ) :
            mSym( sym ),
            mFreq( freq ),
            mpLeft( left ),
            mpRight( right ) {}

        /// \brief Символ
        char mSym = '0';
        /// \brief Частота символа в исходном тексте
        size_t mFreq = 0;
        /// \brief Левый лист
        sNode *mpLeft = nullptr;
        /// \brief Правый лист
        sNode *mpRight = nullptr;
    };

    /// \brief Псевдоним для отображения символа на код
    /// \typedef mapSym2VtorBit_t
    using mapSym2VtorBit_t = std::map< char, std::string >;

    /// \brief Количество бит в символе
    constexpr static size_t BIT_2_SYM = 8;

    /// \brief Количество байт в коде, занимаемых размером кодовой таблицы
    constexpr static size_t SHIFT_TABLE_SIZE = 4;
    /// \brief Количество байт в коде, занимаемых количеством значимых бит
    constexpr static size_t SHIFT_BITS_COUNT = 8;

    /// \brief Сдвиг в строке кодовой таблицы для получения количества бит для кода символа
    constexpr static size_t SHIFT_IN_TABLE_BIT_COUNT = 0;
    /// \brief Сдвиг в строке кодовой таблицы для получения символа
    constexpr static size_t SHIFT_IN_TABLE_SYM = 1;
    /// \brief Сдвиг в строке кодовой таблицы для получения кода Хаффмана символа
    constexpr static size_t SHIFT_IN_TABLE_CODE = 2;

    /// \brief Создать дерево
    /// \warning Данные выделяются в куче! Для освобождения - deleteTree
    /// \param [in] data Исходные данные
    /// \return Корень дерева
    sNode* buildTree(const std::string_view oldData );

    /// \brief Получение отображения символов на их код Хаффмана
    ///
    /// \details Для каждого листа дерева, корнем которого является
    /// root создаст запись символ-код. Каждая левая ветвь = code + 0,
    /// каждая правая = code + 1
    ///
    /// \param [in] root Корень дерева Хаффмана
    /// \param [in] code Код символа, если тот является листом
    ///
    /// \return Отображение символов дерева на их код
    mapSym2VtorBit_t encodeTree(sNode* root, const std::string &code = std::string() );

    /// \brief Создать таблицу кодов для добавления к сжатому файлу
    /// \param [in] sym2map Отображение символов на код
    /// \return Таблица для записи к сжатым данным
    std::string encodeMap( const mapSym2VtorBit_t &sym2map );

    /// \brief Чтение кодовой таблицы из сжатых данных
    /// \param [in] code Сжатые данные
    /// \return Кортеж из отображения символа на код и размера таблицы
    std::tuple< mapSym2VtorBit_t, size_t > readCodeTableFromCmprData(const std::string_view code );

    /// \brief Заполнить выходной контейнер для декомпрессии
    ///
    /// \param [in] oldData Сжатые данные
    /// \param [in] servDataShift Сдвиг до данных (минуя сервисные байты)
    /// \param [in] sym2code Отображение символа на код Хаффмана
    /// \param [in] significantBitCount Количество значимых бит в данных
    ///
    /// \return Контейнер для разжатых данных
    std::string fillDecomrData( const std::string_view oldData,
                                size_t servDataShift,
                                const mapSym2VtorBit_t &sym2code,
                                uint64_t significantBitCount );

    /// \brief Очистить динамически выделенную память под дерево
    /// \param [in] root Корень дерева
    void deleteTree( sNode *root ) const;

    /// \brief Уплотнение коллекции символов
    /// \param [in] collection Коллекция для уплотнения
    /// \return Уплотненные данные \todo rename
    std::string sealCollection(const std::string_view collection) const;

    /// \brief Запись в place2Write коллекции clctn размера size побайтно
    ///
    /// \param [in] clctn Коллекция
    /// \param [in] place2Write Место для записи
    /// \param [in] size Размер для записи
    template< typename T >
    inline void writeSize2StartOfClctn( std::string &clctn,
                                        size_t index,
                                        T size ) const
    {
        constexpr size_t BYTE_IN_SIZE_WD( sizeof( T ) );
        for( size_t bt = 0; bt < BYTE_IN_SIZE_WD; ++bt )
            clctn.insert( index, 1,
                        ( size & ( 0xFF << ( BIT_2_SYM * bt ) ) ) >> ( BIT_2_SYM * bt ) );
    }

    /// \brief Чтение первых sizeof(T) байт коллекции clctn со сдвигом shiftFromStart
    ///
    /// \param [in] clctn Коллекция
    /// \param [in] shiftFromStart Сдвиг
    ///
    /// \return Размер
    template< typename T >
    inline T readSizeFromStartOfClctn( const std::string_view clctn, size_t shiftFromStart = 0 ) const
    {
        constexpr size_t BYTE_IN_SIZE_WD( sizeof( T ) );

        T result = 0;
        for( size_t bt = 0, shift = BYTE_IN_SIZE_WD - 1;
             bt < BYTE_IN_SIZE_WD;
             ++bt, --shift )
        {
            result |= ( uint8_t( clctn[ bt + shiftFromStart ] ) << ( shift * BIT_2_SYM ) );
        }

        return result;
    }
};

/// @}

#endif // CALGORITHMHAFFMAN_H
