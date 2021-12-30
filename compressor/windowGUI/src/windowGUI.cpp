/** ****************************************************************************
 * \brief Исходные коды класса GUI
 *
 * \file windowGUI.сpp
 * ****************************************************************************/


#include "windowGUI/h/windowGUI.h" /// Заголовок класса
#include "ui_windowGUI.h" /// Файл, генерируемый из формы windowGUI.ui
#include <QFileDialog> /// Диалоговое окно выбора файла
#include <thread> /// Поддержка многопоточности
#include "algorithm/cAlgorithmRLE/h/cAlgorithmRLE.h" /// Алгоритм RLE
#include "algorithm/cAlgorithmHaffman/h/cAlgorithmHaffman.h" /// Алгоритм Хаффмана

/** ****************************************************************************
 * Определение API
 * ****************************************************************************/

windowGUI::windowGUI( QWidget *parent ) :
    QDialog( parent ),
    mpUI( new Ui::windowGUI ),
    mJouarnalModel( this ),
    mMovieLoading(":/gif/gif/loading.gif") /// Путь до гифки, добавленной в
                                           /// ресурсы прокта
{
    /// Установка GUI
    mpUI->setupUi( this );

    /// Инициализация отображения типа алгоритма к объекту алгоритма
    fillCtrtAlgorithms();

    /// Установка модели логера для представления на GUI
    mpUI->journal->setModel( &mJouarnalModel );

    /// Установка фиксированного размера окна программы
    this->setFixedSize( this->size() );

    /// Инициализация gif
    initGifLoading();

    /// Запрет действий до выбора файла
    mpUI->gbAlg->setEnabled( false );

    /// Сигнал-слотовое соединение для сокрытия бесконечной загрузки
    qRegisterMetaType<QVector<int>>("QVector<int>");
    connect( this, &windowGUI::signalHideGif,
             &this->mlblLoading, &QLabel::hide );
}

windowGUI::~windowGUI( void )
{
    delete mpUI;
}

/** ****************************************************************************
 * Определение приватной части
 * ****************************************************************************/

void windowGUI::fillCtrtAlgorithms( void )
{
    mmAlgorithms.insert( std::make_pair( ALG_TYPE_RLE,
                                         std::make_unique< cAlgorithmRLE >() ) );
    mmAlgorithms.insert( std::make_pair( ALG_TYPE_HFMN,
                                         std::make_unique< cAlgorithmHaffman >() ) );
}

void windowGUI::threadEnding( eErrStatus status , const std::string &newName )
{
    switch( status )
    {
    case ERR_STATUS_SUCCESS:
        mJouarnalModel.insertString( "Выполнено! Результат в файле:" );
        mJouarnalModel.insertString( newName.c_str() );
        break;

    case ERR_STATUS_BAD_FILE_OPEN:
        mJouarnalModel.insertString( "Ошибка при открытии файлов!" );
        break;

    case ERR_STATUS_BAD_POSTFIX:
        mJouarnalModel.insertString( "Расширение файла не является допустимым для данного алгоритма!" );
        break;

    case ERR_STATUS_EMPTY_SRC_FILE:
        mJouarnalModel.insertString( "Выбран пустой файл!" );
        break;

    case ERR_STATUS_BAD_ALG:
        mJouarnalModel.insertString( "Ошибка при выполнении алгоритма!" );
        break;
    }

    /// Скрыть гифку
    emit signalHideGif();
}

void windowGUI::initGifLoading()
{
    static constexpr size_t GIF_WIDTH( 320 );
    static constexpr size_t GIF_HEIGHT( 180 );

    mlblLoading.setParent( this );
    mlblLoading.setFixedSize( GIF_WIDTH, GIF_HEIGHT );

    /// Центровка
    mlblLoading.move(( this->width() / 2 ) - GIF_WIDTH / 2,
                    ( this->height() / 2 ) - GIF_HEIGHT / 2);

    mlblLoading.setMovie( &mMovieLoading );
    mMovieLoading.start();
    mlblLoading.hide();
}

void windowGUI::delegateExecRealWork( eTypeOfActions action )
{
    /// Не пускать, если поток еще в процессе
    if( !checkEndThreadFlag() )
        return;

    /// Открытие файла для записи
    if( ERR_STATUS_SUCCESS != mFileWorker.updateWriteFile("./temporary" ) )
    {
        mJouarnalModel.insertString( "Ошибка открытия файла!" );
    }
    else
    {
        mJouarnalModel.insertString( "Открыт файл для записи" );
        mJouarnalModel.insertString( "Выполнение работы..." );

        /// Запуск потока дабы не блокировать GUI
        mIsThreadEnd = false;

        std::thread thread( [ this, action ]( void )
        {
            /// Запуск выполнения алгоритма
            auto [ newName, applyStatus ] =
                    mFileWorker.applyAlgorithm(
                        *mmAlgorithms.at( eTypeOfComprAlgorithm( mpUI->cbAlgorithmType->currentIndex() ) ),
                        action );

            /// В конце поднятие флага
            mIsThreadEnd = true;
            threadEnding( applyStatus, newName );
        });

        /// Не дожидаться
        thread.detach();

        /// Показать гифку загрузки
        mlblLoading.show();
    }
}

bool windowGUI::checkEndThreadFlag( void )
{
    if( !mIsThreadEnd )
        mJouarnalModel.insertString( "Дождитесь завершения работы..." );

    return mIsThreadEnd;
}

void windowGUI::on_btnOpenFile_clicked( void )
{
    /// Не пускать, если поток еще в процессе
    if( !checkEndThreadFlag() )
        return;

    /// Выбор файла пользователем
    QString fileName = QFileDialog::getOpenFileName( this, "Выбор файла", "." );
    mpUI->path2File->setText( fileName );

    if( !fileName.isEmpty() )
    {
        /// Установка файла, как файл для чтения для mFileWorker
        eErrStatus isOpenGood(
                    mFileWorker.updateReadFile( mpUI->path2File->text().toStdString() ) );

        mJouarnalModel.insertString( ERR_STATUS_SUCCESS == isOpenGood
                                     ? "Открыт файл для чтения:\n" + fileName
                                     : "Ошибка открытия файла! " + fileName );

        mpUI->gbAlg->setEnabled( true );
    }
    else
    {
        mpUI->gbAlg->setEnabled( false );
        mJouarnalModel.insertString( "Файл не выбран" );
    }
}

void windowGUI::on_btnCompress_clicked( void )
{
    delegateExecRealWork( ACT_TYPE_COMPR );
}

void windowGUI::on_btnDecompress_clicked( void )
{
    delegateExecRealWork( ACT_TYPE_DECOMPR );
}

