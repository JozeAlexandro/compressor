/** ****************************************************************************
 * \file windowGUI.h
 *
 * \defgroup WindowGUI Графический интерфейс
 * @{
 *
 * \brief Модуль графического интерфейса программы
 *
 * \details Служит для обеспечения взаимодействия пользователя с программой.
 * Графические объекты созданы с помощью библиотеки Qt ( QtDesigner, Widgets )
 *
 * Реализован с поиощью класса \ref windowGUI
 * ****************************************************************************/

#ifndef WINDOWGUI_H
#define WINDOWGUI_H

#include <QDialog> /// Qt класс диалогового окна
#include <QLabel> /// Qt класс метки
#include <QMovie> /// Qt класс для воспроизведения анимаций
#include <map> /// Контейнер отображения
#include "cFileWorker/h/cFileWorker.h" /// Класс для работы с файлами
#include "libJournalView.h" /// Объект модели для представления журнала
#include <memory> /// Умные указатели
#include "common.h" /// Общие константы программы
#include "algorithm/cAbstractAlgorithm/h/cAbstractAlgorithm.h" /// Интерфейс алгоритмов


QT_BEGIN_NAMESPACE
namespace Ui { class windowGUI; }
QT_END_NAMESPACE

/// \brief Класс, реализующий GUI
/// \class windowGUI
class windowGUI final : public QDialog
{
    // Для использования сигналов-слотов
    Q_OBJECT

public:
    /// \brief Конструктор класса
    /// \param [in] parent Родитель для объекта
    windowGUI( QWidget *parent = nullptr );

    /// \brief Деструктор класса
    ~windowGUI( void );


signals:
    /// \brief Сигнал Qt для скрытия GIF бесконечной загрузки
    void signalHideGif();

private slots:
    /// \brief Клик по кнопке выбора файла
    void on_btnOpenFile_clicked( void );

    /// \brief Клик по кнопке "сжать"
    ///
    /// \details Применит выбранный алгоритм к выбранному файлу для сжатия.
    /// Во время сжатия отобразит gif бесконечной загрузки
    void on_btnCompress_clicked( void );

    /// \brief Клик по кнопке "разархивирорвать"
    ///
    /// \details Применит выбранный алгоритм к выбранному файлу для разархивирования.
    /// Во время сжатия отобразит gif бесконечной загрузки
    void on_btnDecompress_clicked( void );

private:
    /// \brief Заполнить отображение mmAlgorithms
    void fillCtrtAlgorithms( void );

    /// \brief Метод, вызывающийся при завершении потоков обработки
    /// (при завершении компрессии/декомпрессии)
    ///
    /// \details Вывод результата выполнения алгоритма в лог, скрытие
    /// гифки бесконечной загрузки
    ///
    /// \param [in] status Результат выполнения алгоритма
    /// \param [in] newName Результирующий файл
    void threadEnding( eErrStatus status, const std::string &newName );

    /// \brief Инициализация объекта гифки бесконечной загрузки
    void initGifLoading( void );

    /// \brief Делегировать выполнение реальной работы в бэк
    /// \param [in] action Тип действия
    void delegateExecRealWork( eTypeOfActions action );

    /// \brief Проверка флага состояния потоков
    /// \return true - Флаг поднят, false - опущен
    bool checkEndThreadFlag( void );

    /// \brief Указатель на графические объекты, созданные в режиме дизайна
    /// Qt Widget
    Ui::windowGUI *mpUI;

    /// \brief Объект для работы с файлами ОС
    cFileWorker mFileWorker;

    /// \brief Псевдоним для отображения типа алгоритма на unuque ptr от
    /// объекта алгоритма
    /// \typedef mapAlgType2UniqPtrAlg_t
    using mapAlgType2UniqPtrAlg_t = std::map< eTypeOfComprAlgorithm,
                                              std::unique_ptr< cAbstractAlgorithm > >;

    /// \brief Отображение типа алгоритма по его номеру на сам объект,
    /// реализующий алгоритм
    mapAlgType2UniqPtrAlg_t mmAlgorithms;

    /// \brief Объект модели для представления журнала
    cJournalModel mJouarnalModel;

    /// \brief Флаг, отвечающий за состояние потоков обработчиков.
    /// Если все потоки выполнены - флаг = true, иначе - false
    bool mIsThreadEnd = true;

    /// \brief Метка загрузки для помещения в нее объекта QMovie
    QLabel mlblLoading;
    /// \brief Объект для отображения бесконечной загрузки
    QMovie mMovieLoading;
};

/// @}

#endif // WINDOWGUI_H
