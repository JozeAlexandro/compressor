/** ****************************************************************************
 * \file libJournalView.h
 * 
 * \defgroup LibJournalView Модель журнала
 * @{
 * \ingroup WindowGUI  
 * 
 * \brief Библиотечный модуль для работы с представлениями журналов
 *
 * Реализован с поиощью класса \ref cJournalModel
 * ****************************************************************************/

#ifndef C_JOURNAL_VIEW_H
#define C_JOURNAL_VIEW_H

#ifdef Q_OS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

#include <QAbstractListModel>

extern "C++"
{
/// \brief Класс, реализующий модель для представлений
/// \class cJournalModel
class cJournalModel final : public QAbstractListModel
{
    Q_OBJECT
public:
    /** ************************************************************************
     * \brief Конструктор класса
     *
     * \param strings [in] Список строк для инициализации
     * \param parent  [in] Имя родительского класса
     * ************************************************************************/
    MY_EXPORT cJournalModel(const QStringList &strings,
                            QObject *parent = nullptr) noexcept;

    /** *********** *************************************************************
     * \brief Конструктор класса
     *
     * \param parent [in] Имя родительского класса
     * ************************************************************************/
    MY_EXPORT cJournalModel(QObject *parent = nullptr ) noexcept;

    /** ************************************************************************
     * \brief Переопределенный метод. Возвращает количество строк
     * ************************************************************************/
    MY_EXPORT int rowCount(const QModelIndex &parent) const override;

    /** ************************************************************************
     * \brief Переопределенный метод. Возвращает данные, хранящиеся под данной
     * ролью для элемента на который ссылается индекс
     * ************************************************************************/
    MY_EXPORT QVariant data(const QModelIndex &index, int role) const override;

    /** ************************************************************************
     * \brief Добавление новой строки
     *
     * \param str [in] Строка, необходимая для добавления
     * ************************************************************************/
    MY_EXPORT void insertString(QString str) noexcept;

    /** ************************************************************************
     * \brief Добавление новых строк из контейнера QStringList
     *
     * \param str [in] Набор строк, необходимых для добавления
     * ************************************************************************/
    MY_EXPORT void insertString(QStringList str) noexcept;

    /** ************************************************************************
     * \brief Получение копии всех данных, хранящихся в контейнере
     *
     * \return копия stringList
     * ************************************************************************/
    MY_EXPORT QStringList getStringList() const noexcept;

    /** ************************************************************************
     * \brief Очистка контейнера
     * ************************************************************************/
    MY_EXPORT void clear() noexcept;

private:
    /** ************************************************************************
     * \brief Контейнер для хранения строк
     * ************************************************************************/
    QStringList stringList;
};
}

/// @}
#endif // C_JOURNAL_VIEW_H
