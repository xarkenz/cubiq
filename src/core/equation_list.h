#pragma once

#include <vector>

#include <QWidget>
#include <QLineEdit>


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
