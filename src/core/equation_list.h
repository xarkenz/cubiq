#pragma once

#include <vector>

#include <QWidget>
#include <QLineEdit>


namespace Cubiq {

    class EquationListItem : public QLineEdit {

    };


    class EquationList : public QWidget {

    public:
        EquationList(QWidget* parent);

        void addItem(EquationListItem* item);

        std::vector<EquationListItem*>& getItemList();

    private:
        std::vector<EquationListItem*> items;

    };

}