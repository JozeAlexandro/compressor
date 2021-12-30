/** ****************************************************************************
 * \brief Исходные коды класса для работы с файлами
 *
 * \file cFileWorker.сpp
 * ****************************************************************************/

#include "cFileWorker/h/cFileWorker.h" /// Заголовок класса
#include <tuple> /// Кортежи
#include <QString> /// Строки Qt
#include <QFileInfo> /// Qt класс информации о файле

/** ****************************************************************************
 * Определение API
 * ****************************************************************************/

cFileWorker::~cFileWorker( void )
{
    /// Закрытие ранее открытых потоков
    mFile2Read.close();
    mFile2Write.close();
}

eErrStatus cFileWorker::updateWriteFile( std::string_view fileWritePath )
{
    mFile2Write.close();
    mFile2WritePath = fileWritePath;
    mFile2Write.open( fileWritePath.data(),
                      std::ios_base::binary | std::ios_base::out | std::ios_base::trunc );

    return mFile2Write.is_open() ? ERR_STATUS_SUCCESS : ERR_STATUS_BAD_FILE_OPEN;
}

eErrStatus cFileWorker::updateReadFile( std::string_view fileReadPath )
{
    mFile2Read.close();
    mFile2ReadPath = fileReadPath;
    mFile2Read.open( fileReadPath.data(),
                     std::ios_base::binary | std::ios_base::in );
    return mFile2Read.is_open() ? ERR_STATUS_SUCCESS : ERR_STATUS_BAD_FILE_OPEN;
}

std::tuple< std::string, eErrStatus > cFileWorker::applyAlgorithm( cAbstractAlgorithm &algorithm, eTypeOfActions action )
{
    if( !mFile2Read.is_open() || !mFile2Write.is_open() )
        return std::make_tuple( "", ERR_STATUS_BAD_FILE_OPEN );

    /// В случае декомпресси проверяем на правильное имя архива
    if( ACT_TYPE_DECOMPR == action && !checkPostfix( algorithm ) )
        return std::make_tuple( "", ERR_STATUS_BAD_POSTFIX );

    /// Читаем данные из файла для чтения
    auto [ buffer, readStatus ] = readData();
    if( ERR_STATUS_SUCCESS != readStatus )
        return std::make_tuple( "", ERR_STATUS_EMPTY_SRC_FILE );

    /// Выполнение сжатия/распаковки с выбранным алгоритмом
    std::string resultData( action == ACT_TYPE_COMPR ? algorithm.compress( buffer )
                                                     : algorithm.decompress( buffer ) );

    /// Если результат пуст
    if( resultData.empty() )
        return std::make_tuple( "", ERR_STATUS_BAD_ALG );

    /// Запись в новый файл
    mFile2Write.write( reinterpret_cast<char*>( resultData.data() ), resultData.size() );
    mFile2Write.flush();

    /// Добавление постфикса/префикса к имени файла
    std::string newName;
    if( action == ACT_TYPE_COMPR )
    {
        /// В случае сжатия - добавление постфикса алгоритма
        newName = mFile2ReadPath + algorithm.getPostfix();
    }
    else
    {
        newName = mFile2ReadPath;
        /// Удаление постфикса алгоритма
        newName.erase( newName.rfind( algorithm.getPostfix() ) );
        /// Добавление '_' перед именем файла
        newName.insert( newName.rfind( '/' ) + 1, 1, '_' );
    }

    mFile2Write.close();
    QFile::rename( QString( mFile2WritePath.data() ), QString::fromStdString( newName ) );

    return std::make_tuple( newName, ERR_STATUS_SUCCESS );
}

/** ****************************************************************************
 * Определение приватной части
 * ****************************************************************************/

bool cFileWorker::checkPostfix( const cAbstractAlgorithm &algorithm ) const
{
    /// Поиск последнего вхождения точки
    const auto dotPos = mFile2ReadPath.find_last_of('.');
    if( std::string::npos == dotPos )
        return false;

    /// Получение последнего постфикса
    const auto curPostfix = mFile2ReadPath.substr( dotPos );
    if( algorithm.getPostfix() != curPostfix )
        return false;

    return true;
}


std::tuple< std::string, eErrStatus > cFileWorker::readData( void )
{

    QFileInfo fInfo( QString( mFile2ReadPath.data() ) );
    const size_t FILE_READ_SIZE =fInfo.size();

    if( 0 == FILE_READ_SIZE )
        return std::make_tuple( std::string(), ERR_STATUS_EMPTY_SRC_FILE );

    symbol_t *rawBuffer = new symbol_t[ FILE_READ_SIZE ];
    mFile2Read.seekg( 0 );
    mFile2Read.read( reinterpret_cast<char*>( rawBuffer ), FILE_READ_SIZE );

    std::string buffer;
    buffer.reserve( FILE_READ_SIZE );

    for( size_t i = 0; i < FILE_READ_SIZE; i++ )
        buffer += ( rawBuffer[ i ] );

    delete[] rawBuffer;

    return std::make_tuple( std::move( buffer ), ERR_STATUS_SUCCESS );
}

