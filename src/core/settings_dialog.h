#pragma once

#include <QWidget>
#include <QDialog>
#include <QSettings>


class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QSpinBox;


namespace Cubiq {

    class SettingsPage : public QWidget {
    Q_OBJECT

    public:
        virtual void saveSettings(QSettings& settings) = 0;

    };


    class GeneralSettingsPage : public SettingsPage {
    Q_OBJECT

    public:
        explicit GeneralSettingsPage(QSettings& settings);

        void saveSettings(QSettings& settings);

    };


    class DisplaySettingsPage : public SettingsPage {
    Q_OBJECT

    public:
        explicit DisplaySettingsPage(QSettings& settings);

        void saveSettings(QSettings& settings);

    private:
        QSpinBox* cfgSampleWidth;

    };


    class SettingsDialog : public QDialog {
    Q_OBJECT

    public:
        SettingsDialog();

    public slots:

        void changePage(QListWidgetItem* toPage, QListWidgetItem* fromPage);

        void save();

        void saveClose();

    private:
        QListWidget* pageSelector;
        QStackedWidget* pageContainer;

        QSettings settings;

    };

}