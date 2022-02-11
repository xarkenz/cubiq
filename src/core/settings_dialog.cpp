#include <QtWidgets>
#include <iostream>

#include "settings_dialog.h"


namespace Cubiq {

    #define MAKE_PAGE(id, name) \
        auto* p##id = new QListWidgetItem(tr(name), pageSelector); \
        p##id->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); \
        pageContainer->addWidget(new id##SettingsPage(settings));

    #define PAGE_SETUP \
        setMinimumSize(500, 400); \
        auto* mainLayout = new QVBoxLayout; \
        mainLayout->addStretch(1); \
        setLayout(mainLayout);

    #define MAKE_SECTION(id, name) \
        auto* s##id = new QGroupBox(tr(name)); \
        auto* l##id = new QFormLayout; \
        l##id->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint); \
        s##id->setLayout(l##id); \
        mainLayout->insertWidget(mainLayout->count() - 1, s##id);


    GeneralSettingsPage::GeneralSettingsPage(QSettings& settings) {
        PAGE_SETUP

        // Test code taken from official Qt website
        auto* configGroup = new QGroupBox(tr("Test Section"));

        auto* serverLabel = new QLabel(tr("Choice:"));
        auto* serverCombo = new QComboBox;
        serverCombo->addItem(tr("Option 1"));
        serverCombo->addItem(tr("Option 2"));
        serverCombo->addItem(tr("Option 3"));
        serverCombo->addItem(tr("Option 4"));
        serverCombo->addItem(tr("Option 5"));

        auto* serverLayout = new QHBoxLayout;
        serverLayout->addWidget(serverLabel);
        serverLayout->addWidget(serverCombo);

        auto* configLayout = new QVBoxLayout;
        configLayout->addLayout(serverLayout);
        configGroup->setLayout(configLayout);

        mainLayout->insertWidget(0, configGroup);
    }

    void GeneralSettingsPage::saveSettings(QSettings& settings) {
        // Nothing to save yet
    }


    DisplaySettingsPage::DisplaySettingsPage(QSettings& settings) :
            cfgSampleWidth(new QSpinBox) {
        PAGE_SETUP

        MAKE_SECTION(Advanced, "Advanced")

        cfgSampleWidth->setValue(settings.value("display/sampleWidth", 3).toInt());
        cfgSampleWidth->setRange(1, 20);
        cfgSampleWidth->setSuffix(" px");
        lAdvanced->addRow("Function sample width:", cfgSampleWidth);
    }

    void DisplaySettingsPage::saveSettings(QSettings& settings) {
        settings.setValue("display/sampleWidth", cfgSampleWidth->value());
    }


    SettingsDialog::SettingsDialog() : pageSelector(new QListWidget), pageContainer(new QStackedWidget) {
        MAKE_PAGE(General, "General")
        MAKE_PAGE(Display, "Display")

        connect(pageSelector, &QListWidget::currentItemChanged, this, &SettingsDialog::changePage);
        pageSelector->setCurrentRow(0);
        pageSelector->setMinimumWidth(100);
        pageSelector->setStyleSheet("QListWidget::item { padding: 10px; }");

        auto* splitLayout = new QHBoxLayout;
        splitLayout->addWidget(pageSelector);
        splitLayout->addWidget(pageContainer, 1);

        auto* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch(1);
        auto* bCancel = new QPushButton(tr("Cancel"));
        auto* bApply = new QPushButton(tr("Apply"));
        auto* bOK = new QPushButton(tr("OK"));
        connect(bCancel, &QAbstractButton::clicked, this, &QWidget::close);
        connect(bApply, &QAbstractButton::clicked, this, &SettingsDialog::save);
        connect(bOK, &QAbstractButton::clicked, this, &SettingsDialog::saveClose);
        buttonLayout->addWidget(bCancel);
        buttonLayout->addWidget(bApply);
        buttonLayout->addWidget(bOK);

        auto* mainLayout = new QVBoxLayout;
        mainLayout->addLayout(splitLayout);
        mainLayout->addSpacing(12);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
        setWindowTitle(tr("Settings"));
    }


    void SettingsDialog::changePage(QListWidgetItem* toPage, QListWidgetItem* fromPage) {
        if (!toPage) toPage = fromPage;
        pageContainer->setCurrentIndex(pageSelector->row(toPage));
    }

    void SettingsDialog::save() {
        for (int i = 0; i < pageContainer->count(); ++i) {
            ((SettingsPage*) pageContainer->widget(i))->saveSettings(settings);
        }
    }

    void SettingsDialog::saveClose() {
        // Hopefully self-explanatory
        save();
        close();
    }

}